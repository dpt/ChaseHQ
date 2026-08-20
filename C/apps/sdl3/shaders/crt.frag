#version 450

/* GLSL/SPIR-V port of the Metal CRT fragment shader (chq_crt_fragment_msl in
 * CRTShader.c) for the Linux/Vulkan SDL_GPU backend. Keep the two in sync --
 * see CRTShader.c's header comment for what each effect does and why.
 *
 * Resource bindings follow SDL_GPU's fixed SPIR-V convention for fragment
 * stage resources: samplers/textures at set 2, uniform buffers at set 3.
 */

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 out_colour;

layout(set = 2, binding = 0) uniform sampler2D tex;

layout(set = 3, binding = 0) uniform Params
{
  float curvature;
  float bloom_threshold;
  float bloom_intensity;
  float brightness;
  float contrast;
  float saturation;
  float scanline_intensity;
  float vignette_strength;
  float chroma_bleed;
  float glitch;
  float time;
}
p;

float chq_hash(float x)
{
  return fract(sin(x * 12.9898) * 43758.5453);
}

void main()
{
  vec2 coord;
  vec2 uv0;
  ivec2 tex_size;
  float tick;
  float band;
  float seed;
  vec2 edge;
  float edge_mask;
  vec2 uvc;
  vec2 texel;
  vec4 c;
  vec3 W;
  vec3 bleed;
  float ylum;
  vec3 chroma;
  vec3 bloom;
  vec3 bc;
  vec3 bn;
  vec3 bs;
  vec3 be;
  vec3 bw;
  float flicker;
  float lum;
  float scan_phase;
  float scan_width;
  float scan_atten;
  float scan;
  float adaptive;
  vec2 d;
  float vignette;

  /* curveRemapUV: barrel distortion via dot(coord,coord) radial distance. */
  coord = uv * 2.0 - 1.0;
  coord *= 1.0 + dot(coord, coord) * p.curvature;
  uv0 = coord * 0.5 + 0.5;

  /* Line tear: seed each source scanline separately, reseed at the
   * Spectrum's 50Hz frame rate and shift the lines whose seed clears the
   * threshold. step(0.9) picks roughly one line in ten, and picks a
   * different ten every frame, so no torn line survives into the next.
   */
  /* Branch on p.glitch -- see the root-cause writeup in CRTShader.c's GLES
   * shader (a runtime-uniform 0.0 can't be constant-folded the way a
   * compile-time 0.0 can, so a NaN from chq_hash's internal sin() survives
   * "* p.glitch"). Vulkan/SPIR-V is not known to hit the underlying mediump
   * sin() bug, but the branch is kept in sync so the three shaders' math
   * stays identical.
   */
  tick = mod(floor(p.time * 50.0), 2048.0);
  band = 0.0;
  seed = 0.0;
  if (p.glitch > 0.0)
  {
    tex_size = textureSize(tex, 0);
    band = floor(uv0.y * float(tex_size.y));
    seed = chq_hash(band * 78.233 + tick * 37.719);
    uv0.x += (chq_hash(seed * 91.0) - 0.5) * 0.005 * p.glitch * step(0.9, seed);
  }

  /* Soft edge: smoothstep border fade instead of a hard uv-bounds cutoff,
   * which otherwise aliases into a jagged edge along the curvature.
   */
  edge = smoothstep(vec2(0.0), vec2(0.005), uv0) *
         smoothstep(vec2(0.0), vec2(0.005), 1.0 - uv0);
  edge_mask = edge.x * edge.y;
  uvc = clamp(uv0, 0.0, 1.0);
  texel = vec2(1.0 / 256.0, 1.0 / 192.0);
  c = texture(tex, uvc);

  /* PAL colour bleed: chroma was broadcast at a fraction of the luma
   * bandwidth, so colour smears horizontally while edges stay sharp. Four
   * taps to the left with decaying weights -- the decoder lags the signal,
   * so the smear trails to the right. Luma is taken from the centre tap
   * only.
   */
  W = vec3(0.299, 0.587, 0.114);
  bleed = c.rgb * 0.4;
  bleed += texture(tex, clamp(uvc - vec2(texel.x, 0.0), 0.0, 1.0)).rgb * 0.3;
  bleed +=
      texture(tex, clamp(uvc - vec2(texel.x * 2.0, 0.0), 0.0, 1.0)).rgb * 0.2;
  bleed +=
      texture(tex, clamp(uvc - vec2(texel.x * 3.0, 0.0), 0.0, 1.0)).rgb * 0.1;
  ylum = dot(c.rgb, W);
  chroma = mix(c.rgb - ylum, bleed - dot(bleed, W), p.chroma_bleed);
  c.rgb = ylum + chroma;
  c *= edge_mask;

  /* sampleBloom: threshold-gated centre + 4-tap cross sample. */
  bloom = vec3(0.0);
  bc = c.rgb;
  bn = texture(tex, clamp(uvc + vec2(0.0, texel.y), 0.0, 1.0)).rgb;
  bs = texture(tex, clamp(uvc - vec2(0.0, texel.y), 0.0, 1.0)).rgb;
  be = texture(tex, clamp(uvc + vec2(texel.x, 0.0), 0.0, 1.0)).rgb;
  bw = texture(tex, clamp(uvc - vec2(texel.x, 0.0), 0.0, 1.0)).rgb;
  if (max(bc.r, max(bc.g, bc.b)) > p.bloom_threshold) bloom += bc;
  if (max(bn.r, max(bn.g, bn.b)) > p.bloom_threshold) bloom += bn;
  if (max(bs.r, max(bs.g, bs.b)) > p.bloom_threshold) bloom += bs;
  if (max(be.r, max(be.g, be.b)) > p.bloom_threshold) bloom += be;
  if (max(bw.r, max(bw.g, bw.b)) > p.bloom_threshold) bloom += bw;
  c.rgb += bloom * p.bloom_intensity;

  /* brightness / contrast / saturation. */
  c.rgb = (c.rgb - 0.5) * p.contrast + 0.5;

  /* Mains flicker: brightness wobble reseeded 50 times a second, the rate
   * an unsynchronised 50Hz display would beat at.
   */
  flicker = 1.0;
  if (p.glitch > 0.0)
    flicker = 1.0 + (chq_hash(tick * 91.7) - 0.5) * 0.06 * p.glitch;
  c.rgb *= p.brightness * flicker;
  lum = dot(c.rgb, vec3(0.299, 0.587, 0.114));
  c.rgb = mix(vec3(lum), c.rgb, p.saturation);

  /* scanlines, intensity adapted to local luminance. Band-limit via fwidth
   * so the scanline amplitude fades out (rather than aliasing into moire)
   * once curvature or downscaling makes a pixel span several source lines.
   */
  scan_phase = uv0.y * 384.0 * 3.14159265;
  scan_width = fwidth(scan_phase);
  scan_atten =
      scan_width > 0.0001 ? clamp(sin(scan_width * 0.5) / (scan_width * 0.5), 0.0, 1.0) : 1.0;
  scan = sin(scan_phase) * scan_atten * 0.5 + 0.5;
  adaptive = mix(p.scanline_intensity, p.scanline_intensity * (1.0 - lum), 0.5);
  c.rgb *= 1.0 - adaptive * scan;

  /* vignetteApprox: Chebyshev (max-component) distance falloff. */
  d = abs(uv0 - 0.5) * 2.0;
  vignette = 1.0 - max(d.x, d.y) * max(d.x, d.y) * p.vignette_strength;
  c.rgb *= vignette;

  out_colour = c;
}
