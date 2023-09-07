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

typedef struct
{
  unsigned       enable: 1;
  unsigned int   write_mask;
  struct
  { D3D11_BLEND    src;
    D3D11_BLEND    dst;
    D3D11_BLEND_OP op; } color;
  struct
  { D3D11_BLEND    src;
    D3D11_BLEND    dst;
    D3D11_BLEND_OP op; } alpha;
} EMU_color_blend_config_t;

typedef struct
{
  EMU_color_blend_config_t blend_config;

  int format;

  struct
  {
    /* this is a little bit more clear */
    unsigned enabled: 1;

    int samples;
    int quality;

  } multisampling;
} EMU_color_config_t;

// typedef struct
// {
//  EMU_color_config_t color_array[2];
//  int                color_count;
//
//  struct
//  { ID3D11BlendState *blend_state;
//    ID3D11DepthStencilState *stencil_state;
//     ID3D11RasterizerState *rasterizer_state;
//  } d3d11;
// } EMU_program_config_t;

enum
{ REG_PS_TEX_0,
  REG_PS_TEX_1,
  REG_PS_SAM_0,
  REG_PS_SAM_1,
  REG_PS_BLC_0,
  REG_PS_BLC_1,

  REG_VS_TEX_0,
  REG_VS_TEX_1,
  REG_VS_SAM_0,
  REG_VS_SAM_1,
  REG_VS_BLC_0,
  REG_VS_BLC_1,

  REG_CS_TEX_0,
  REG_CS_TEX_1,
  REG_CS_SAM_0,
  REG_CS_SAM_1,
  REG_CS_BLC_0,
  REG_CS_BLC_1,
};

/* regenv may only be pointer sized */
typedef union
{
  void *val;
  union
  {
    ID3D11ShaderResourceView   *resource_view;
    ID3D11Buffer               *buffer;
    ID3D11SamplerState         *sampler;
  } d3d11;
} regenv_t;

typedef struct
{
  struct
  {
    ID3D11InputLayout     *in;

    ID3D11DepthStencilState *ds;

    ID3D11VertexShader    *vs;/* these have to be more intricate */
    ID3D11PixelShader     *ps;/* these have to be more intricate */

    ID3D11ComputeShader   *cs;

    /* (recommended) do not modify these directly */
    ID3D11RasterizerState *rastr_state;
    ID3D11BlendState      *blend_state;

  } d3d11;

  Emu_shader_t ps_;

  unsigned dual_source_blending: 1;

  struct
  { Emu_texture_t *color[2];
    int            count;
    Emu_texture_t *depth; } out;

  regenv_t              reg[0x20];
} pipenv_t;

/* todo: these are temporary */
void
rxpipset_program(
  Emu_shader_t vs,
  Emu_shader_t ps, int flush);

void
regset(
  int reg, void *res, int flush);
void
rxpipset_sampler(
  int reg, rxsampler_t sampler, int flush);
void
rxpipset_texture(
  int reg, Emu_texture_t *texture, int flush);
void
rxpipset_varying(
  int reg, rxuniform_buffer_t buffer, int flush);
