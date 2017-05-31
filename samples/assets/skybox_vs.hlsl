#pragma shader_stage(vertex)

cbuffer SceneUniforms : register(b0) {
  float4 time_and_res;
} scene_const;

struct VS_OUTPUT {
  float4 pos : SV_POSITION;
};

VS_OUTPUT main(uint vertex_id : SV_VertexID) {
  VS_OUTPUT output;
  output.pos = float4(0,0,0,1);
  return output;
}
