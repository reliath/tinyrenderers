// Pixel Shader
float4 main(float4 sysPos   : SV_POSITION, 
            float4 position : POSITION, 
            float2 texCoord : TEXCOORD, 
            float3 normal   : NORMAL, 
            float3 tangent  : TANGENT): SV_TARGET
{
  float4 ret = float4(0.25 * texCoord, 0.0, 1.0) +
               float4(0.50 * normal, 0.0) +
               float4(0.50 * tangent, 0.0);
  return ret;
}

