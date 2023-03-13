cbuffer ConstBuffer : register(b0)
{ float4x4 ProjectionMatrix;
};
struct VS_INPUT
{ float2 pos : POSITION;
  float4 col : COLOR0;
  float2 uv  : TEXCOORD0;
};
struct PS_INPUT
{ float4 pos : SV_POSITION;
  float4 col : COLOR0;
  float2 uv  : TEXCOORD0;
};
PS_INPUT MainVS(VS_INPUT input)
{ PS_INPUT output;
  output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));
  output.col = input.col;
  output.uv  = input.uv;
  return output;
}
SamplerState      sampler0;
Texture2D<float4> texture0;
float4 MainPS(PS_INPUT input) : SV_Target
{ float4 out_col = texture0.Sample(sampler0,input.uv);
  return out_col;
}