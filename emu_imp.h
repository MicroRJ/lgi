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

#ifndef EMU_IMP_INDEX_BUFFER_SIZE
#define EMU_IMP_INDEX_BUFFER_SIZE  0x1000
# endif//EMU_IMP_INDEX_BUFFER_SIZE

#ifndef EMU_IMP_VERTEX_BUFFER_SIZE
#define EMU_IMP_VERTEX_BUFFER_SIZE 0x1000
# endif//EMU_IMP_VERTEX_BUFFER_SIZE

#ifndef EMU_IMP_ENABLE_DEPTH_TESTING
#define EMU_IMP_ENABLE_DEPTH_TESTING
# endif//EMU_IMP_ENABLE_DEPTH_TESTING

enum
{ EMU_IMP_MODE_2D            = 0,
  EMU_IMP_MODE_3D,
  EMU_IMP_MODE_QUAD,
  EMU_IMP_MODE_SDF_RECT,
  EMU_IMP_MODE_SDF_CIRCLE,
  EMU_IMP_MODE_SDF_TEXT,
  EMU_IMP_MODE_LCD_TEXT };

/* going for simplicity here, this should be enough for most simple
 things */
typedef int Emu_imp_index_t;

typedef union
{ struct
  { rxvec4_t  xyzw;
    rxvec4_t  rgba;
    rxvec2_t  uv; };
  struct
  { float x,y,z,w;
    float r,g,b,a;
    float u,v; };

  /* for specialized shaders */
  struct
  { rxvec2_t xy;
    rxvec4_t xyxy;
    rxvec4_t rgba;
    rxvec4_t flag;
  } rect;
} Emu_imp_vertex_t;

/* section: immediate mode */
ccfunc void Emu_imp_flush();
ccfunc void Emu_imp_apply();

ccfunc void Emu_window_clear();

typedef struct
{
  int mode;

  rxmatrix_t   view_matrix;
  rxmatrix_t  world_matrix;

  struct
  { rxmatrix_t matrix;

    rxvec2_t   xyscreen;
    rxvec2_t   xysource;/* this should be an array */
    rxvec2_t   xycursor;

    double     total_seconds;
    double     delta_seconds;
  } var;

  rxuniform_buffer_t  varying;



  struct
  {
    ID3D11BlendState *default_blend_state;
    ID3D11BlendState *subpixel_dual_blending_blend_state;
  } d3d11;

  struct
  {
    rxShader sha_vtx_sdf;
    rxShader sha_pxl_sdf_cir;
    rxShader sha_pxl_sdf_box;

    rxShader sha_pxl_txt;
    rxShader sha_pxl_txt_sdf;

    rxShader sha_vtx;
    rxShader sha_pxl; };

  rxvertex_buffer_t   asm_vtx;
  rxindex_buffer_t    asm_idx;

  /* cleanup #pending */
  rxborrowed_t       vertex_buffer_writeonly;
  Emu_imp_vertex_t  *vertex_array;
           int       vertex_tally;

  /* cleanup #pending */
  rxborrowed_t       index_buffer_writeonly;
  Emu_imp_index_t   *index_array;
           int       index_tally;

           int       index_offset;
  Emu_imp_vertex_t   attr;

  Emu_texture_t  *white_texture;

} Emu_imp_t;


