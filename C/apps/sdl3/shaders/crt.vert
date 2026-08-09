#version 450

/* Fullscreen triangle generated from gl_VertexIndex, mirroring the MSL
 * vertex shader in CRTShader.c (chq_crt_vertex_msl). No vertex buffer
 * needed.
 */

layout(location = 0) out vec2 uv;

void main()
{
  vec2 pos[3] = vec2[](vec2(-1.0, -1.0), vec2(3.0, -1.0), vec2(-1.0, 3.0));
  vec2 texc[3] = vec2[](vec2(0.0, 1.0), vec2(2.0, 1.0), vec2(0.0, -1.0));

  gl_Position = vec4(pos[gl_VertexIndex], 0.0, 1.0);
  uv = texc[gl_VertexIndex];
}
