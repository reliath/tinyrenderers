// Pixel Shader
struct NestedNested_1 {
  float4 position : POSITION;
  float2 texCoord : TEXCOORD;
};

struct NestedNestedNested_1 {
  float3 tangent : TANGENT;
};

struct NestedNested_2 {
  float3               normal   : NORMAL;
  NestedNestedNested_1 nested_1;
};

struct Nested {
  NestedNested_1 nested_1;
  NestedNested_2 nested_2;
};

struct PSInput {
  float4 sysPos   : SV_POSITION;
  Nested nested;
};

float4 main(PSInput input) : SV_TARGET
{
  float4 ret = float4(0.25 * input.nested.nested_1.texCoord, 0.0, 1.0) +
               float4(0.50 * input.nested.nested_2.normal, 0.0) +
               float4(0.50 * input.nested.nested_2.nested_1.tangent, 0.0);
  return ret;
}

