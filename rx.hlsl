cbuffer _ : register(b0)
{ float4x4 _xyzworldtoclip;
	float2     screen_xysize;
	float2    mouse_xycursor;
};
struct VS_INPUT
{ float2   xyz:POSITION;
  float2    uv:TEXCOORD0;
  float4 color:COLOR0;
};
struct PS_INPUT
{ float4      xyz:SV_POSITION;
  float4    color:COLOR0;
  float2       uv:TEXCOORD0;
};
PS_INPUT MainVS(VS_INPUT i)
{
	PS_INPUT o;
  o.     xyz=mul(_xyzworldtoclip,float4(i.xyz, 0.f, 1.f));
  o.   color=i.color;
  o.      uv=i.uv;
  return o;
}
SamplerState      sampler0;
Texture2D<float4> texture0;
float4 MainPS(PS_INPUT i) : SV_Target
{
  float4 color=i.color*texture0.Sample(sampler0,i.uv);
  return color;
}

