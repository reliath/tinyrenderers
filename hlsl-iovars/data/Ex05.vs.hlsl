// Gratuitous nested structs

// Vertex Shader
struct VSInput {
  float4 position : POSITION;
  float2 texCoord : TEXCOORD;
  float3 normal   : NORMAL;
  float3 tangent  : TANGENT;
};

struct NestedNestedNested_1 {
  float4 position : POSITION;
};

struct NestedNested_1 {
  NestedNestedNested_1 nested_1;
  float2               texCoord : TEXCOORD;
};

struct NestedNested_2 {
  float3 normal   : NORMAL;
  float3 tangent  : TANGENT;
};

struct Nested {
  NestedNested_1 nested_1;
  NestedNested_2 nested_2;
};

struct VSOutput {
  float4 sysPos   : SV_POSITION;
  Nested nested;
  // PS will not contain unused vars
  float2 unused0  : UNUSED0;
  uint   unused1  : UNUSED1;
};

VSOutput main(VSInput input) 
{
  VSOutput ret;
  ret.sysPos                            = input.position;
  ret.nested.nested_1.nested_1.position = input.position;
  ret.nested.nested_1.texCoord          = input.texCoord;  
  ret.nested.nested_2.normal            = input.normal;
  ret.nested.nested_2.tangent           = input.tangent;
  return ret;
}