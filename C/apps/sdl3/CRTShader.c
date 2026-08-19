/**
 * CRTShader.c
 *
 * This file is part of "Chase H.Q. in C".
 *
 * This project recreates the ZX Spectrum version of the chase-and-smash game
 * "Chase H.Q." in portable C code. It is free software provided without
 * warranty in the interests of education and software preservation.
 *
 * The arcade original was created by Taito Corporation in 1988. It was then
 * ported to the ZX Spectrum by Ocean Software Limited and released in 1989.
 *
 * The original game and design is copyright (c) 1988 Taito Corporation.
 * The ZX Spectrum version is copyright (c) 1989 Ocean Software Limited.
 * The recreated version is copyright (c) 2023-2026 David Thomas.
 */

#include <stdio.h>
#include <string.h>

#include "CRTShader.h"

#if defined(CHQ_CRT_SHADER_GLES)
#include <SDL3/SDL_opengles2.h>
#endif

#if defined(CHQ_CRT_SHADER_GLES)

/* Emscripten build: WebGL1 (GLES2) via a raw GL context, not SDL's GPU API --
 * SDL3 GPU has no stable WebGPU backend yet. Shader source compiled at
 * runtime by the browser's GL driver, same as the MSL path below; no offline
 * compile step. Fullscreen triangle uses a real VBO -- GLES2 has no
 * gl_VertexID equivalent to build it in-shader like the MSL/SPIR-V paths do.
 */

static const char *const chq_crt_vertex_gles =
    "attribute vec2 a_pos;\n"
    "attribute vec2 a_uv;\n"
    "varying vec2 v_uv;\n"
    "void main() {\n"
    "  v_uv = a_uv;\n"
    "  gl_Position = vec4(a_pos, 0.0, 1.0);\n"
    "}\n";

/* Direct GLSL ES 100 port of the MSL fragment shader below -- same effect,
 * same uniform order (kept parallel here for review, not loaded from
 * chq_CRT_params_t directly: GLES2 has no uniform buffer objects, so each
 * field is its own uniform, set by name in chq_CRT_shader_render).
 */
static const char *const chq_crt_fragment_gles =
    "#extension GL_OES_standard_derivatives : enable\n"
    "precision mediump float;\n"
    "varying vec2 v_uv;\n"
    "uniform sampler2D u_tex;\n"
    "uniform vec2 u_texel;\n"
    "uniform float u_curvature;\n"
    "uniform float u_bloomThreshold;\n"
    "uniform float u_bloomIntensity;\n"
    "uniform float u_brightness;\n"
    "uniform float u_contrast;\n"
    "uniform float u_saturation;\n"
    "uniform float u_scanlineIntensity;\n"
    "uniform float u_vignetteStrength;\n"
    "uniform float u_chromaBleed;\n"
    "uniform float u_glitch;\n"
    "uniform float u_time;\n"
    "float chq_hash(float x) {\n"
    "  return fract(sin(x * 12.9898) * 43758.5453);\n"
    "}\n"
    "void main() {\n"
    "  vec2 coord = v_uv * 2.0 - 1.0;\n"
    "  coord *= 1.0 + dot(coord, coord) * u_curvature;\n"
    "  vec2 uv = coord * 0.5 + 0.5;\n"
    "  float band = floor(uv.y / u_texel.y);\n"
    "  float seed = chq_hash(band * 78.233 + floor(u_time * 50.0) * 37.719);\n"
    "  uv.x += (chq_hash(seed * 91.0) - 0.5) * 0.005 * u_glitch *\n"
    "          step(0.9, seed);\n"
    "  vec2 edge = smoothstep(vec2(0.0), vec2(0.005), uv) *\n"
    "              smoothstep(vec2(0.0), vec2(0.005), 1.0 - uv);\n"
    "  float edgeMask = edge.x * edge.y;\n"
    "  vec2 uvc = clamp(uv, 0.0, 1.0);\n"
    "  vec4 c = texture2D(u_tex, uvc);\n"
    "  vec3 W = vec3(0.299, 0.587, 0.114);\n"
    "  vec3 bleed = c.rgb * 0.4;\n"
    "  bleed += texture2D(u_tex, clamp(uvc - vec2(u_texel.x, 0.0), 0.0, 1.0)).rgb * 0.3;\n"
    "  bleed += texture2D(u_tex, clamp(uvc - vec2(u_texel.x * 2.0, 0.0), 0.0, 1.0)).rgb * 0.2;\n"
    "  bleed += texture2D(u_tex, clamp(uvc - vec2(u_texel.x * 3.0, 0.0), 0.0, 1.0)).rgb * 0.1;\n"
    "  float ylum = dot(c.rgb, W);\n"
    "  vec3 chroma = mix(c.rgb - ylum, bleed - dot(bleed, W), u_chromaBleed);\n"
    "  c.rgb = ylum + chroma;\n"
    "  c *= edgeMask;\n"
    "  vec3 bloom = vec3(0.0);\n"
    "  vec3 bc = c.rgb;\n"
    "  vec3 bn = texture2D(u_tex, clamp(uvc + vec2(0.0, u_texel.y), 0.0, 1.0)).rgb;\n"
    "  vec3 bs = texture2D(u_tex, clamp(uvc - vec2(0.0, u_texel.y), 0.0, 1.0)).rgb;\n"
    "  vec3 be = texture2D(u_tex, clamp(uvc + vec2(u_texel.x, 0.0), 0.0, 1.0)).rgb;\n"
    "  vec3 bw = texture2D(u_tex, clamp(uvc - vec2(u_texel.x, 0.0), 0.0, 1.0)).rgb;\n"
    "  if (max(bc.r, max(bc.g, bc.b)) > u_bloomThreshold) bloom += bc;\n"
    "  if (max(bn.r, max(bn.g, bn.b)) > u_bloomThreshold) bloom += bn;\n"
    "  if (max(bs.r, max(bs.g, bs.b)) > u_bloomThreshold) bloom += bs;\n"
    "  if (max(be.r, max(be.g, be.b)) > u_bloomThreshold) bloom += be;\n"
    "  if (max(bw.r, max(bw.g, bw.b)) > u_bloomThreshold) bloom += bw;\n"
    "  c.rgb += bloom * u_bloomIntensity;\n"
    "  c.rgb = (c.rgb - 0.5) * u_contrast + 0.5;\n"
    "  float flicker = 1.0 + (chq_hash(floor(u_time * 50.0) * 91.7) - 0.5) *\n"
    "                        0.06 * u_glitch;\n"
    "  c.rgb *= u_brightness * flicker;\n"
    "  float lum = dot(c.rgb, vec3(0.299, 0.587, 0.114));\n"
    "  c.rgb = mix(vec3(lum), c.rgb, u_saturation);\n"
    "  float scanPhase = uv.y * 384.0 * 3.14159265;\n"
    "  float scanWidth = fwidth(scanPhase);\n"
    "  float scanAtten = scanWidth > 0.0001 ? clamp(sin(scanWidth * 0.5) / (scanWidth * 0.5), 0.0, 1.0) : 1.0;\n"
    "  float scan = sin(scanPhase) * scanAtten * 0.5 + 0.5;\n"
    "  float adaptive = mix(u_scanlineIntensity, u_scanlineIntensity * (1.0 - lum), 0.5);\n"
    "  c.rgb *= 1.0 - adaptive * scan;\n"
    "  vec2 d = abs(uv - 0.5) * 2.0;\n"
    "  float vignette = 1.0 - max(d.x, d.y) * max(d.x, d.y) * u_vignetteStrength;\n"
    "  c.rgb *= vignette;\n"
    "  gl_FragColor = c;\n"
    "}\n";

#elif defined(CHQ_CRT_SHADER_SPIRV)

/* Linux/Vulkan build: shader bytecode is precompiled from
 * apps/sdl3/shaders/crt.{vert,frag} by glslangValidator at build time and
 * embedded as byte arrays (see CMakeLists.txt's crt_shader_spirv target and
 * cmake/EmbedSPIRV.cmake). Keep the GLSL sources in sync with the MSL below
 * if the effect changes.
 */
#include "CRTShaderSPIRV.h"

#define CHQ_GPU_SHADER_FORMAT SDL_GPU_SHADERFORMAT_SPIRV
#define CHQ_GPU_ENTRYPOINT_VS "main"
#define CHQ_GPU_ENTRYPOINT_FS "main"

#else

/* MSL source is handed to SDL_CreateGPUShader() as SDL_GPU_SHADERFORMAT_MSL
 * and compiled at runtime by Metal; no offline .metallib build step. This
 * is macOS/iOS-only (Metal backend only).
 *
 * Fullscreen triangle is generated in the vertex shader from vertex_id, so
 * no vertex buffer is needed.
 */

#define CHQ_GPU_SHADER_FORMAT SDL_GPU_SHADERFORMAT_MSL
#define CHQ_GPU_ENTRYPOINT_VS "vs_main"
#define CHQ_GPU_ENTRYPOINT_FS "fs_main"
static const char *const chq_crt_vertex_msl =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "struct VSOut { float4 position [[position]]; float2 uv; };\n"
    "vertex VSOut vs_main(uint vid [[vertex_id]]) {\n"
    "  float2 pos[3] = { float2(-1,-1), float2(3,-1), float2(-1,3) };\n"
    "  float2 uv[3]  = { float2(0,1),  float2(2,1),  float2(0,-1) };\n"
    "  VSOut out;\n"
    "  out.position = float4(pos[vid], 0.0, 1.0);\n"
    "  out.uv = uv[vid];\n"
    "  return out;\n"
    "}\n";

/* Port of a reference Three.js/GLSL CRTShader. Its uniforms (curvature,
 * bloom threshold/intensity, brightness/contrast/saturation, scanline
 * count/intensity, adaptive intensity, vignette strength) are hardcoded
 * below rather than plumbed through as tunable uniforms - none of them
 * change at runtime in this game; the constants have been re-tuned by eye
 * against this game's screen and no longer match the reference's own
 * defaults.
 * The reference's rgbShift channel-separation effect is dropped - it
 * defaults to 0.0 (a no-op in the source shader too). Its time-driven
 * flicker is kept, folded into the `glitch` knob along with an occasional
 * single-scanline tear; both are driven by the `time` field the renderer
 * fills in each frame.
 * Must match chq_CRT_params_t in CRTShader.h field-for-field: plain floats,
 * same order, no padding.
 */
static const char *const chq_crt_fragment_msl =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "struct VSOut { float4 position [[position]]; float2 uv; };\n"
    "struct Params {\n"
    "  float curvature;\n"
    "  float bloomThreshold;\n"
    "  float bloomIntensity;\n"
    "  float brightness;\n"
    "  float contrast;\n"
    "  float saturation;\n"
    "  float scanlineIntensity;\n"
    "  float vignetteStrength;\n"
    "  float chromaBleed;\n"
    "  float glitch;\n"
    "  float time;\n"
    "};\n"
    /* Cheap hash: fract(sin(x) * large). Good enough for flicker and tear
     * seeds; no noise texture or per-frame random uniform needed.
     */
    "static inline float chq_hash(float x) {\n"
    "  return fract(sin(x * 12.9898) * 43758.5453);\n"
    "}\n"
    "fragment float4 fs_main(VSOut in [[stage_in]],\n"
    "                        texture2d<float> tex [[texture(0)]],\n"
    "                        sampler samp [[sampler(0)]],\n"
    "                        constant Params& p [[buffer(0)]]) {\n"
    // curveRemapUV: barrel distortion via dot(coord,coord) radial distance.
    "  float2 coord = in.uv * 2.0 - 1.0;\n"
    "  coord *= 1.0 + dot(coord, coord) * p.curvature;\n"
    "  float2 uv = coord * 0.5 + 0.5;\n"
    /* Line tear: seed each source scanline separately, reseed at the Spectrum's
     * 50Hz frame rate and shift the lines whose seed clears the threshold.
     * step(0.9) picks roughly one line in ten, and picks a different ten every
     * frame, so no torn line survives into the next.
     */
    "  float band = floor(uv.y * float(tex.get_height()));\n"
    "  float seed = chq_hash(band * 78.233 + floor(p.time * 50.0) * 37.719);\n"
    "  uv.x += (chq_hash(seed * 91.0) - 0.5) * 0.005 * p.glitch *\n"
    "          step(0.9, seed);\n"
    /* Soft edge: smoothstep border fade instead of a hard uv-bounds cutoff,
     * which otherwise aliases into a jagged edge along the curvature.
     */
    "  float2 edge = smoothstep(float2(0.0), float2(0.005), uv) *\n"
    "                smoothstep(float2(0.0), float2(0.005), 1.0 - uv);\n"
    "  float edgeMask = edge.x * edge.y;\n"
    "  float2 uvc = clamp(uv, 0.0, 1.0);\n"
    "  float2 texel = float2(1.0 / 256.0, 1.0 / 192.0);\n"
    "  float4 c = tex.sample(samp, uvc);\n"
    /* PAL colour bleed: chroma was broadcast at a fraction of the luma
     * bandwidth, so colour smears horizontally while edges stay sharp. Four
     * taps to the left with decaying weights - the decoder lags the signal, so
     * the smear trails to the right. Luma is taken from the centre tap only.
     */
    "  float3 W = float3(0.299, 0.587, 0.114);\n"
    "  float3 bleed = c.rgb * 0.4;\n"
    "  bleed += tex.sample(samp, clamp(uvc - float2(texel.x, 0.0), 0.0, "
    "1.0)).rgb * 0.3;\n"
    "  bleed += tex.sample(samp, clamp(uvc - float2(texel.x * 2.0, 0.0), 0.0, "
    "1.0)).rgb * 0.2;\n"
    "  bleed += tex.sample(samp, clamp(uvc - float2(texel.x * 3.0, 0.0), 0.0, "
    "1.0)).rgb * 0.1;\n"
    "  float ylum = dot(c.rgb, W);\n"
    "  float3 chroma = mix(c.rgb - ylum, bleed - dot(bleed, W), "
    "p.chromaBleed);\n"
    "  c.rgb = ylum + chroma;\n"
    "  c *= edgeMask;\n"
    // sampleBloom: threshold-gated centre + 4-tap cross sample.
    "  float3 bloom = float3(0.0);\n"
    "  float3 bc = c.rgb;\n"
    "  float3 bn = tex.sample(samp, clamp(uvc + float2(0.0, texel.y), 0.0, "
    "1.0)).rgb;\n"
    "  float3 bs = tex.sample(samp, clamp(uvc - float2(0.0, texel.y), 0.0, "
    "1.0)).rgb;\n"
    "  float3 be = tex.sample(samp, clamp(uvc + float2(texel.x, 0.0), 0.0, "
    "1.0)).rgb;\n"
    "  float3 bw = tex.sample(samp, clamp(uvc - float2(texel.x, 0.0), 0.0, "
    "1.0)).rgb;\n"
    "  if (max(bc.r, max(bc.g, bc.b)) > p.bloomThreshold) bloom += bc;\n"
    "  if (max(bn.r, max(bn.g, bn.b)) > p.bloomThreshold) bloom += bn;\n"
    "  if (max(bs.r, max(bs.g, bs.b)) > p.bloomThreshold) bloom += bs;\n"
    "  if (max(be.r, max(be.g, be.b)) > p.bloomThreshold) bloom += be;\n"
    "  if (max(bw.r, max(bw.g, bw.b)) > p.bloomThreshold) bloom += bw;\n"
    "  c.rgb += bloom * p.bloomIntensity;\n"
    // brightness / contrast / saturation.
    "  c.rgb = (c.rgb - 0.5) * p.contrast + 0.5;\n"
    /* Mains flicker: brightness wobble reseeded 50 times a second, the rate an
     * unsynchronised 50Hz display would beat at.
     */
    "  float flicker = 1.0 + (chq_hash(floor(p.time * 50.0) * 91.7) - 0.5) *\n"
    "                        0.06 * p.glitch;\n"
    "  c.rgb *= p.brightness * flicker;\n"
    "  float lum = dot(c.rgb, float3(0.299, 0.587, 0.114));\n"
    "  c.rgb = mix(float3(lum), c.rgb, p.saturation);\n"
    // scanlines, intensity adapted to local luminance. Band-limit via
    // fwidth so amplitude fades out (rather than aliasing into moire) once
    // curvature or downscaling makes a pixel span several source lines.
    "  float scanPhase = uv.y * 384.0 * 3.14159265;\n"
    "  float scanWidth = fwidth(scanPhase);\n"
    "  float scanAtten = scanWidth > 0.0001 ? saturate(sin(scanWidth * 0.5) "
    "/ (scanWidth * 0.5)) : 1.0;\n"
    "  float scan = sin(scanPhase) * scanAtten * 0.5 + 0.5;\n"
    "  float adaptive = mix(p.scanlineIntensity, p.scanlineIntensity * (1.0 - "
    "lum), 0.5);\n"
    "  c.rgb *= 1.0 - adaptive * scan;\n"
    // vignetteApprox: Chebyshev (max-component) distance falloff.
    "  float2 d = abs(uv - 0.5) * 2.0;\n"
    "  float vignette = 1.0 - max(d.x, d.y) * max(d.x, d.y) * "
    "p.vignetteStrength;\n"
    "  c.rgb *= vignette;\n"
    "  return c;\n"
    "}\n";

#endif /* CHQ_CRT_SHADER_SPIRV */

#if defined(CHQ_CRT_SHADER_GLES)

/* Uniform names, in the order their locations are cached into
 * shader->uniform_locs by chq_CRT_shader_create -- keep in sync with the
 * enum below and with the uniform declarations in chq_crt_fragment_gles.
 */
enum
{
  CHQ_U_TEX,
  CHQ_U_TEXEL,
  CHQ_U_CURVATURE,
  CHQ_U_BLOOM_THRESHOLD,
  CHQ_U_BLOOM_INTENSITY,
  CHQ_U_BRIGHTNESS,
  CHQ_U_CONTRAST,
  CHQ_U_SATURATION,
  CHQ_U_SCANLINE_INTENSITY,
  CHQ_U_VIGNETTE_STRENGTH,
  CHQ_U_CHROMA_BLEED,
  CHQ_U_GLITCH,
  CHQ_U_TIME,
  CHQ_U_COUNT
};

static const char *const chq_gles_uniform_names[CHQ_U_COUNT] = {
  "u_tex", "u_texel", "u_curvature", "u_bloomThreshold", "u_bloomIntensity",
  "u_brightness", "u_contrast", "u_saturation", "u_scanlineIntensity",
  "u_vignetteStrength", "u_chromaBleed", "u_glitch", "u_time",
};

/* Compiles one shader stage and checks the compile log; returns 0 (and
 * prints the log) on failure, matching the SDL_GPU paths' error handling.
 */
static GLuint chq_gles_compile(GLenum stage, const char *source)
{
  GLuint shader;
  GLint  compiled;

  shader = glCreateShader(stage);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled)
  {
    char log[512];
    glGetShaderInfoLog(shader, sizeof(log), NULL, log);
    fprintf(stderr, "Error: GLES shader compile: %s\n", log);
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

int chq_CRT_shader_create(chq_CRT_shader_t *shader,
                          SDL_Window       *window,
                          int               game_width,
                          int               game_height)
{
  GLuint vertex_shader;
  GLuint fragment_shader;
  GLint  linked;
  int    i;
  /* Fullscreen triangle: positions [-1,-1]..[3,3] extend past the clip
   * volume on two corners, which is fine -- the rasteriser clips it back to
   * the screen and the third corner lands exactly at the far edges, same
   * trick as the vertex_id-generated triangle in the MSL/SPIR-V paths.
   */
  static const float vertices[] = {
    -1.0f, -1.0f, 0.0f, 1.0f,
     3.0f, -1.0f, 2.0f, 1.0f,
    -1.0f,  3.0f, 0.0f, -1.0f,
  };

  (void) game_width;
  (void) game_height;

  memset(shader, 0, sizeof(*shader));

  shader->gl_context = SDL_GL_CreateContext(window);
  if (shader->gl_context == NULL)
  {
    fprintf(stderr, "Error: SDL_GL_CreateContext: %s\n", SDL_GetError());
    return 0;
  }

  vertex_shader = chq_gles_compile(GL_VERTEX_SHADER, chq_crt_vertex_gles);
  if (vertex_shader == 0)
    return 0;

  fragment_shader = chq_gles_compile(GL_FRAGMENT_SHADER, chq_crt_fragment_gles);
  if (fragment_shader == 0)
  {
    glDeleteShader(vertex_shader);
    return 0;
  }

  shader->program = glCreateProgram();
  glAttachShader(shader->program, vertex_shader);
  glAttachShader(shader->program, fragment_shader);
  glBindAttribLocation(shader->program, 0, "a_pos");
  glBindAttribLocation(shader->program, 1, "a_uv");
  glLinkProgram(shader->program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  glGetProgramiv(shader->program, GL_LINK_STATUS, &linked);
  if (!linked)
  {
    char log[512];
    glGetProgramInfoLog(shader->program, sizeof(log), NULL, log);
    fprintf(stderr, "Error: GLES program link: %s\n", log);
    return 0;
  }

  for (i = 0; i < CHQ_U_COUNT; i++)
    shader->uniform_locs[i] =
        glGetUniformLocation(shader->program, chq_gles_uniform_names[i]);

  glGenBuffers(1, &shader->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, shader->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenTextures(1, &shader->texture);
  glBindTexture(GL_TEXTURE_2D, shader->texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  return 1;
}

void chq_CRT_shader_render(chq_CRT_shader_t       *shader,
                           SDL_Window             *window,
                           zxspectrum_t           *zx,
                           int                     x,
                           int                     y,
                           int                     w,
                           int                     h,
                           int                     game_width,
                           int                     game_height,
                           const chq_CRT_params_t *params,
                           const unsigned char    *osd_mask,
                           const uint32_t         *override_pixels)
{
  const zx_frame_t *frame;
  const uint32_t   *pixels;
  uint32_t         *composited = NULL;

  SDL_GL_MakeCurrent(window, shader->gl_context);

  /* Composite the OSD mask on the CPU before upload, same as the SDL_GPU
   * path -- there is no SDL_Renderer here for the caller to draw an overlay
   * rect with. Needs a scratch copy since, unlike the GPU transfer buffer,
   * the game's screen buffer and the caller's override buffer are not ours
   * to write into.
   */
  if (override_pixels != NULL)
    pixels = override_pixels;
  else
  {
    frame  = zxspectrum_claim_screen(zx);
    pixels = frame->pixels;
  }

  if (osd_mask != NULL)
  {
    int i;

    composited = SDL_malloc((size_t) game_width * (size_t) game_height * 4);
    if (composited != NULL)
    {
      memcpy(composited, pixels, (size_t) game_width * (size_t) game_height * 4);
      for (i = 0; i < game_width * game_height; i++)
        if (osd_mask[i] == 1)
          composited[i] = 0xFF40FF40u; /* green interior */
        else if (osd_mask[i] == 2)
          composited[i] = 0xFF000000u; /* black outline */
      pixels = composited;
    }
    /* SDL_malloc failure: fall through and upload the frame without the
     * OSD overlay rather than crash on a null buffer. */
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, shader->texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, game_width, game_height, 0, GL_RGBA,
              GL_UNSIGNED_BYTE, pixels);

  if (override_pixels == NULL)
    zxspectrum_release_screen(zx);
  if (composited != NULL)
    SDL_free(composited);

  glViewport(x, y, w, h);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(shader->program);

  glUniform1i(shader->uniform_locs[CHQ_U_TEX], 0);
  glUniform2f(shader->uniform_locs[CHQ_U_TEXEL],
             1.0f / (float) game_width, 1.0f / (float) game_height);
  glUniform1f(shader->uniform_locs[CHQ_U_CURVATURE], params->curvature);
  glUniform1f(shader->uniform_locs[CHQ_U_BLOOM_THRESHOLD], params->bloom_threshold);
  glUniform1f(shader->uniform_locs[CHQ_U_BLOOM_INTENSITY], params->bloom_intensity);
  glUniform1f(shader->uniform_locs[CHQ_U_BRIGHTNESS], params->brightness);
  glUniform1f(shader->uniform_locs[CHQ_U_CONTRAST], params->contrast);
  glUniform1f(shader->uniform_locs[CHQ_U_SATURATION], params->saturation);
  glUniform1f(shader->uniform_locs[CHQ_U_SCANLINE_INTENSITY], params->scanline_intensity);
  glUniform1f(shader->uniform_locs[CHQ_U_VIGNETTE_STRENGTH], params->vignette_strength);
  glUniform1f(shader->uniform_locs[CHQ_U_CHROMA_BLEED], params->chroma_bleed);
  glUniform1f(shader->uniform_locs[CHQ_U_GLITCH], params->glitch);
  glUniform1f(shader->uniform_locs[CHQ_U_TIME], SDL_GetTicks() * 0.001f);

  glBindBuffer(GL_ARRAY_BUFFER, shader->vbo);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *) (2 * sizeof(float)));

  glDrawArrays(GL_TRIANGLES, 0, 3);

  SDL_GL_SwapWindow(window);
}

void chq_CRT_shader_destroy(chq_CRT_shader_t *shader, SDL_Window *window)
{
  (void) window;

  glDeleteTextures(1, &shader->texture);
  glDeleteBuffers(1, &shader->vbo);
  glDeleteProgram(shader->program);
  SDL_GL_DestroyContext(shader->gl_context);
}

#else

int chq_CRT_shader_create(chq_CRT_shader_t *shader,
                          SDL_Window       *window,
                          int               game_width,
                          int               game_height)
{
  SDL_GPUTextureCreateInfo          texture_info;
  SDL_GPUTransferBufferCreateInfo   transfer_info;
  SDL_GPUSamplerCreateInfo          sampler_info;
  SDL_GPUShaderCreateInfo           shader_info;
  SDL_GPUColorTargetDescription     color_target_desc;
  SDL_GPUGraphicsPipelineCreateInfo pipeline_info;
  SDL_GPUShader                    *vertex_shader;
  SDL_GPUShader                    *fragment_shader;

  memset(shader, 0, sizeof(*shader));

  shader->gpu = SDL_CreateGPUDevice(CHQ_GPU_SHADER_FORMAT, false, NULL);
  if (shader->gpu == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateGPUDevice: %s\n", SDL_GetError());
    return 0;
  }

  if (!SDL_ClaimWindowForGPUDevice(shader->gpu, window))
  {
    fprintf(stderr, "Error: SDL_ClaimWindowForGPUDevice: %s\n", SDL_GetError());
    SDL_DestroyGPUDevice(shader->gpu);
    shader->gpu = NULL;
    return 0;
  }

  /* Conv: on Linux (Wayland/X11), re-claiming this window for a new GPU
   * swapchain right after the SDL_Renderer swapchain that previously owned
   * it was torn down (F4 toggling back to the CRT backend) presents
   * successfully every frame but the compositor keeps showing the old
   * surface's last frame -- nothing on screen updates. Explicitly
   * (re)configuring the swapchain rather than relying on whatever it
   * defaulted to on this claim forces the compositor to pick up the new
   * surface. Metal/macOS doesn't need this, but it's harmless there too.
   */
  if (!SDL_SetGPUSwapchainParameters(shader->gpu, window,
                                     SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                     SDL_GPU_PRESENTMODE_VSYNC))
  {
    fprintf(stderr, "Error: SDL_SetGPUSwapchainParameters: %s\n", SDL_GetError());
    SDL_ReleaseWindowFromGPUDevice(shader->gpu, window);
    SDL_DestroyGPUDevice(shader->gpu);
    shader->gpu = NULL;
    return 0;
  }

  memset(&texture_info, 0, sizeof(texture_info));
  texture_info.type                 = SDL_GPU_TEXTURETYPE_2D;
  texture_info.format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
  texture_info.usage                = SDL_GPU_TEXTUREUSAGE_SAMPLER;
  texture_info.width                = game_width;
  texture_info.height               = game_height;
  texture_info.layer_count_or_depth = 1;
  texture_info.num_levels           = 1;
  texture_info.sample_count         = SDL_GPU_SAMPLECOUNT_1;

  shader->texture = SDL_CreateGPUTexture(shader->gpu, &texture_info);
  if (shader->texture == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateGPUTexture: %s\n", SDL_GetError());
    return 0;
  }

  memset(&transfer_info, 0, sizeof(transfer_info));
  transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  transfer_info.size  = game_width * game_height * 4;

  shader->transfer_buffer = SDL_CreateGPUTransferBuffer(shader->gpu, &transfer_info);
  if (shader->transfer_buffer == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateGPUTransferBuffer: %s\n", SDL_GetError());
    return 0;
  }

  /* Linear filtering: the CRT shader (curvature, bloom, scanlines) reads
   * this softer look as part of the effect rather than the crisp pixels
   * nearest-neighbour gave the plain blit.
   */
  memset(&sampler_info, 0, sizeof(sampler_info));
  sampler_info.min_filter     = SDL_GPU_FILTER_LINEAR;
  sampler_info.mag_filter     = SDL_GPU_FILTER_LINEAR;
  sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
  sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

  shader->sampler = SDL_CreateGPUSampler(shader->gpu, &sampler_info);
  if (shader->sampler == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateGPUSampler: %s\n", SDL_GetError());
    return 0;
  }

#if defined(CHQ_CRT_SHADER_SPIRV)
  memset(&shader_info, 0, sizeof(shader_info));
  shader_info.code       = (const Uint8 *) chq_crt_vertex_spirv;
  shader_info.code_size  = chq_crt_vertex_spirv_len;
  shader_info.entrypoint = CHQ_GPU_ENTRYPOINT_VS;
  shader_info.format     = CHQ_GPU_SHADER_FORMAT;
  shader_info.stage      = SDL_GPU_SHADERSTAGE_VERTEX;
#else
  memset(&shader_info, 0, sizeof(shader_info));
  shader_info.code       = (const Uint8 *) chq_crt_vertex_msl;
  shader_info.code_size  = strlen(chq_crt_vertex_msl);
  shader_info.entrypoint = CHQ_GPU_ENTRYPOINT_VS;
  shader_info.format     = CHQ_GPU_SHADER_FORMAT;
  shader_info.stage      = SDL_GPU_SHADERSTAGE_VERTEX;
#endif

  vertex_shader = SDL_CreateGPUShader(shader->gpu, &shader_info);
  if (vertex_shader == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateGPUShader (vertex): %s\n", SDL_GetError());
    return 0;
  }

#if defined(CHQ_CRT_SHADER_SPIRV)
  memset(&shader_info, 0, sizeof(shader_info));
  shader_info.code                = (const Uint8 *) chq_crt_fragment_spirv;
  shader_info.code_size           = chq_crt_fragment_spirv_len;
  shader_info.entrypoint          = CHQ_GPU_ENTRYPOINT_FS;
  shader_info.format              = CHQ_GPU_SHADER_FORMAT;
  shader_info.stage               = SDL_GPU_SHADERSTAGE_FRAGMENT;
  shader_info.num_samplers        = 1;
  shader_info.num_uniform_buffers = 1;
#else
  memset(&shader_info, 0, sizeof(shader_info));
  shader_info.code                = (const Uint8 *) chq_crt_fragment_msl;
  shader_info.code_size           = strlen(chq_crt_fragment_msl);
  shader_info.entrypoint          = CHQ_GPU_ENTRYPOINT_FS;
  shader_info.format              = CHQ_GPU_SHADER_FORMAT;
  shader_info.stage               = SDL_GPU_SHADERSTAGE_FRAGMENT;
  shader_info.num_samplers        = 1;
  shader_info.num_uniform_buffers = 1;
#endif

  fragment_shader = SDL_CreateGPUShader(shader->gpu, &shader_info);
  if (fragment_shader == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateGPUShader (fragment): %s\n", SDL_GetError());
    return 0;
  }

  memset(&color_target_desc, 0, sizeof(color_target_desc));
  color_target_desc.format = SDL_GetGPUSwapchainTextureFormat(shader->gpu, window);

  memset(&pipeline_info, 0, sizeof(pipeline_info));
  pipeline_info.vertex_shader   = vertex_shader;
  pipeline_info.fragment_shader = fragment_shader;
  pipeline_info.primitive_type  = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
  pipeline_info.target_info.color_target_descriptions = &color_target_desc;
  pipeline_info.target_info.num_color_targets         = 1;

  shader->pipeline = SDL_CreateGPUGraphicsPipeline(shader->gpu, &pipeline_info);

  /* Conv: pipelines don't retain the shader modules internally past this
   * call, so these can be released immediately rather than kept alive for
   * the lifetime of the app.
   */
  SDL_ReleaseGPUShader(shader->gpu, vertex_shader);
  SDL_ReleaseGPUShader(shader->gpu, fragment_shader);

  if (shader->pipeline == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateGPUGraphicsPipeline: %s\n", SDL_GetError());
    return 0;
  }

  return 1;
}

void chq_CRT_shader_render(chq_CRT_shader_t       *shader,
                           SDL_Window             *window,
                           zxspectrum_t           *zx,
                           int                     x,
                           int                     y,
                           int                     w,
                           int                     h,
                           int                     game_width,
                           int                     game_height,
                           const chq_CRT_params_t *params,
                           const unsigned char    *osd_mask,
                           const uint32_t         *override_pixels)
{
  SDL_GPUViewport              viewport;
  const zx_frame_t            *frame;
  void                        *mapped;
  SDL_GPUCommandBuffer        *upload_cmdbuf;
  SDL_GPUCopyPass             *copy_pass;
  SDL_GPUTextureTransferInfo   src;
  SDL_GPUTextureRegion         dst;
  SDL_GPUTexture              *swapchain_texture;
  SDL_GPUColorTargetInfo       color_target;
  SDL_GPURenderPass           *render_pass;
  SDL_GPUTextureSamplerBinding tex_binding;
  chq_CRT_params_t             frame_params;

  /* Conv: the caller owns the tunable knobs but not the clock, so the time
   * the flicker and tear run off is filled in here, on a copy.
   */
  frame_params      = *params;
  frame_params.time = SDL_GetTicks() * 0.001f;

  viewport.x         = (float) x;
  viewport.y         = (float) y;
  viewport.w         = (float) w;
  viewport.h         = (float) h;
  viewport.min_depth = 0.0f;
  viewport.max_depth = 1.0f;

  /* Upload the game's converted screen buffer to the GPU texture, or the
   * caller's override buffer (the backbuffer debug view) if given. */
  mapped = SDL_MapGPUTransferBuffer(shader->gpu, shader->transfer_buffer, true);
  if (override_pixels != NULL)
  {
    memcpy(mapped, override_pixels, game_width * game_height * 4);
  }
  else
  {
    frame = zxspectrum_claim_screen(zx);
    memcpy(mapped, frame->pixels, game_width * game_height * 4);
    zxspectrum_release_screen(zx);
  }

  /* Composite the OSD mask directly into the staging buffer -- this path
   * has no SDL_Renderer for the caller to draw an overlay rect with, so the
   * pixels are burned in here instead. Mask value 1 = bright green text,
   * matching the plain-renderer OSD; 2 = black outline stamped behind the
   * text so it reads over any background. Both are packed to match
   * ABGR8888's in-memory byte order (R,G,B,A -- see palette_abgr's comment
   * in Screen.c); alpha is irrelevant, this texture is sampled opaquely.
   */
  if (osd_mask != NULL)
  {
    uint32_t *pixels = mapped;
    int       i;

    for (i = 0; i < game_width * game_height; i++)
      if (osd_mask[i] == 1)
        pixels[i] = 0xFF40FF40u; /* green interior */
      else if (osd_mask[i] == 2)
        pixels[i] = 0xFF000000u; /* black outline */
  }

  SDL_UnmapGPUTransferBuffer(shader->gpu, shader->transfer_buffer);

  memset(&src, 0, sizeof(src));
  src.transfer_buffer = shader->transfer_buffer;
  src.pixels_per_row  = game_width;
  src.rows_per_layer  = game_height;

  memset(&dst, 0, sizeof(dst));
  dst.texture = shader->texture;
  dst.w       = game_width;
  dst.h       = game_height;
  dst.d       = 1;

  upload_cmdbuf = SDL_AcquireGPUCommandBuffer(shader->gpu);
  copy_pass     = SDL_BeginGPUCopyPass(upload_cmdbuf);
  SDL_UploadToGPUTexture(copy_pass, &src, &dst, true);
  SDL_EndGPUCopyPass(copy_pass);

  /* Render the uploaded texture through the CRT shader pipeline. */
  if (SDL_WaitAndAcquireGPUSwapchainTexture(upload_cmdbuf, window,
                                            &swapchain_texture, NULL, NULL) &&
      swapchain_texture != NULL)
  {
    memset(&color_target, 0, sizeof(color_target));
    color_target.texture       = swapchain_texture;
    color_target.load_op       = SDL_GPU_LOADOP_CLEAR;
    color_target.store_op      = SDL_GPU_STOREOP_STORE;
    color_target.clear_color.r = 0.0f;
    color_target.clear_color.g = 0.0f;
    color_target.clear_color.b = 0.0f;
    color_target.clear_color.a = 1.0f;

    render_pass = SDL_BeginGPURenderPass(upload_cmdbuf, &color_target, 1, NULL);

    SDL_BindGPUGraphicsPipeline(render_pass, shader->pipeline);
    SDL_SetGPUViewport(render_pass, &viewport);

    tex_binding.texture = shader->texture;
    tex_binding.sampler = shader->sampler;
    SDL_BindGPUFragmentSamplers(render_pass, 0, &tex_binding, 1);
    SDL_PushGPUFragmentUniformData(upload_cmdbuf, 0, &frame_params,
                                   sizeof(frame_params));

    SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);

    SDL_EndGPURenderPass(render_pass);
  }
  else
  {
    fprintf(stderr, "Error: SDL_WaitAndAcquireGPUSwapchainTexture: %s\n",
           SDL_GetError());
  }

  SDL_SubmitGPUCommandBuffer(upload_cmdbuf);
}

void chq_CRT_shader_destroy(chq_CRT_shader_t *shader, SDL_Window *window)
{
  SDL_ReleaseGPUGraphicsPipeline(shader->gpu, shader->pipeline);
  SDL_ReleaseGPUSampler(shader->gpu, shader->sampler);
  SDL_ReleaseGPUTransferBuffer(shader->gpu, shader->transfer_buffer);
  SDL_ReleaseGPUTexture(shader->gpu, shader->texture);
  SDL_ReleaseWindowFromGPUDevice(shader->gpu, window);
  SDL_DestroyGPUDevice(shader->gpu);
}

#endif /* CHQ_CRT_SHADER_GLES */
