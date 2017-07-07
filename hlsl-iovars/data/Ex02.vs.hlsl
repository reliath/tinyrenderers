// Members separated to parameters in PS entry point

// Vertex Shader
struct VSInput {
  float4 position : POSITION;
  float2 texCoord : TEXCOORD;
  float3 normal   : NORMAL;
  float3 tangent  : TANGENT;
};

struct VSOutput {
  float4 sysPos   : SV_POSITION;
  float4 position : POSITION;
  float2 texCoord : TEXCOORD;
  float3 normal   : NORMAL;
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