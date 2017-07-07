// SV_Position relocated to middle of struct members

// Vertex Shader
struct VSInput {
  float4 position : POSITION;
  float2 texCoord : TEXCOORD;
  float3 normal   : NORMAL;
  float3 tangent  : TANGENT;
};

// SV_Position not at beginning
struct VSOutput {
  float4 position : POSITION;
  float2 texCoord : TEXCOORD;
  float3 normal   : NORMAL;
  float4 sysPos   : SV_POSITION;
  float3 tangent  : TANGENT;
  // PS will not contain unused vars
  float2 unused0  : UNUSED0;
  uint   unused1  : UNUSED1;
};

VSOutput main(VSInput input) 
{
  VSOutput ret;
  ret.sysPos   = input.position;
  ret.position = input.position;
  ret.texCoord = input.texCoord;  
  ret.normal   = input.normal;
  ret.tangent  = input.tangent;
  return ret;
}