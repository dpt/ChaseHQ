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

#if defined(CHQ_CRT_SHADER_SPIRV)

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
    // scanlines, intensity adapted to local luminance.
    "  float scan = sin(uv.y * 384.0 * 3.14159265) * 0.5 + 0.5;\n"
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
  pipeline_info.vertex_shader                         = vertex_shader;
  pipeline_info.fragment_shader                       = fragment_shader;
  pipeline_info.primitive_type                        = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
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
        pixels[i] = 0x0040FF40u; /* green interior */
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
