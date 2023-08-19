/*
**
**                      -+- rx -+-
**
**                 Mundane Graphics API.
**
**  *--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
**  Copyright(C) Dayan Rodriguez, 2022, All Rights Reserved
**  *--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*
**
**        NOT PRODUCTION READY/NOT PRODUCTION QUALITY
**
**
**               HONI SOIT QUI MAL Y PENSE
**
**
**                 github.com/MicroRJ/rx
**
*/

/* shared structures */
cbuffer _ : register(b0)
{ float4x4  transform;
  float2    xyscreen;
  float2    xysample;
  float2    xycursor;

  double total_seconds;
  double delta_seconds;
};

SamplerState      sampler0: register(s0);
Texture2D<float4> texture0: register(t0);


/* this is the basic vertex shader */
struct VS_INPUT
{ float4  xyzw:POSITION;
  float4 color:COLOR0;
  float2    uv:TEXCOORD0;
};
struct PS_INPUT
{ float4  xyzw:SV_POSITION;
  float4 color:COLOR0;
  float2    uv:TEXCOORD0;
};
PS_INPUT MainVS(VS_INPUT i)
{
  PS_INPUT o;
  o.    xyzw=mul(transform,i.xyzw);
  o.   color=i.color;
  o.      uv=i.uv;
  return o;
}

float4 MainPS(PS_INPUT i) : SV_Target
{
  return i.color * texture0.Sample(sampler0, i.uv);
}


float4 MainPS_Text(PS_INPUT i) : SV_Target
{
  float3 sample =
    float3(
      texture0.Sample(sampler0, i.uv + float2(-1., 0) / xysample).r,
      texture0.Sample(sampler0, i.uv).r,
      texture0.Sample(sampler0, i.uv + float2(+1., 0) / xysample).r );

  return i.color * sample.rgbg;
}

/* these are the extended versions for SDF shape rendering */
struct VS_INPUT_SDF
{ float2 xy                 : POSITION0;
  float4 shape_xyxy         : POSITION1;
  float4 shape_rgba         :    COLOR0;
  /* flag.x: corner radius for rounded rects
     flag.w: shape softness */
  float4 shape_flag         : TEXCOORD0;
};
struct PS_INPUT_SDF
{ float4 xyzw        : SV_POSITION;
  float4 shape_xyxy  :   POSITION1;
  float4 shape_rgba  :      COLOR0;
  float4 shape_flag  :   TEXCOORD0;
};

PS_INPUT_SDF MainVS_SDF(VS_INPUT_SDF i)
{
  PS_INPUT_SDF o;
  o.xyzw     =mul(transform,float4(i.xy,.0,1.));
  o.shape_rgba=i.shape_rgba;
  o.shape_xyxy=i.shape_xyxy;
  o.shape_flag=i.shape_flag;
  return o;
}

float box_sdf(float2 p, float2 b, float r)
{
  float2 d = abs(p) - (b - r);
  return length(max(d,0.0)) + min(max(d.x,d.y),0.0) - r;
}

float circle_sdf(float2 p, float r)
{
  return length(p) - r;
}

float4 MainPS_CircleSDF(PS_INPUT_SDF i) : SV_Target
{
  float2 xy = float2(i.xyzw.x,xyscreen.y - i.xyzw.y);

  return float4(i.shape_rgba.rgb,
    i.shape_rgba.a * smoothstep(0.,i.shape_flag.w,(1-circle_sdf(xy-i.shape_xyxy.xy,i.shape_xyxy.z))));
}

float4 MainPS_BoxSDF(PS_INPUT_SDF i) : SV_Target
{
  float2 xy = float2(i.xyzw.x,xyscreen.y - i.xyzw.y);

  return float4(i.shape_rgba.rgb,
    i.shape_rgba.a * smoothstep(0.,i.shape_flag.w,(1-box_sdf(xy-i.shape_xyxy.xy,i.shape_xyxy.zw,i.shape_flag.x))));
}
