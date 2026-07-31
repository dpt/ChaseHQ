/**
 * CRTShader.h
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

#ifndef CHQ_CRT_SHADER_H
#define CHQ_CRT_SHADER_H

#include <SDL3/SDL.h>

#include "ZXSpectrum/Spectrum.h"

// CRT post-effect prototype: renders the game's converted screen buffer
// through a Metal fragment shader (scanlines/bloom/vignette) via SDL's GPU
// API, instead of the plain SDL_Renderer blit. See CRTShader.c for the
// SDL3/Metal-only caveat.
typedef struct
{
  SDL_GPUDevice         *gpu;
  SDL_GPUTexture        *texture;         // holds the game's converted screen
  SDL_GPUTransferBuffer *transfer_buffer; // staging buffer for the upload above
  SDL_GPUSampler        *sampler;
  SDL_GPUGraphicsPipeline *pipeline;
} chq_CRT_shader_t;

// Tunable shader knobs, pushed to the fragment shader as a uniform each
// frame. Layout must match the MSL Params struct in CRTShader.c exactly
// (plain floats, same order, no padding).
typedef struct
{
  float curvature;          // barrel distortion strength
  float bloom_threshold;    // luminance level above which bloom kicks in
  float bloom_intensity;    // bloom contribution scale
  float brightness;         // post-contrast multiplicative brightness
  float contrast;           // contrast around mid-grey
  float saturation;         // 1.0 = full colour, 0.0 = greyscale
  float scanline_intensity; // base scanline darkening amount
  float vignette_strength;  // corner darkening strength
  float chroma_bleed;       // PAL horizontal colour smear, 0.0 = off
  float glitch;             // mains flicker + scanline tear, 0.0 = off
  float time;               // seconds since start; filled in by the renderer
} chq_CRT_params_t;

// Defaults re-tuned by eye against this game's screen; see CRTShader.c.
#define CHQ_CRT_PARAMS_DEFAULT \
  { 0.025f, 0.5f, 0.025f, 1.1f, 1.1f, 1.0f, 0.75f, 0.3f, 0.6f, 0.0f, 0.0f }

/**
 * Creates the GPU device, texture, sampler and shader pipeline. Must be
 * called once, after the SDL window is created.
 *
 * \param[out] shader      Shader state to initialise.
 * \param[in]  window      SDL window to claim for the GPU device.
 * \param[in]  game_width  Width in pixels of the game's screen buffer.
 * \param[in]  game_height Height in pixels of the game's screen buffer.
 * \return 1 on success, 0 on failure (an error is printed to stderr).
 */
int chq_CRT_shader_create(chq_CRT_shader_t *shader,
                          SDL_Window       *window,
                          int               game_width,
                          int               game_height);

/**
 * Uploads the game's current screen buffer and renders one frame through
 * the CRT shader pipeline.
 *
 * \param[in,out] shader      Shader state, as created by chq_CRT_shader_create.
 * \param[in]     window      SDL window to present into.
 * \param[in]     zx          Facade instance to claim the screen buffer from.
 * \param[in]     x           Destination viewport X offset, in pixels.
 * \param[in]     y           Destination viewport Y offset, in pixels.
 * \param[in]     w           Destination viewport width, in pixels.
 * \param[in]     h           Destination viewport height, in pixels.
 * \param[in]     game_width  Width in pixels of the game's screen buffer.
 * \param[in]     game_height Height in pixels of the game's screen buffer.
 * \param[in]     params      Shader tuning parameters for this frame.
 */
void chq_CRT_shader_render(chq_CRT_shader_t       *shader,
                           SDL_Window             *window,
                           zxspectrum_t           *zx,
                           int                     x,
                           int                     y,
                           int                     w,
                           int                     h,
                           int                     game_width,
                           int                     game_height,
                           const chq_CRT_params_t *params);

/**
 * Releases everything created by chq_CRT_shader_create.
 *
 * \param[in,out] shader Shader state to release.
 * \param[in]     window SDL window to release the GPU device from.
 */
void chq_CRT_shader_destroy(chq_CRT_shader_t *shader, SDL_Window *window);

#endif /* CHQ_CRT_SHADER_H */
