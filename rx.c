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

/*
** Libraries that you should use instead:
** sokol or tigr or raylib or virtually any other library that isn't this one.
**
** ** panic notes **
** - I DON'T SEE ANYTHING:
**   - Ensure that the pixel shader is not the problem first, make it return a solid color, like white,
**     if it does show, ensure the alpha channel is set.
**   - Ensure the render stack is configured properly, see note below to ensure how.
**   - Ensure your constant, vertex and index buffers contain valid data, you can check all this quite
**     trivially in the amazing 'Render Doc' for free!
**     - Select the 'Event Browser' window, (You can toggle windows under the 'Window' menu)
**         - First look at your draw calls, ensure you have a 'ClearRenderTarget()', 'Present()' and
**           at least a draw instruction, such as 'DrawIndexed(6)', if you don't it means you forgot
**           missed some code.
**         - For further inspection, select a draw event, such as 'DrawIndexed(..)'
**           - First, go to the 'Pipeline Stage' window, click the vertex and then the pixel shader stages to ensure
**             that all required resources are properly bound in their corresponding slots.
**              - You can click on the texture or resource to view its contents.
**           - Then, to verify that the per vertex data is proper, click on the 'Mesh Viewer' window,
**             you can see the projected inputs and outputs of the vertex shader, use this window to ensure
**             everything is properly set.
**           - If applicable, ensure the 'w' component of your vectors is properly set, if not sure, edit
**              the vertex shader and manually set to .5.
**   - If you're sure the problem is not the data nor the shaders, then it will most likely be a pipeline configuration
**     mistake, check the blend state first, then the rasterizer state.
**      - Ensure there's at least one blend state properly configured, if you're not sure disable blending
**        altogether to use d3d's default blend state.
**      - If clipping enabled, ensure you've got a clip set big enough for your scene, if not sure, disable
**        clipping altogether by not setting the rasterizer.
**      - Ensure your view-port is properly set!
**   - If you still don't see anything, try changing the clear color to something different, if you don't
**     see that color, then it could be a driver error, in which case, run Visual Studio's Graphic Debugger.
*/
// todo: bind different textures to the shader to avoid having to switch them multiple times per frame?
#ifndef _RX_H
#define _RX_H

#define _CRT_SECURE_NO_WARNINGS
#define              CINTERFACE
#define              COBJMACROS
#define        D3D11_NO_HELPERS
#define                NOMINMAX
#define     WIN32_LEAN_AND_MEAN
#define    _NO_CRT_STDIO_INLINE
#pragma comment(lib,        "Gdi32")
#pragma comment(lib,       "dxguid")
#pragma comment(lib,        "d3d11")
#pragma comment(lib,  "d3dcompiler")
#pragma warning(push)
#pragma warning(disable:4115)
#pragma warning(disable:4201)
#include <d3dcompiler.h>
#include     <windows.h>
#include    <Windowsx.h>
#include   <dxgidebug.h>
#include        <dxgi.h>
#include       <d3d11.h>
#include     <dxgi1_3.h>
#pragma warning(pop)

# ifdef _RX_NO_CONVERSION_WARNING
#pragma warning(disable:4244)
# endif
# ifdef _RX_NO_TRUNCATION_WARNING
#pragma warning(disable:4305)
# endif

#   ifdef _RX_STANDALONE
# include "cc\cc.c"
#  define STB_IMAGE_IMPLEMENTATION
#  define STBI_MALLOC(size)          ccmalloc(size)
#  define STBI_REALLOC(size,memory)  ccrealloc(size,memory)
#  define STBI_FREE(memory)          ccfree(memory)
# include "stb_image.h"

#  define STB_IMAGE_WRITE_IMPLEMENTATION
#  define STBIW_MALLOC(size)         ccmalloc(size)
#  define STBIW_REALLOC(size,memory) ccrealloc(size,memory)
#  define STBIW_FREE(memory)         ccfree(memory)
# include "stb_image_write.h"

# define  STB_TRUETYPE_IMPLEMENTATION
# define  STBTT_malloc(size,u) ccmalloc(size)
# define  STBTT_free(memory,u) ccfree(memory)
# include "stb_truetype.h"
#endif//_RX_STANDALONE

/* todo: we can do this better */
#include  "rxps.hlsl"
#include  "rxvs.hlsl"
#include  "rxsdf.vs.hlsl"
#include  "rxtxt.ps.hlsl"
#include  "rxsdf_cir.ps.hlsl"
#include  "rxsdf_box.ps.hlsl"


/* delicacies of programming */
#ifndef RX_TLIT
#ifndef __cplusplus
#define RX_TLIT(T) (T)
#  else
#define RX_TLIT(T)
# endif//__cplusplus
# endif//RX_TLIT

#pragma warning(push)

/* Suppress some warnings, normally I like to be explicit with casting but I have developed an
 aesthetic for the plain '.' syntax and honestly, for most code, I could care less. */
#pragma warning(disable:4244)
#pragma warning(disable:4305)

/* The following are rather crude setup macros, not the long-term approach that one would like
 to use but for what this API does it seems to be the next simplest step you'd take towards
 slightly more flexibility.
   Normally, in my use cases when I write a graphics app I just want to see something on
 screen first and then figure out where to go from there, this usually means the last thing I want
 to bother with is the graphics API or windows stuff, so in the end, I usually end up either
 modifying this file directly or extracting the bits I want from it or a combination of both, so for
 now this seems sufficient, not sure why we wrote this comment. */
#ifndef _RX_DEFAULT_WINDOW_SIZE_X
#define _RX_DEFAULT_WINDOW_SIZE_X CW_USEDEFAULT
# endif//_RX_DEFAULT_WINDOW_SIZE_X
#ifndef _RX_DEFAULT_WINDOW_SIZE_Y
#define _RX_DEFAULT_WINDOW_SIZE_Y CW_USEDEFAULT
# endif//_RX_DEFAULT_WINDOW_SIZE_Y
#ifndef     _RX_MSAA
#define     _RX_MSAA 4
# endif//_RX_MSAA
#ifndef     _RX_REFRESH_RATE
#define     _RX_REFRESH_RATE 60
# endif//_RX_REFRESH_RATE
#ifndef RX_INDEX_BUFFER_SIZE
#define RX_INDEX_BUFFER_SIZE 0x4000
# endif//RX_INDEX_BUFFER_SIZE
#ifndef RX_VERTEX_BUFFER_SIZE
#define RX_VERTEX_BUFFER_SIZE 0x4000
# endif//RX_VERTEX_BUFFER_SIZE
/* This is only if 3d mode is enabled - XXX - the one called rj */
#ifndef _RX_ENABLE_DEPTH_STENCIL
#define _RX_ENABLE_DEPTH_STENCIL
# endif//_RX_ENABLE_DEPTH_STENCIL

/* XX Would one want to support row/col matrices? */
#ifndef RX_SHADER_COMPILATION_FLAGS
#ifdef _CCDEBUG
#define RX_SHADER_COMPILATION_FLAGS\
  D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR|\
                     D3DCOMPILE_DEBUG|\
         D3DCOMPILE_SKIP_OPTIMIZATION|\
       D3DCOMPILE_WARNINGS_ARE_ERRORS

#else
#define RX_SHADER_COMPILATION_FLAGS\
  D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR|\
         D3DCOMPILE_ENABLE_STRICTNESS|\
       D3DCOMPILE_OPTIMIZATION_LEVEL3
# endif//_CCDEBUG
# endif//RX_SHADER_COMPILATION_FLAGS

#include "rxm.cc"

typedef struct rxcolor8_t rxcolor8_t;
typedef struct rxcolor8_t
{ unsigned char r,g,b,a;
} rxcolor8_t;

typedef rxvec4_t rxcolor_t;

#ifndef RX_RGBA
#define RX_RGBA(R,G,B,A) RX_TLIT(rxcolor_t){R,G,B,A}
# endif
#ifndef RX_RGBA_UNORM
#define RX_RGBA_UNORM(R,G,B,A) RX_RGBA(R/255.,G/255.,B/255.,A/255.)
# endif

/*
**
**  NOLI SE TANGERE
**
*/

/* This could be made into an enum instead - XXX - the one called rj */
#define RX_COLOR_WHITE         RX_RGBA_UNORM(0xFF, 0xFF, 0xFF, 0xFF)
#define RX_COLOR_BLACK         RX_RGBA_UNORM(0x00, 0x00, 0x00, 0xFF)
#define RX_COLOR_RED           RX_RGBA_UNORM(0xFF, 0x00, 0x00, 0xFF)
#define RX_COLOR_GREEN         RX_RGBA_UNORM(0x00, 0xFF, 0x00, 0xFF)
#define RX_COLOR_BLUE          RX_RGBA_UNORM(0x00, 0x00, 0xFF, 0xFF)
#define RX_COLOR_YELLOW        RX_RGBA_UNORM(0xFF, 0xFF, 0x00, 0xFF)
#define RX_COLOR_CYAN          RX_RGBA_UNORM(0x00, 0xFF, 0xFF, 0xFF)
#define RX_COLOR_MAGENTA       RX_RGBA_UNORM(0xFF, 0x00, 0xFF, 0xFF)
#define RX_COLOR_ORANGE        RX_RGBA_UNORM(0xFF, 0xA5, 0x00, 0xFF)
#define RX_COLOR_PURPLE        RX_RGBA_UNORM(0x80, 0x00, 0x80, 0xFF)
#define RX_COLOR_PINK          RX_RGBA_UNORM(0xFF, 0xC0, 0xCB, 0xFF)
#define RX_COLOR_LIME          RX_RGBA_UNORM(0x00, 0xFF, 0x00, 0xFF)
#define RX_COLOR_TEAL          RX_RGBA_UNORM(0x00, 0x80, 0x80, 0xFF)
#define RX_COLOR_SKY_BLUE      RX_RGBA_UNORM(0x87, 0xCE, 0xEB, 0xFF)
#define RX_COLOR_GOLD          RX_RGBA_UNORM(0xFF, 0xD7, 0x00, 0xFF)
#define RX_COLOR_INDIGO        RX_RGBA_UNORM(0x4B, 0x00, 0x82, 0xFF)
#define RX_COLOR_SILVER        RX_RGBA_UNORM(0xC0, 0xC0, 0xC0, 0xFF)
#define RX_COLOR_TURQUOISE     RX_RGBA_UNORM(0x40, 0xE0, 0xD0, 0xFF)
#define RX_COLOR_CORAL         RX_RGBA_UNORM(0xFF, 0x7F, 0x50, 0xFF)
#define RX_COLOR_ORCHID        RX_RGBA_UNORM(0xDA, 0x70, 0xD6, 0xFF)
#define RX_COLOR_LAVENDER      RX_RGBA_UNORM(0xE6, 0xE6, 0xFA, 0xFF)
#define RX_COLOR_MAROON        RX_RGBA_UNORM(0x80, 0x00, 0x00, 0xFF)
#define RX_COLOR_NAVY          RX_RGBA_UNORM(0x00, 0x00, 0x80, 0xFF)
#define RX_COLOR_OLIVE         RX_RGBA_UNORM(0x80, 0x80, 0x00, 0xFF)
#define RX_COLOR_SALMON        RX_RGBA_UNORM(0xFA, 0x80, 0x72, 0xFF)
#define RX_COLOR_AQUAMARINE    RX_RGBA_UNORM(0x7F, 0xFF, 0xD4, 0xFF)

/* XX these have to be renamed */
enum {
     rxRGB8 = DXGI_FORMAT_R8_UNORM,
 rxRGBA8888 = DXGI_FORMAT_R8G8B8A8_UNORM };

enum
{ rx_kNONE   = 0,

  rx_kKEY_F1,
  rx_kKEY_F2,
  rx_kKEY_F3,
  rx_kKEY_F4,
  rx_kKEY_F5,
  rx_kKEY_F6,
  rx_kKEY_F7,
  rx_kKEY_F8,
  rx_kKEY_F9,
  rx_kKEY_F10,
  rx_kKEY_F11,
  rx_kKEY_F12,

  rx_kRETURN,

  rx_kKEY_LEFT,
  rx_kKEY_RIGHT,
  rx_kKEY_UP,
  rx_kKEY_DOWN,

  rx_kLCTRL,
  rx_kRCTRL,
  rx_kLSFHT,
  rx_kRSFHT,
  rx_kBCKSPC,
  rx_kDELETE,
  rx_kHOME,
  rx_kEND,
  rx_kESCAPE,

  // rx_kLBUTTON,
  // rx_kRBUTTON,
  // rx_kMBUTTON,
  // rx_kMVWHEEL,
  // rx_kMHWHEEL,

  rx_kKEY_SPACE = ' ',
  rx_kKEY_A     = 'A',
  rx_kKEY_Z     = 'Z',
  rx_kKEY_0     = '0',
  rx_kKEY_9     = '9',
};

// note: is this is good name?
typedef struct rxborrowed_t rxborrowed_t;
typedef struct rxborrowed_t
{
            void * memory;
  union
  {          int   stride;
             int   length;
  };

  /* this will be removed possibly if we ever switch to a handle based system */
  struct
  {
    ID3D11Resource * resource;
  } d3d11;
} rxborrowed_t;

typedef ID3D11DeviceChild *rxunknown_t;

/* these are stupid */
ccfunc ccinle void
rxunknown_delete(void *unknown);

ccfunc rxborrowed_t
rxunknown_borrow(rxunknown_t buffer);

/* section: GPU buffers */
typedef struct rxuniform_buffer_t rxuniform_buffer_t;
typedef struct rxuniform_buffer_t
{
  rxunknown_t unknown;
} rxuniform_buffer_t;

void
rxuniform_buffer_delete(
  rxuniform_buffer_t buffer );

void
rxuniform_buffer_update(
  rxuniform_buffer_t uniform, void *memory, size_t length );

typedef struct rxstruct_buffer_t rxstruct_buffer_t;
typedef struct rxstruct_buffer_t
{
  rxunknown_t unknown;
} rxstruct_buffer_t;

void
rxstruct_buffer_delete(
  rxstruct_buffer_t);

typedef struct rxindex_buffer_t rxindex_buffer_t;
typedef struct rxindex_buffer_t
{
  rxunknown_t unknown;
} rxindex_buffer_t;

void
rxindex_buffer_delete(
  rxindex_buffer_t);

typedef struct rxvertex_buffer_t rxvertex_buffer_t;
typedef struct rxvertex_buffer_t
{
  rxunknown_t unknown;
} rxvertex_buffer_t;

void
rxvertex_buffer_delete(
  rxvertex_buffer_t);


/* section: GPU sampler */
typedef struct rxsampler_t
{
  struct
  {

    ID3D11SamplerState *state;
  } d3d11;
} rxsampler_t;

ccfunc void
rxsampler_apply(
  int slot,
  rxsampler_t sampler);

/* section: textures */

typedef struct
{
  int    format;
  int    size_x;
  int    size_y;
  /* memory and stride are optional, if memory provided stride may not be 0 */
  int    stride;
  void  *memory;

  /* multisampling, default should be 1,0 */
  int    samples;
  int    quality;

  /* todo: replace this with a single flag field instead */
  struct
  { /* GPU allocation flags */
    D3D11_USAGE memtype;
            int useflag;
    /* CPU access flags */
            int memflag; };

  /* optional in/out */
  struct
  { union
    { ID3D11Resource         *resource;
      ID3D11Texture2D        *texture_2d; };

    ID3D11ShaderResourceView *view;
  } d3d11;
} rxtexture_config_t;

ccfunc ccinle rxtexture_config_t
rxtexture_config_init(
          int     size_x,
          int     size_y,
  DXGI_FORMAT     format,

          int     stride,
          void  * memory,

          int    samples,
          int    quality,

  D3D11_USAGE    memtype,
          int    useflag,
          int    memflag );

/* base texture object for all other textures */
typedef struct
{
  int format;
  int size_x;
  int size_y;
  int samples;
  int quality;

  struct
  { union
    { ID3D11Resource  *resource;
      ID3D11Texture2D *texture_2d; };
    /* the view is optional */
    ID3D11ShaderResourceView *view;
  } d3d11;
} rxtexture_t;

ccfunc int
rxtexture_init(
  rxtexture_t *texture, rxtexture_config_t *config);

ccfunc ccinle rxtexture_t
rxtexture_create_exex(
          int             size_x,
          int             size_y,
  DXGI_FORMAT             format,
          int             stride,
          void           *memory,
  D3D11_USAGE            memtype,
          int            useflag,
          int            memflag );

ccfunc ccinle rxtexture_t
rxtexture_create_ex(
          int  size_x,
          int  size_y,
  DXGI_FORMAT  format,
          int  stride,
        void  *memory);

ccfunc ccinle rxtexture_t
rxtexture_create(
  int  size_x,
  int  size_y,
  int  format);

ccfunc rxborrowed_t
rxtexture_borrow(
  rxtexture_t texture);

typedef struct
{  int   size_x, size_y;
   int   format;
   int   stride;
  void * memory;
} rxtexture_memory_t;

ccfunc rxtexture_memory_t
rxtexture_load(
  const char *name);

ccfunc rxtexture_t
rxtexture_memory_upload(
  rxtexture_memory_t texture);

ccfunc rxtexture_t
rxtexture_upload(
  const char *);

/* render target, not sure why I chose to make this a separate type but we're going for it */
typedef struct
{
  rxtexture_t texture;/* must be first field */

  struct
  { ID3D11RenderTargetView  *view;
  } d3d11;
} rxdrawing_texture_t;

typedef struct rxstencil_texture_t
{
  rxtexture_t texture;/* must be first field */

  struct
  { ID3D11DepthStencilState *state;
    ID3D11DepthStencilView  *view;
  } d3d11;
} rxstencil_texture_t;

void
rxdrawing_texture_apply(
  rxdrawing_texture_t target, float *clear_color);

/* section: GPU shader */
enum
{ RX_OBJECT_TYPE_kINVALID        = 0,
  RX_OBJECT_TYPE_kPIXEL_SHADER   = 1,
  RX_OBJECT_TYPE_kVERTEX_SHADER  = 2,
  RX_OBJECT_TYPE_kCOMPUTE_SHADER = 3,
};


typedef struct
{
  int type;

  struct
  { struct
    {
      struct
      { size_t      length;
        void       *memory; };

      /* load from file if memory not given */
      char const  *fpath;

      char const  *label;/* optional */
      char const  *model;/* optional */
      char const  *entry;

    } source;
    struct
    { size_t length;
      void * memory;
    } bytecode;
  } hlsl;

  struct
  { /* pending: who cares? */
    D3D11_INPUT_ELEMENT_DESC layout_config[0x20];
                         int layout_length;

    ID3D11InputLayout *layout;
  } d3d11;
} shader_config_t;

typedef struct
{ int type;
  struct
  { union
    { ID3D11DeviceChild    *unknown;
      ID3D11VertexShader   *vertex_shader;
      ID3D11PixelShader    *pixel_shader;
      ID3D11ComputeShader  *compute_shader; };

    ID3D11InputLayout *layout;
  } d3d11;
} rxshader_t;

/* beware that this function may not handle padding very well should
 you choose to let the API create the input layout for you automatically,
  ideally you should stick to aligned units, try to pack things into float4's
 if necessary. */
void
rxshader_init(
  rxshader_t *shader, shader_config_t *config);

rxshader_t
rxshader_create(/* this function takes the bytecode itself */
  int type, size_t length, void *memory);

rxshader_t
rxshader_load(
  int type, char const *entry, char const *fpath);


/* todo: implement this */
typedef struct
{
  rxshader_t ps;
  rxshader_t vs;
} rxprogram_t;

/* section: immediate mode */
ccfunc void rximp_flush();
ccfunc void rximp_apply();
ccfunc void rximp_clear();

/* We're going for simplicity here, this should be enough for most simple
 things */
typedef int rxvindex_imp_t;

typedef union
{ struct
  { rxvec4_t  xyzw;
    rxvec4_t  rgba;
    rxvec2_t  uv; };
  struct
  { float x,y,z,w;
    float r,g,b,a;
    float u,v; };

  /* specialized shaders */
  struct
  { rxvec2_t xy;
    rxvec4_t xyxy;
    rxvec4_t rgba;
    rxvec4_t flag;
  } rect;
} rxvertex_imp_t;

/* todo */
typedef union
{
  void *val;

  union
  { ID3D11ShaderResourceView   *texture_2d;
    ID3D11Buffer               *buffer;
    ID3D11SamplerState         *sampler;
  } d3d11;
} regenv_t;

/* stores information about a render target for the pipeline */
typedef struct
{
  rxdrawing_texture_t surface;

  struct
  { int x0,y0,x1,y1;
  } vport;
  struct
  { unsigned       enable: 1;
    int            mask;
    struct
    { D3D11_BLEND    src;
      D3D11_BLEND    dest;
      D3D11_BLEND_OP op; };
    struct
    { D3D11_BLEND    src;
      D3D11_BLEND    dest;
      D3D11_BLEND_OP op; } alpha;
  } blend;
} pipout_t;

typedef struct
{
  struct
  {
    ID3D11InputLayout     *in;

    ID3D11VertexShader    *vs;/* these have to be more intricate */
    ID3D11PixelShader     *ps;/* these have to be more intricate */

    ID3D11ComputeShader   *cs;

    /* (recommended) do not modify these directly */
    ID3D11RasterizerState *rastr_state;
    ID3D11BlendState      *blend_state;
  } d3d11;

  rxshader_t sha;
  pipout_t   out[0x10];
  regenv_t   reg[0x20];
  char       opt[0x10];/* find better way to store this, possibly just using flags */
} pipenv_t;

enum
{
  OPT_ZTST,
};

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

void
regset(
  int reg, void *res);

/* todo: these are temporary */
void
rxpipset_program(
  rxshader_t vs,
  rxshader_t ps);

void
rxpipset_sampler(
  int reg, rxsampler_t sampler );
void
rxpipset_texture(
  int reg, rxtexture_t texture );
void
rxpipset_varying(
  int reg, rxuniform_buffer_t buffer );

/* this struct is typedef'd should you want to avoid allocating it globally,
 for instance say you had your own global state and within that state you'd
 like to have the rx object, in such case simply use the appropriate macro to
 prevent this file from adding rx to the global scope. */
typedef struct rx_t rx_t;
typedef struct rx_t
{
  /* timing stuff */
            int   tick_count;
  ccclocktick_t   start_ticks;
  ccclocktick_t   frame_ticks;
  ccclocktick_t   total_ticks;
  ccclocktick_t   delta_ticks;
  /* todo!: there are more correct and robust ways to store time long term, @TomForsyth */
  double          total_seconds;
  double          delta_seconds;

  /* todo: constants, should be upper case */
  rxsampler_t          linear_sampler;
  rxsampler_t          point_sampler;
  rxsampler_t          anisotropic_sampler;
  rxtexture_t          white;

  /* main stuff */
  struct
  { struct
    { ID3D11InfoQueue        *inf;
      ID3D11Device           *dev;
      ID3D11DeviceContext    *ctx; };
  } d3d11;

  rxmatrix_t     view_matrix;
  rxmatrix_t     world_matrix;

  /* immediate mode pass */
  struct
  { struct
    { rxmatrix_t matrix;

      rxvec2_t   xyscreen;
      rxvec2_t   xysource;/* this should be an array */
      rxvec2_t   xycursor;

      double     total_seconds;
      double     delta_seconds;
    } var;

    rxuniform_buffer_t  varying;

    struct
    { rxdrawing_texture_t surface;
      rxstencil_texture_t stencil; };

    struct
    {
      rxshader_t sha_vtx_sdf;
      rxshader_t sha_pxl_sdf_cir;
      rxshader_t sha_pxl_sdf_box;

      rxshader_t sha_pxl_txt;

      rxshader_t sha_vtx;
      rxshader_t sha_pxl; };

    /* assembler buffers */
    rxvertex_buffer_t   asm_vtx;
    rxindex_buffer_t    asm_idx;

    /* todo: cleanup */
    rxborrowed_t       vertex_buffer_writeonly;
    rxvertex_imp_t    *vertex_array;
             int       vertex_tally;

    /* todo: cleanup */
    rxborrowed_t       index_buffer_writeonly;
    rxvindex_imp_t    *index_array;
             int       index_tally;

             int       index_offset;
    rxvertex_imp_t     attr;
  } imp;

  /* current pipeline state, not recommended to modify directly,
    use the appropriate functions instead which will flush if necessary */
  pipenv_t pip;

  /* whether the current pipeline differs from the one set in the render
    context (effectively the GPU) and should be uploaded */
  int      upl;

  /* window related structure, we only support one window for now but it'd be
   trivial to extend this */
  struct
  {
    unsigned  off: 1;
    unsigned  vis: 1;

    /* window dimensions */
    struct
    { int size_x;
      int size_y;
      int center_x;
      int center_y; };

    /* native window objects */
    struct
    {
      HWND obj; } win32;

    /* output media */
    struct
    { rxdrawing_texture_t tar;

      struct
      { IDXGISwapChain2 *swap_chain;
        void            *frame_await; } d3d11;
    } out;

    /* input handling members, these get updated once every tick */
    struct
    { struct
      { short    chr;

        /* prob find better way to store this - xxx rj */
        char     key_lst[0x100];
        char     key    [0x100];

        unsigned is_ctrl: 1;
        unsigned is_menu: 1;
        unsigned is_shft: 1;
      } kbrd;/*todo: this should be an array*/
      struct
      { int xcursor;
        int ycursor;
        int yscroll;
        int xscroll;

        int  xclick;
        int  yclick;

        int btn_old;
        int btn_cur;
      } mice;/*todo: this should be an array*/
    } in;
  } wnd;

  /* basic platform specific stuff */
  struct
  {
    struct
    {
      HCURSOR arrow;
    } cursor;
  } win32;

  /* this is for the built-in font */
  rxtexture_t     font_atlas;
  rxvec2i16_t     font_glyph[95];
  float           font_ysize;
  float           font_xsize;
} rx_t;


/* the source of all evil is here */
ccglobal rx_t rx;

/* section: basic system functions */

void
rxsys_setcur(/*todo*/HCURSOR cursor)
{
  SetCursor(cursor);
}

void
rxsys_ini()
{
  /* todo: */
  rx.win32.cursor.arrow = LoadCursorA(NULL,IDC_ARROW);
}

/* section: windowing functions */
int
rxwndmsg_handler_win32(UINT,WPARAM,LPARAM);

LRESULT CALLBACK
rxwndmsg_callback_win32(HWND,UINT,WPARAM,LPARAM);

ccfunc void
rxwnd_poll();


/* todo: rename */
#ifndef       WAS_DOWN
#define       WAS_DOWN(x) ((rx.wnd.in.mice.btn_old & (1 << x)) != 0)
# endif
#ifndef        IS_DOWN
#define        IS_DOWN(x) ((rx.wnd.in.mice.btn_cur & (1 << x)) != 0)
# endif
#ifndef IS_CLICK_LEAVE
#define IS_CLICK_LEAVE(x) !IS_DOWN(x) &&  WAS_DOWN(x)
# endif
#ifndef IS_CLICK_ENTER
#define IS_CLICK_ENTER(x)  IS_DOWN(x) && !WAS_DOWN(x)
# endif

ccfunc ccinle int rxisctrl()
{ return rx.wnd.in.kbrd.is_ctrl; }

ccfunc ccinle int rxismenu()
{ return rx.wnd.in.kbrd.is_menu; }

ccfunc ccinle int rxisshft()
{ return rx.wnd.in.kbrd.is_shft; }

ccfunc ccinle int rxtstbtn(int x)
{ return IS_DOWN(x); }

int
rxtstkey(int x)
{
  return rx.wnd.in.kbrd.key[x] != 0;
}

int rxchr()
{
  return rx.wnd.in.kbrd.chr;
}

#include "rx.shader.cc"

void rxunknown_delete(void *unknown)
{
  if(unknown != 0)
  {
    IUnknown_Release((rxunknown_t)(unknown));
  }
}

void rxvertex_buffer_delete(rxvertex_buffer_t buffer)
{
  rxunknown_delete(buffer.unknown);
}

void rxindex_buffer_delete(rxindex_buffer_t buffer)
{
  rxunknown_delete(buffer.unknown);
}

/* section: pipeline config */
void
rxpipset_program(
  rxshader_t vs,
  rxshader_t ps )
{
  if( rx.pip.d3d11.vs != vs.d3d11.vertex_shader )
  {
    rximp_flush();

    rx.pip.d3d11.vs = vs.d3d11.vertex_shader;
    rx.pip.d3d11.in = vs.d3d11.layout;
    rx.upl = TRUE;
  }

  if( rx.pip.d3d11.ps != ps.d3d11.pixel_shader )
  {
    rximp_flush();

    rx.pip.d3d11.ps = ps.d3d11.pixel_shader;
    rx.upl = TRUE;
  }
}

void
optset(int opt, int val)
{
  if(rx.pip.opt[opt] != val)
  {
    rximp_flush();

    rx.pip.opt[opt] = val;
    rx.upl = TRUE;
  }
}

void
regset(
  int reg, void *val)
{
  if(rx.pip.reg[reg].val != val)
  {
    rximp_flush();

    rx.pip.reg[reg].val  = val;
    rx.upl = TRUE;
  }
}

/* todo: this should take the id of the resource */
void
rxpipset_sampler(
  int reg, rxsampler_t sampler )
{
  regset(reg,sampler.d3d11.state);
}

void
rxpipset_texture(
  int reg, rxtexture_t texture )
{
  regset(reg,texture.d3d11.view);

  /* todo: this shouldn't have to be set here */
  rx.imp.var.xysource.x = texture.size_x;
  rx.imp.var.xysource.y = texture.size_y;
}


/* todo: this should take the id of the resource */
void
rxpipset_varying(
  int reg, rxuniform_buffer_t buffer )
{
  regset(reg,buffer.unknown);
}

void
pipupl()
{
  if(rx.upl != TRUE)
  {
    return;
  }

  rx.upl = FALSE;

  rxmatrix_t matrix = rxmatrix_multiply(rx.world_matrix,rx.view_matrix);

  rx.imp.var.       matrix=matrix;
  rx.imp.var.xyscreen.x=(float)(rx.wnd.size_x);
  rx.imp.var.xyscreen.y=(float)(rx.wnd.size_y);
  rx.imp.var.xycursor.x=(float)(rx.wnd.in.mice.xcursor) / rx.wnd.size_x; // todo!!:
  rx.imp.var.xycursor.y=(float)(rx.wnd.in.mice.ycursor) / rx.wnd.size_y; // todo!!:
  rx.imp.var.total_seconds=rx.total_seconds;
  rx.imp.var.delta_seconds=rx.delta_seconds;

  /* todo: only update if necessary */
  rxuniform_buffer_update(rx.imp.varying,&rx.imp.var,sizeof(rx.imp.var));

  /* todo */
  ID3D11DeviceContext_OMSetDepthStencilState(rx.d3d11.ctx,
    rx.imp.stencil.d3d11.state,1);

  // ID3D11DeviceContext_OMSetDepthStencilState(rx.d3d11.ctx,
  //   rx.pip.opt[OPT_ZTST] ? rx.imp.stencil.d3d11.state : NULL,1);

  ID3D11DeviceContext_VSSetShader(rx.d3d11.ctx,rx.pip.d3d11.vs,0x00,0);
  ID3D11DeviceContext_PSSetShader(rx.d3d11.ctx,rx.pip.d3d11.ps,0x00,0);
  ID3D11DeviceContext_CSSetShader(rx.d3d11.ctx,rx.pip.d3d11.cs,0x00,0);

  for(int i=0;i<2;i+=1)
  { ID3D11DeviceContext_VSSetConstantBuffers(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_VS_BLC_0+i].d3d11.buffer);
    ID3D11DeviceContext_PSSetConstantBuffers(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_PS_BLC_0+i].d3d11.buffer);
    ID3D11DeviceContext_CSSetConstantBuffers(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_CS_BLC_0+i].d3d11.buffer);

    ID3D11DeviceContext_VSSetShaderResources(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_VS_TEX_0+i].d3d11.texture_2d);
    ID3D11DeviceContext_PSSetShaderResources(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_PS_TEX_0+i].d3d11.texture_2d);
    ID3D11DeviceContext_CSSetShaderResources(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_CS_TEX_0+i].d3d11.texture_2d);

    ID3D11DeviceContext_VSSetSamplers(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_VS_SAM_0+i].d3d11.sampler);
    ID3D11DeviceContext_PSSetSamplers(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_PS_SAM_0+i].d3d11.sampler);
    ID3D11DeviceContext_CSSetSamplers(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_CS_SAM_0+i].d3d11.sampler);
  }

  /* pending: this a good placement? */
  ID3D11DeviceContext_IASetInputLayout(rx.d3d11.ctx,rx.pip.d3d11.in);

  /* ensure the proper render target is set */
  ID3D11DeviceContext_OMSetRenderTargets(rx.d3d11.ctx,
    1,&rx.imp.surface.d3d11.view,rx.imp.stencil.d3d11.view);

  unsigned int Stride=sizeof(rxvertex_imp_t);
  unsigned int Offset=0;

  ID3D11DeviceContext_IASetVertexBuffers(rx.d3d11.ctx,0,1,
    (ID3D11Buffer**)&rx.imp.asm_vtx.unknown,&Stride,&Offset);
  ID3D11DeviceContext_IASetIndexBuffer(rx.d3d11.ctx,
    (ID3D11Buffer *) rx.imp.asm_idx.unknown,DXGI_FORMAT_R32_UINT,0);
  /* get this from the right place */
  // D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
  ID3D11DeviceContext_IASetPrimitiveTopology(rx.d3d11.ctx,
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

#include "rx.buffer.cc"

/* todo: deprecated, should use the appropiate pipenv function */
void rxpipset_viewport(float w, float h)
{ D3D11_VIEWPORT viewport_d3d;
  viewport_d3d.TopLeftX=0;
  viewport_d3d.TopLeftY=0;
  viewport_d3d.   Width=w;
  viewport_d3d.  Height=h;
  viewport_d3d.MinDepth=0;
  viewport_d3d.MaxDepth=1;
  ID3D11DeviceContext_RSSetViewports(rx.d3d11.ctx,1,&viewport_d3d);
}

ccfunc void
rx3d()
{
  /* todo: */
  rx.world_matrix = rxmatrix_identity();
  rx. view_matrix = rxmatrix_identity();
  rx. view_matrix = rxmatrix_projection(
    (double)(rx.wnd.size_y)/(double)(rx.wnd.size_x),90,0.01,1000);

  optset(OPT_ZTST,TRUE);
}

ccfunc void
rx2d()
{
  rx.world_matrix = rxmatrix_identity();
  rx. view_matrix = rxmatrix_identity();
  rx.view_matrix.m[0][0]=  2. / rx.wnd.size_x;
  rx.view_matrix.m[1][1]=  2. / rx.wnd.size_y;
  rx.view_matrix.m[2][2]= .5;
  rx.view_matrix.m[3][0]=- 1.;
  rx.view_matrix.m[3][1]=- 1.;

  optset(OPT_ZTST,FALSE);
}

void rximp_clip(int x0, int y0, int x1, int y1)
{
  ccassert(x0 <= x1);
  ccassert(y0 <= y1);

  /* todo: */
  x0 = rxclampi(x0,0,rx.wnd.size_x);
  y0 = rxclampi(y0,0,rx.wnd.size_y);
  x1 = rxclampi(x1,0,rx.wnd.size_x);
  y1 = rxclampi(y1,0,rx.wnd.size_y);

  D3D11_RECT rect_d3d;
  rect_d3d.left  = x0;
  rect_d3d.top   = rx.wnd.size_y - y1;
  rect_d3d.right = x1;
  rect_d3d.bottom= rx.wnd.size_y - y0;
  ID3D11DeviceContext_RSSetScissorRects(rx.d3d11.ctx,1,&rect_d3d);
}

ccfunc ccinle void
rxend()
{
}

ccfunc void
rxbgn()
{
  rx.imp.attr.xyzw.x =  0;
  rx.imp.attr.xyzw.y =  0;
  rx.imp.attr.xyzw.z = .5;
  rx.imp.attr.xyzw.w =  1;
  rx.imp.attr.rgba.r =  0;
  rx.imp.attr.rgba.g =  0;
  rx.imp.attr.rgba.b = .5;
  rx.imp.attr.rgba.a =  1;
  rx.imp.attr.  uv.x =  0;
  rx.imp.attr.  uv.y =  0;

  rx.imp.index_offset = rx.imp.vertex_tally;

  if(rx.imp.vertex_array == 0)
  {
    rx.imp.vertex_buffer_writeonly = rxborrow_vertex_buffer(rx.imp.asm_vtx);
    rx.imp. index_buffer_writeonly =  rxborrow_index_buffer(rx.imp.asm_idx);
    rx.imp. vertex_array=rx.imp.vertex_buffer_writeonly.memory;
    rx.imp.  index_array=rx.imp. index_buffer_writeonly.memory;
    rx.imp. vertex_tally=0;
    rx.imp.  index_tally=0;

    rx.imp.index_offset =0;
  }

  ccassert(rx.imp.vertex_array != 0);
  ccassert(rx.imp. index_array != 0);
}

ccfunc ccinle void
rxaddidx(rxvindex_imp_t value)
{
  if(rx.imp.index_tally < RX_INDEX_BUFFER_SIZE)
  {
    rx.imp.index_array[
    rx.imp.index_tally] = rx.imp.index_offset + value;

    rx.imp.index_tally += 1;
  }
}

ccfunc ccinle void
rxaddnidx(int num, ...)
{
  va_list vli;
  va_start(vli,num);

  for(int i=0;i<num;i+=1)
  {
    rxaddidx(va_arg(vli,rxvindex_imp_t));
  }

  va_end(vli);
}

ccfunc ccinle void
rxaddvtx(rxvertex_imp_t vertex)
{
  if(rx.imp.vertex_tally < RX_VERTEX_BUFFER_SIZE)
  {
    rx.imp.vertex_array[
    rx.imp.vertex_tally] = vertex;

    rx.imp.vertex_tally += 1;
  }
}

ccfunc ccinle void
rxaddnvtx(int num, ...)
{
  va_list vli;
  va_start(vli,num);

  for(int i=0;i<num;i+=1)
  {
    rxaddvtx(va_arg(vli,rxvertex_imp_t));
  }

  va_end(vli);
}

ccfunc ccinle rxvertex_imp_t
rxvtx_xy(float x, float y)
{
  rx.imp.attr.xyzw.x =  x;
  rx.imp.attr.xyzw.y =  y;
  return rx.imp.attr;
}

ccfunc ccinle rxvertex_imp_t
rxvtx_xyuv(float x, float y, float u, float v)
{
  rx.imp.attr.xyzw.x =  x;
  rx.imp.attr.xyzw.y =  y;
  rx.imp.attr.xyzw.z = .5;
  rx.imp.attr.xyzw.w =  1;
  rx.imp.attr.  uv.x =  u;
  rx.imp.attr.  uv.y =  v;
  return rx.imp.attr;
}

ccfunc ccinle rxvertex_imp_t
rxvtx_xyuv_col(float x, float y, float u, float v, rxcolor_t rgba)
{
  rx.imp.attr.xyzw.x =  x;
  rx.imp.attr.xyzw.y =  y;
  rx.imp.attr.xyzw.z = .5;
  rx.imp.attr.xyzw.w =  1;
  rx.imp.attr.  uv.x =  u;
  rx.imp.attr.  uv.y =  v;
  rx.imp.attr.rgba   = rgba;
  return rx.imp.attr;
}

#include "rx.texture.cc"


/* todo: temporary */
ccfunc ccinle void
rximp_clear()
{
  float clear_color[]={.0f,.0f,.0f,1.f};

#ifdef _RX_ENABLE_DEPTH_STENCIL
  ID3D11DeviceContext_ClearDepthStencilView(
    rx.d3d11.ctx,rx.imp.stencil.d3d11.view,D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.0f,0);
#endif

  ID3D11DeviceContext_ClearRenderTargetView(rx.d3d11.ctx,
    rx.imp.surface.d3d11.view,clear_color);
}

ccfunc ccinle void
rximp_apply()
{
  rxpipset_program(
  	rx.imp.sha_vtx,
    rx.imp.sha_pxl );

  rx2d();

  rxpipset_varying(REG_VS_BLC_0,rx.imp.varying);
  rxpipset_varying(REG_PS_BLC_0,rx.imp.varying);
}

#include "rx.win32.cc"

ccfunc void
rximp_flush()
{
  if(rx.imp.index_tally != 0)
  {
    /* return immediate mode resources */
    rxreturn(rx.imp.vertex_buffer_writeonly);
    rxreturn(rx.imp. index_buffer_writeonly);
    rx.imp.vertex_array=ccnull;
    rx.imp. index_array=ccnull;

    /* upload pipeline changes if necessary */
    pipupl();

    ID3D11DeviceContext_DrawIndexed(rx.d3d11.ctx,rx.imp.index_tally,0,0);
  }

  rx.imp.index_tally = 0;
  rx.imp.vertex_tally = 0;
}


void rxtime()
{
  ccclocktick_t ticks=ccclocktick();

  rx.total_ticks=ticks-rx.start_ticks;
  rx.total_seconds=ccclocksecs(rx.total_ticks);

  rx.delta_ticks=ticks-rx.frame_ticks;
  rx.delta_seconds=ccclocksecs(rx.delta_ticks);

  rx.frame_ticks=ticks;
}

int rxtick()
{
  rx.tick_count += 1;

  rxwnd_poll();

  /* todo: */
  rxsys_setcur(rx.win32.cursor.arrow);

  rximp_flush();

  /* this has to be formalized */
  rxtexture_copy(
    &rx.wnd.out.tar.texture ,
    &rx.imp.surface.texture );

  IDXGISwapChain_Present(rx.wnd.out.d3d11.swap_chain,1u,0);

  // todo!:
  WaitForSingleObjectEx(rx.wnd.out.d3d11.frame_await,INFINITE,TRUE);

  rxwnd_show();

  rximp_clear();

  rxtime();

  return !rx.wnd.off;
}

/* todo: revise */
rxstencil_texture_t
rxcreate_stencil_texture(
  int size_x, int size_y, int depth_enabled, int stencil_enabled)
{
  rxtexture_config_t texture_config =
    rxtexture_config_init(size_x,size_y,DXGI_FORMAT_D32_FLOAT,0,NULL,1,0,
      D3D11_USAGE_DEFAULT,D3D11_BIND_DEPTH_STENCIL,0);

  rxtexture_t texture;
  rxtexture_init(&texture,&texture_config);

  ID3D11DepthStencilView *stencil_view_d3d;
  ID3D11Device_CreateDepthStencilView(rx.d3d11.dev,
    texture_config.d3d11.resource,NULL,&stencil_view_d3d);

  D3D11_DEPTH_STENCIL_DESC stencil_config_d3d;
  stencil_config_d3d.     DepthEnable=depth_enabled;
  stencil_config_d3d.   StencilEnable=stencil_enabled;
  stencil_config_d3d.  DepthWriteMask=D3D11_DEPTH_WRITE_MASK_ALL;
  stencil_config_d3d.       DepthFunc=D3D11_COMPARISON_LESS;
  stencil_config_d3d. StencilReadMask=D3D11_DEFAULT_STENCIL_READ_MASK;
  stencil_config_d3d.StencilWriteMask=D3D11_DEFAULT_STENCIL_WRITE_MASK;
  stencil_config_d3d.FrontFace.      StencilFailOp=D3D11_STENCIL_OP_KEEP;
  stencil_config_d3d.FrontFace. StencilDepthFailOp=D3D11_STENCIL_OP_DECR;
  stencil_config_d3d.FrontFace.      StencilPassOp=D3D11_STENCIL_OP_KEEP;
  stencil_config_d3d.FrontFace.        StencilFunc=D3D11_COMPARISON_ALWAYS;
  stencil_config_d3d. BackFace.      StencilFailOp=D3D11_STENCIL_OP_KEEP;
  stencil_config_d3d. BackFace. StencilDepthFailOp=D3D11_STENCIL_OP_DECR;
  stencil_config_d3d. BackFace.      StencilPassOp=D3D11_STENCIL_OP_KEEP;
  stencil_config_d3d. BackFace.        StencilFunc=D3D11_COMPARISON_ALWAYS;

  ID3D11DepthStencilState *stencil_d3d;
  ID3D11Device_CreateDepthStencilState(rx.d3d11.dev,&stencil_config_d3d,&stencil_d3d);

  rxstencil_texture_t stencil;
  stencil.texture     = texture;
  stencil.d3d11.state = stencil_d3d;
  stencil.d3d11.view  = stencil_view_d3d;
  return stencil;
}


void rxinit_default_font();

rxdrawing_texture_t
rxcreate_window_surface()
{
  IDXGIFactory2 * DXGIFactory=NULL;
  IDXGIDevice   *  DXGIDevice=NULL;
  IDXGIAdapter  * DXGIAdapter=NULL;
  ID3D11Device_QueryInterface(rx.d3d11.dev,&IID_IDXGIDevice,(void **)&DXGIDevice);
  IDXGIDevice_GetAdapter(DXGIDevice,&DXGIAdapter);
  IDXGIAdapter_GetParent(DXGIAdapter,&IID_IDXGIFactory2,(void**)&DXGIFactory);
  IDXGIAdapter_Release(DXGIAdapter);
  IDXGIDevice_Release(DXGIDevice);

  DXGI_SWAP_CHAIN_DESC1 swapchain_config_d3d;
  ZeroMemory(&swapchain_config_d3d,sizeof(swapchain_config_d3d));
  swapchain_config_d3d.      Width=rx.wnd.size_x;
  swapchain_config_d3d.     Height=rx.wnd.size_y;
  swapchain_config_d3d.     Format=DXGI_FORMAT_R8G8B8A8_UNORM;
  swapchain_config_d3d.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT; // DXGI_USAGE_UNORDERED_ACCESS
  swapchain_config_d3d.BufferCount=2;
  swapchain_config_d3d. SwapEffect=DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
  swapchain_config_d3d.      Flags=DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH|
                            DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
  /* todo: allow the user to specify this */
  swapchain_config_d3d. SampleDesc.  Count=1;
  swapchain_config_d3d. SampleDesc.Quality=0;

  DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapchain_fullscreen_config_d3d;
  ZeroMemory(&swapchain_fullscreen_config_d3d,sizeof(swapchain_fullscreen_config_d3d));
  swapchain_fullscreen_config_d3d.RefreshRate.  Numerator=_RX_REFRESH_RATE;
  swapchain_fullscreen_config_d3d.RefreshRate.Denominator=1;
  swapchain_fullscreen_config_d3d.Windowed               =TRUE;

  IDXGIFactory2_CreateSwapChainForHwnd(DXGIFactory,
    (IUnknown *) rx.d3d11.dev,rx.wnd.win32.obj,
    &swapchain_config_d3d,&swapchain_fullscreen_config_d3d,NULL,
      (IDXGISwapChain1 **)&rx.wnd.out.d3d11.swap_chain);

  ID3D11Texture2D          *texture_d3d;
  ID3D11RenderTargetView   *texture_view_d3d;
  IDXGISwapChain_GetBuffer(rx.wnd.out.d3d11.swap_chain,0,&IID_ID3D11Texture2D,(void **)&texture_d3d);

  IDXGIFactory_Release(DXGIFactory);

  D3D11_RENDER_TARGET_VIEW_DESC view_config_d3d;
  ZeroMemory(&view_config_d3d,sizeof(view_config_d3d));
  view_config_d3d.       Format=DXGI_FORMAT_UNKNOWN;
  view_config_d3d.ViewDimension=D3D11_RTV_DIMENSION_TEXTURE2D;

  ID3D11Device_CreateRenderTargetView(rx.d3d11.dev,
    (ID3D11Resource*)texture_d3d,&view_config_d3d,&texture_view_d3d);

  rx.wnd.out.d3d11.frame_await = IDXGISwapChain2_GetFrameLatencyWaitableObject(rx.wnd.out.d3d11.swap_chain);

  rxdrawing_texture_t drawing;
  drawing.texture.size_x           = swapchain_config_d3d. Width;
  drawing.texture.size_y           = swapchain_config_d3d.Height;
  drawing.texture.format           = swapchain_config_d3d.Format;
  drawing.texture.d3d11.texture_2d = texture_d3d;
  drawing.texture.d3d11.view       = NULL;
  drawing.d3d11.view               = texture_view_d3d;

  return drawing;
}

void rxinit(const wchar_t *window_title)
{
  UINT DriverModeFlags=
#ifdef _CCDEBUG
    D3D11_CREATE_DEVICE_DEBUG| // -- Note: COMMENT THIS OUT TO USE INTEL'S GRAPHIC ANALYZER
    D3D11_CREATE_DEVICE_SINGLETHREADED|
#endif
    D3D11_CREATE_DEVICE_BGRA_SUPPORT;

  D3D_FEATURE_LEVEL DriverFeatureMenu[2][2]=
  { {D3D_FEATURE_LEVEL_11_1,D3D_FEATURE_LEVEL_11_0},
    {D3D_FEATURE_LEVEL_10_1,D3D_FEATURE_LEVEL_10_0},
  };
  D3D_FEATURE_LEVEL DriverSelectedFeatureLevel;
  if(SUCCEEDED(D3D11CreateDevice(NULL,D3D_DRIVER_TYPE_HARDWARE,0,DriverModeFlags,DriverFeatureMenu[0],
      ARRAYSIZE(DriverFeatureMenu[0]),D3D11_SDK_VERSION,&rx.d3d11.dev,&DriverSelectedFeatureLevel,&rx.d3d11.ctx))||
     SUCCEEDED(D3D11CreateDevice(NULL,D3D_DRIVER_TYPE_WARP,0,DriverModeFlags,DriverFeatureMenu[1],
      ARRAYSIZE(DriverFeatureMenu[1]),D3D11_SDK_VERSION,&rx.d3d11.dev,&DriverSelectedFeatureLevel,&rx.d3d11.ctx)))
  { if((DriverModeFlags&D3D11_CREATE_DEVICE_DEBUG))
    { if(SUCCEEDED(IProvideClassInfo_QueryInterface(rx.d3d11.dev,&IID_ID3D11InfoQueue,(void**)&rx.d3d11.inf)))
      {
        ID3D11InfoQueue_SetBreakOnSeverity(rx.d3d11.inf, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        ID3D11InfoQueue_SetBreakOnSeverity(rx.d3d11.inf, D3D11_MESSAGE_SEVERITY_ERROR,      TRUE);
        ID3D11InfoQueue_SetBreakOnSeverity(rx.d3d11.inf, D3D11_MESSAGE_SEVERITY_WARNING,    TRUE);
      }
    }
  }

  rxsys_ini();
  rxwindow_init(window_title);

  typedef BOOL WINAPI _YYY_(void);
  typedef BOOL WINAPI _XXX_(DPI_AWARENESS_CONTEXT);
  HMODULE user32_dll = LoadLibraryA("user32.dll");
  _XXX_ *XXX = (_XXX_ *) GetProcAddress(user32_dll, "SetProcessDPIAwarenessContext");
  _YYY_ *YYY = (_YYY_ *) GetProcAddress(user32_dll, "SetProcessDPIAware");

  if(XXX) XXX(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE); else
  if(YYY) YYY();

  FreeLibrary(user32_dll);

  rxwnd_poll();

  {
    // note: we can use the adapter to enumerate display devices,
    // this might come useful to the user!

    rx.wnd.out.tar = rxcreate_window_surface();

    /* determine the proper size for this */
    rx.white = rxtexture_create(512,512,rxRGBA8888);
    rxborrowed_t t = rxtexture_borrow(rx.white);
    memset(t.memory,0xff,t.stride*rx.white.size_y);
    rxreturn(t);

    /* create default raster stage */
    rx.imp.varying = rxuniform_buffer_create(sizeof(rx.imp.var),NULL);
    rx.imp.surface = rxdrawing_texture_create(
        rx.wnd.out.tar.texture.size_x,
        rx.wnd.out.tar.texture.size_y,
        rx.wnd.out.tar.texture.format,_RX_MSAA,0);

    rx.imp.sha_vtx         = rxshader_create(RX_OBJECT_TYPE_kVERTEX_SHADER,sizeof(rx_vs_shader_bytecode),(void*)rx_vs_shader_bytecode);
    rx.imp.sha_pxl         = rxshader_create(RX_OBJECT_TYPE_kPIXEL_SHADER ,sizeof(rx_ps_shader_bytecode),(void*)rx_ps_shader_bytecode);
    rx.imp.sha_pxl_txt     = rxshader_create(RX_OBJECT_TYPE_kPIXEL_SHADER ,sizeof(rx_ps_txt_sb),(void*)rx_ps_txt_sb);
    rx.imp.sha_vtx_sdf     = rxshader_create(RX_OBJECT_TYPE_kVERTEX_SHADER,sizeof(rx_vs_sdf_sb),(void*)rx_vs_sdf_sb);
    rx.imp.sha_pxl_sdf_cir = rxshader_create(RX_OBJECT_TYPE_kPIXEL_SHADER ,sizeof(rx_ps_sdf_cir_sb),(void*)rx_ps_sdf_cir_sb);
    rx.imp.sha_pxl_sdf_box = rxshader_create(RX_OBJECT_TYPE_kPIXEL_SHADER ,sizeof(rx_ps_sdf_box_sb),(void*)rx_ps_sdf_box_sb);


#ifdef _RX_ENABLE_DEPTH_STENCIL
    rx.imp.stencil = rxcreate_stencil_texture(
      rx.wnd.out.tar.texture.size_x,
      rx.wnd.out.tar.texture.size_y,FALSE,FALSE);
#endif

    rx.imp.asm_idx =  rxcreate_index_buffer(sizeof(rxvindex_imp_t),  RX_INDEX_BUFFER_SIZE);
    rx.imp.asm_vtx = rxcreate_vertex_buffer(sizeof(rxvertex_imp_t), RX_VERTEX_BUFFER_SIZE);
  }

  /* todo */
  D3D11_RASTERIZER_DESC raster_config_d3d;
  ZeroMemory(&raster_config_d3d,sizeof(raster_config_d3d));
  raster_config_d3d.             FillMode=D3D11_FILL_SOLID;
  raster_config_d3d.             CullMode=D3D11_CULL_NONE;
  raster_config_d3d.FrontCounterClockwise=FALSE;
  raster_config_d3d.            DepthBias=D3D11_DEFAULT_DEPTH_BIAS;
  raster_config_d3d.       DepthBiasClamp=D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
  raster_config_d3d. SlopeScaledDepthBias=D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
  raster_config_d3d.      DepthClipEnable= FALSE;
  raster_config_d3d.        ScissorEnable= TRUE;
  raster_config_d3d.    MultisampleEnable= _RX_MSAA >= 2;
  raster_config_d3d.AntialiasedLineEnable= FALSE;
  ID3D11Device_CreateRasterizerState(rx.d3d11.dev,&raster_config_d3d,&rx.pip.d3d11.rastr_state);

  D3D11_BLEND_DESC blender_config_d3d;
  ZeroMemory(&blender_config_d3d,sizeof(blender_config_d3d));
  for(int I=0; I<1; ++I)
  { blender_config_d3d.RenderTarget[I].          BlendEnable=TRUE;
    blender_config_d3d.RenderTarget[I].             SrcBlend=D3D11_BLEND_SRC_ALPHA;
    blender_config_d3d.RenderTarget[I].            DestBlend=D3D11_BLEND_INV_SRC_ALPHA;
    blender_config_d3d.RenderTarget[I].              BlendOp=D3D11_BLEND_OP_ADD;

    blender_config_d3d.RenderTarget[I].        SrcBlendAlpha=D3D11_BLEND_ZERO;
    blender_config_d3d.RenderTarget[I].       DestBlendAlpha=D3D11_BLEND_ZERO;
    blender_config_d3d.RenderTarget[I].         BlendOpAlpha=D3D11_BLEND_OP_ADD;
    blender_config_d3d.RenderTarget[I].RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;
  }
  ID3D11Device_CreateBlendState(rx.d3d11.dev,&blender_config_d3d,&rx.pip.d3d11.blend_state);

  D3D11_SAMPLER_DESC SamplerInfo;
  ZeroMemory(&SamplerInfo,sizeof(SamplerInfo));
  SamplerInfo.AddressU=D3D11_TEXTURE_ADDRESS_WRAP;
  SamplerInfo.AddressV=D3D11_TEXTURE_ADDRESS_WRAP;
  SamplerInfo.AddressW=D3D11_TEXTURE_ADDRESS_WRAP;

  SamplerInfo.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  ID3D11Device_CreateSamplerState(rx.d3d11.dev,&SamplerInfo,&rx.linear_sampler.d3d11.state);

  SamplerInfo.Filter=D3D11_FILTER_MIN_MAG_MIP_POINT;
  ID3D11Device_CreateSamplerState(rx.d3d11.dev,&SamplerInfo,&rx.point_sampler.d3d11.state);

  SamplerInfo.Filter=D3D11_FILTER_ANISOTROPIC;
  ID3D11Device_CreateSamplerState(rx.d3d11.dev,&SamplerInfo,&rx.anisotropic_sampler.d3d11.state);

  rximp_clip(0,0,rx.wnd.size_x,rx.wnd.size_y);

  rxpipset_viewport(rx.wnd.size_x,rx.wnd.size_y);

  ID3D11DeviceContext_RSSetState(rx.d3d11.ctx,rx.pip.d3d11.rastr_state);
  ID3D11DeviceContext_OMSetBlendState(rx.d3d11.ctx,rx.pip.d3d11.blend_state,0x00,0xFFFFFFFu);

  rxinit_default_font();

  rx.start_ticks=ccclocktick();
  rx.frame_ticks=rx.start_ticks;
  rxtime();
}

void load_bitmap8(unsigned char *memory, int stride, unsigned char *source)
{
  for(int y=0; y<16; ++y)
  { unsigned short bitrow=*source++;

    rxcolor8_t *color = (rxcolor8_t*) memory;

    for(int x=0; x<8; ++x)
    {
      unsigned char bp;
      bp=(bitrow & 0x1)*0xff;

      color[0]=RX_TLIT(rxcolor8_t){bp,bp,bp,bp};

      ++color;
      bitrow >>= 1;
    }
    memory += stride;
  }
}

void rxinit_default_font()
{
//
// please forgive me
//
// credits: https://datagoblin.itch.io/monogram
//
unsigned char encoded[95][16]=
{ {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,16,16,16,16,16,0,16,0,0,0,0},
  {0,0,0,0,0,40,40,40,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,40,124,40,40,124,40,0,0,0,0},
  {0,0,0,0,0,16,120,20,56,80,60,16,0,0,0,0},
  {0,0,0,0,0,68,68,32,16,8,68,68,0,0,0,0},
  {0,0,0,0,0,24,36,36,120,36,36,88,0,0,0,0},
  {0,0,0,0,0,16,16,16,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,32,16,16,16,16,16,32,0,0,0,0},
  {0,0,0,0,0,8,16,16,16,16,16,8,0,0,0,0},
  {0,0,0,0,0,0,16,84,56,84,16,0,0,0,0,0},
  {0,0,0,0,0,0,16,16,124,16,16,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,16,16,8,0,0,0},
  {0,0,0,0,0,0,0,0,124,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,16,16,0,0,0,0},
  {0,0,0,0,0,64,64,32,16,8,4,4,0,0,0,0},
  {0,0,0,0,0,56,68,100,84,76,68,56,0,0,0,0},
  {0,0,0,0,0,16,24,16,16,16,16,124,0,0,0,0},
  {0,0,0,0,0,56,68,64,32,16,8,124,0,0,0,0},
  {0,0,0,0,0,56,68,64,48,64,68,56,0,0,0,0},
  {0,0,0,0,0,72,72,68,124,64,64,64,0,0,0,0},
  {0,0,0,0,0,124,4,60,64,64,68,56,0,0,0,0},
  {0,0,0,0,0,56,4,4,60,68,68,56,0,0,0,0},
  {0,0,0,0,0,124,64,64,32,16,16,16,0,0,0,0},
  {0,0,0,0,0,56,68,68,56,68,68,56,0,0,0,0},
  {0,0,0,0,0,56,68,68,120,64,68,56,0,0,0,0},
  {0,0,0,0,0,0,16,16,0,0,16,16,0,0,0,0},
  {0,0,0,0,0,0,16,16,0,0,16,16,8,0,0,0},
  {0,0,0,0,0,0,96,24,4,24,96,0,0,0,0,0},
  {0,0,0,0,0,0,0,124,0,124,0,0,0,0,0,0},
  {0,0,0,0,0,0,12,48,64,48,12,0,0,0,0,0},
  {0,0,0,0,0,56,68,64,32,16,0,16,0,0,0,0},
  {0,0,0,0,0,56,100,84,84,100,4,56,0,0,0,0},
  {0,0,0,0,0,56,68,68,68,124,68,68,0,0,0,0},
  {0,0,0,0,0,60,68,68,60,68,68,60,0,0,0,0},
  {0,0,0,0,0,56,68,4,4,4,68,56,0,0,0,0},
  {0,0,0,0,0,60,68,68,68,68,68,60,0,0,0,0},
  {0,0,0,0,0,124,4,4,60,4,4,124,0,0,0,0},
  {0,0,0,0,0,124,4,4,60,4,4,4,0,0,0,0},
  {0,0,0,0,0,56,68,4,116,68,68,56,0,0,0,0},
  {0,0,0,0,0,68,68,68,124,68,68,68,0,0,0,0},
  {0,0,0,0,0,124,16,16,16,16,16,124,0,0,0,0},
  {0,0,0,0,0,64,64,64,64,68,68,56,0,0,0,0},
  {0,0,0,0,0,68,36,20,12,20,36,68,0,0,0,0},
  {0,0,0,0,0,4,4,4,4,4,4,124,0,0,0,0},
  {0,0,0,0,0,68,108,84,68,68,68,68,0,0,0,0},
  {0,0,0,0,0,68,68,76,84,100,68,68,0,0,0,0},
  {0,0,0,0,0,56,68,68,68,68,68,56,0,0,0,0},
  {0,0,0,0,0,60,68,68,60,4,4,4,0,0,0,0},
  {0,0,0,0,0,56,68,68,68,68,68,56,96,0,0,0},
  {0,0,0,0,0,60,68,68,60,68,68,68,0,0,0,0},
  {0,0,0,0,0,56,68,4,56,64,68,56,0,0,0,0},
  {0,0,0,0,0,124,16,16,16,16,16,16,0,0,0,0},
  {0,0,0,0,0,68,68,68,68,68,68,56,0,0,0,0},
  {0,0,0,0,0,68,68,68,68,40,40,16,0,0,0,0},
  {0,0,0,0,0,68,68,68,68,84,108,68,0,0,0,0},
  {0,0,0,0,0,68,68,40,16,40,68,68,0,0,0,0},
  {0,0,0,0,0,68,68,40,16,16,16,16,0,0,0,0},
  {0,0,0,0,0,124,64,32,16,8,4,124,0,0,0,0},
  {0,0,0,0,0,48,16,16,16,16,16,48,0,0,0,0},
  {0,0,0,0,0,4,4,8,16,32,64,64,0,0,0,0},
  {0,0,0,0,0,24,16,16,16,16,16,24,0,0,0,0},
  {0,0,0,0,0,16,40,68,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,124,0,0,0,0},
  {0,0,0,0,0,8,16,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,120,68,68,68,120,0,0,0,0},
  {0,0,0,0,0,4,4,60,68,68,68,60,0,0,0,0},
  {0,0,0,0,0,0,0,56,68,4,68,56,0,0,0,0},
  {0,0,0,0,0,64,64,120,68,68,68,120,0,0,0,0},
  {0,0,0,0,0,0,0,56,68,124,4,56,0,0,0,0},
  {0,0,0,0,0,48,72,8,60,8,8,8,0,0,0,0},
  {0,0,0,0,0,0,0,120,68,68,68,120,64,56,0,0},
  {0,0,0,0,0,4,4,60,68,68,68,68,0,0,0,0},
  {0,0,0,0,0,16,0,24,16,16,16,124,0,0,0,0},
  {0,0,0,0,0,64,0,96,64,64,64,64,68,56,0,0},
  {0,0,0,0,0,4,4,68,36,28,36,68,0,0,0,0},
  {0,0,0,0,0,12,8,8,8,8,8,112,0,0,0,0},
  {0,0,0,0,0,0,0,60,84,84,84,84,0,0,0,0},
  {0,0,0,0,0,0,0,60,68,68,68,68,0,0,0,0},
  {0,0,0,0,0,0,0,56,68,68,68,56,0,0,0,0},
  {0,0,0,0,0,0,0,60,68,68,68,60,4,4,0,0},
  {0,0,0,0,0,0,0,120,68,68,68,120,64,64,0,0},
  {0,0,0,0,0,0,0,52,76,4,4,4,0,0,0,0},
  {0,0,0,0,0,0,0,120,4,56,64,60,0,0,0,0},
  {0,0,0,0,0,8,8,60,8,8,8,112,0,0,0,0},
  {0,0,0,0,0,0,0,68,68,68,68,120,0,0,0,0},
  {0,0,0,0,0,0,0,68,68,68,40,16,0,0,0,0},
  {0,0,0,0,0,0,0,68,68,84,84,40,0,0,0,0},
  {0,0,0,0,0,0,0,68,40,16,40,68,0,0,0,0},
  {0,0,0,0,0,0,0,68,68,68,68,120,64,56,0,0},
  {0,0,0,0,0,0,0,124,32,16,8,124,0,0,0,0},
  {0,0,0,0,0,32,16,16,8,16,16,32,0,0,0,0},
  {0,0,0,0,0,16,16,16,16,16,16,16,0,0,0,0},
  {0,0,0,0,0,8,16,16,32,16,16,8,0,0,0,0},
  {0,0,0,0,0,0,0,72,52,0,0,0,0,0,0,0},
};

  rxtexture_t texture=rxtexture_create(512,256,rxRGBA8888);

  rxborrowed_t b=rxtexture_borrow(texture);

            int  stride = b.stride;
  unsigned char *memory = b.memory;

  int index;
  for(index=0; index<ccCarrlenL(encoded); ++index)
  { int xcursor=index % 32 * 16;
    int ycursor=index / 32 * 16;

    unsigned char *write;

    write=memory+stride*ycursor+xcursor*sizeof(rxcolor8_t);

    load_bitmap8(write,stride,encoded[index]);

    rxvec2i16_t p;
    p.x=(short)xcursor;
    p.y=(short)ycursor;

    rx.font_glyph[index]=p;
  }

  rxreturn(b);

  rx.font_atlas=texture;
  rx.font_ysize=16;
  rx.font_xsize= 6;
}


/* section: basic drawing functions */
/* todo: instead of center based we should take corner based arguments, makes everything easier */
void
draw_box_sdf(
  rxvec2_t center, rxvec2_t radius, rxcolor_t color, float roundness, float softness )
{
  float x0,y0,x1,y1;
  x0 = center.x - (radius.x + softness);
  y0 = center.y - (radius.y + softness);
  x1 = center.x + (radius.x + softness);
  y1 = center.y + (radius.y + softness);

  rxpipset_program(rx.imp.sha_vtx_sdf,
         rx.imp.sha_pxl_sdf_box);
  rxbgn();
    rx.imp.attr.rect.xy     = rxvec2_xy(x0,y0);
    rx.imp.attr.rect.xyxy   = rxvec4_xyzw(center.x,center.y,radius.x,radius.y);
    rx.imp.attr.rect.rgba   = color;
    rx.imp.attr.rect.flag.x = roundness;
    rx.imp.attr.rect.flag.w = softness;
    rxaddvtx(rx.imp.attr);

    rx.imp.attr.rect.xy    = rxvec2_xy(x0,y1);
    rx.imp.attr.rect.xyxy  = rxvec4_xyzw(center.x,center.y,radius.x,radius.y);
    rx.imp.attr.rect.rgba  = color;
    rx.imp.attr.rect.flag.x = roundness;
    rx.imp.attr.rect.flag.w = softness;
    rxaddvtx(rx.imp.attr);

    rx.imp.attr.rect.xy    = rxvec2_xy(x1,y1);
    rx.imp.attr.rect.xyxy  = rxvec4_xyzw(center.x,center.y,radius.x,radius.y);
    rx.imp.attr.rect.rgba  = color;
    rx.imp.attr.rect.flag.x = roundness;
    rx.imp.attr.rect.flag.w = softness;
    rxaddvtx(rx.imp.attr);

    rx.imp.attr.rect.xy    = rxvec2_xy(x1,y0);
    rx.imp.attr.rect.xyxy  = rxvec4_xyzw(center.x,center.y,radius.x,radius.y);
    rx.imp.attr.rect.rgba  = color;
    rx.imp.attr.rect.flag.x = roundness;
    rx.imp.attr.rect.flag.w = softness;
    rxaddvtx(rx.imp.attr);

    rxaddnidx(6, 0,1,2, 0,2,3);
  rxend();
}




ccfunc ccinle void
rxadd_colatex4(
  rxvec2_t xy0, rxvec2_t xy1,
  rxvec2_t uv0, rxvec2_t uv1,
  rxcolor_t c0,rxcolor_t c1,rxcolor_t c2,rxcolor_t c3)
{
  rxbgn();
    rxaddnvtx(4,
      rxvtx_xyuv_col(xy0.x,xy0.y, uv0.x,uv1.y, c0),
      rxvtx_xyuv_col(xy0.x,xy1.y, uv0.x,uv0.y, c1),
      rxvtx_xyuv_col(xy1.x,xy1.y, uv1.x,uv0.y, c2),
      rxvtx_xyuv_col(xy1.x,xy0.y, uv1.x,uv1.y, c3));
    rxaddnidx(6, 0,1,2, 0,2,3);
  rxend();
}

ccfunc ccinle void
rxadd_latex4(
  rxvec2_t xy0, rxvec2_t xy1,
  rxvec2_t uv0, rxvec2_t uv1 )
{
  rxbgn();
    rxaddnvtx(4,
      rxvtx_xyuv(xy0.x,xy0.y, uv0.x,uv1.y),
      rxvtx_xyuv(xy0.x,xy1.y, uv0.x,uv0.y),
      rxvtx_xyuv(xy1.x,xy1.y, uv1.x,uv0.y),
      rxvtx_xyuv(xy1.x,xy0.y, uv1.x,uv1.y));
    rxaddnidx(6, 0,1,2, 0,2,3);
  rxend();
}

ccfunc ccinle void
rxadd_rec4_col(
  rxcolor_t color, float x, float y, float w, float h, float tx, float ty, float tw, float th)
{
  rxadd_colatex4(
    rxvec2_xy( x, y),rxvec2_xy( x+ w, y+ h),
    rxvec2_xy(tx,ty),rxvec2_xy(tx+tw,ty+th), color,color,color,color);
}

void rxdraw_skinned_unnormalized(
  rxtexture_t texture, rxcolor_t color, float x, float y, float w, float h, float tx, float ty, float tw, float th)
{
  rximp_apply();

  rxpipset_texture(REG_PS_TEX_0,texture);
  rxadd_rec4_col(color,x,y,w,h,tx,ty,tw,th);
}

void rxdraw_rect(rxcolor_t color, float x, float y, float w, float h)
{
  rximp_apply();

  rxpipset_sampler(REG_PS_SAM_0,rx.point_sampler);
  rxdraw_skinned_unnormalized(rx.white,color,x,y,w,h,0,0,1,1);
}

// todo!!: this is disgusting!
void rxdraw_circle(rxcolor_t color, float x, float y, float r)
{
  rximp_apply();

  rxpipset_texture(REG_PS_TEX_0,rx.white);

  rxbgn();
  rx.imp.attr.rgba = color;
  for(int i=0; i<360; ++i)
  {
    float ex,ey;
    ex = x;
    ey = y;

    rxaddvtx(rxvtx_xyuv(ex,ey,0,0));

    ex=x + sinf((i - .5f) * (rxPI_F/180)) * r;
    ey=y + cosf((i - .5f) * (rxPI_F/180)) * r;

    rxaddvtx(rxvtx_xyuv(ex,ey,0,1));

    ex=x + sinf((i + .5f) * (rxPI_F/180)) * r;
    ey=y + cosf((i + .5f) * (rxPI_F/180)) * r;

    rxaddvtx(rxvtx_xyuv(ex,ey,1,1));

    rxaddidx(i*3+0); rxaddidx(i*3+1); rxaddidx(i*3+2);
  }
  rxend();
}

ccfunc void
rxdraw_line(
  rxcolor_t color, float thickness, float x0, float y0, float x1, float y1)
{
  float xdist=x1-x0;
  float ydist=y1-y0;
  float length=sqrtf(xdist*xdist + ydist*ydist);

  float xnormal=.5f * thickness * -ydist/length;
  float ynormal=.5f * thickness * +xdist/length;

  rximp_apply();

  rxpipset_texture(REG_PS_TEX_0,rx.white);
  rxbgn();
    rx.imp.attr.rgba = color;
    rxaddnvtx(4,
      rxvtx_xyuv(x0-xnormal,y0-ynormal,0,1),
      rxvtx_xyuv(x0+xnormal,y0+ynormal,0,0),
      rxvtx_xyuv(x1+xnormal,y1+ynormal,1,0),
      rxvtx_xyuv(x1-xnormal,y1-ynormal,1,1));
    rxaddnidx(6, 0,1,2, 0,2,3);
  rxend();
}
/* todo: this has to be re-visited for sub-pixel rendering */
void rxdraw_outline(rxcolor_t color, float x, float y, float w, float h)
{
  rxdraw_rect(color,x-.5,y+h-.5,w+.5,1.);
  rxdraw_rect(color,x-.5,y+0-.5,w+.5,1.);
  rxdraw_rect(color,x+0-.5,y-.5,1.,h+.5);
  rxdraw_rect(color,x+w-.5,y-.5,1.,h+.5);
}

float rxdraw_text_length(float h, const char *string)
{
  float result = ccCstrlenS(string) * rx.font_xsize;
  result *= h/rx.font_ysize;
  return result;
}

// /* remove */
// float rxchrxsz(int ysize)
// {
//   return rx.font_xsize * (ysize / rx.font_ysize);
// }

void
rxdraw_text_run(
  int x/*starting position x*/,
  int y/*starting position y*/,
  int h/* the raster height of the font in pixels*/,
  void *user,
  int (*callback)(/*return whether to continue the run */
    void */*user pointer (could be a plain string)*/,
    int/*the string index*/,
    int */*[out] character to rasterize*/,
    rxcolor_t */*[out] the color*/))
{
  ccassert(h != 0);

  float xnormalize=1.f/rx.font_atlas.size_x;
  float ynormalize=1.f/rx.font_atlas.size_y;
  float render_scale=h/rx.font_ysize;

  float render_x0 = x;
  float render_y0 = y;

  /* XXX this should restore the state */


  /* XXX we have to get back to this */
  float sample_xsize=rx.font_ysize*xnormalize;
  float sample_ysize=rx.font_ysize*ynormalize;
  float render_xsize=rx.font_ysize*render_scale;
  float render_ysize=rx.font_ysize*render_scale;

  float xadvance=rx.font_xsize*render_scale;

  rximp_apply();

  rxpipset_texture(REG_PS_TEX_0,rx.font_atlas   );
  rxpipset_sampler(REG_PS_SAM_0,rx.point_sampler);

  rxcolor_t color;
  int  code;
  int index;
  for( index =0; callback(user,index,&code,&color/* pass in the vertex color directly */);
       index+=1 )
  {
    rxvec2i16_t baked = rx.font_glyph[code-32];

    float sample_x0=baked.x*xnormalize;
    float sample_y0=baked.y*ynormalize;
    float sample_x1=sample_x0+sample_xsize;
    float sample_y1=sample_y0+sample_ysize;
    float render_x1=render_x0+render_xsize;
    float render_y1=render_y0+render_ysize;

    rxbgn();
      rx.imp.attr.rgba = color;
      rxaddvtx(rxvtx_xyuv(render_x0,render_y0,sample_x0,sample_y1));
      rxaddvtx(rxvtx_xyuv(render_x0,render_y1,sample_x0,sample_y0));
      rxaddvtx(rxvtx_xyuv(render_x1,render_y1,sample_x1,sample_y0));
      rxaddvtx(rxvtx_xyuv(render_x1,render_y0,sample_x1,sample_y1));
      rxaddnidx(6, 0,1,2, 0,2,3);
    rxend();

    render_x0 += xadvance;
  }

}

/* todo: re-use draw-text-run instead */
void
rxdraw_text_ex(rxcolor_t color, int x, int y, int h, int length, const char *string)
{
  ccassert(h != 0);

  if(length <= 0)
  {
    return;
  }

  float xnormalize=1.f/rx.font_atlas.size_x;
  float ynormalize=1.f/rx.font_atlas.size_y;
  float render_scale=h/rx.font_ysize;

  float render_x0 = x;
  float render_y0 = y;

  rxpipset_texture(REG_PS_TEX_0,rx.font_atlas);
  rxpipset_sampler(REG_PS_SAM_0,rx.point_sampler);

  rxbgn();

  rx.imp.attr.rgba = color;

  /* XXX we have to get back to this */
  float sample_xsize=rx.font_ysize*xnormalize;
  float sample_ysize=rx.font_ysize*ynormalize;
  float render_xsize=rx.font_ysize*render_scale;
  float render_ysize=rx.font_ysize*render_scale;

  float xadvance=rx.font_xsize*render_scale;

  int index;
  for(index=0; index < length; index += 1)
  {
    int code = *string ++;

    rxvec2i16_t baked=rx.font_glyph[code-32];

    float sample_x0=baked.x*xnormalize;
    float sample_y0=baked.y*ynormalize;
    float sample_x1=sample_x0+sample_xsize;
    float sample_y1=sample_y0+sample_ysize;
    float render_x1=render_x0+render_xsize;
    float render_y1=render_y0+render_ysize;
    rxaddvtx(rxvtx_xyuv(render_x0,render_y0,sample_x0,sample_y1));
    rxaddvtx(rxvtx_xyuv(render_x0,render_y1,sample_x0,sample_y0));
    rxaddvtx(rxvtx_xyuv(render_x1,render_y1,sample_x1,sample_y0));
    rxaddvtx(rxvtx_xyuv(render_x1,render_y0,sample_x1,sample_y1));
    rxaddidx(index*4+0); rxaddidx(index*4+1); rxaddidx(index*4+2);
    rxaddidx(index*4+0); rxaddidx(index*4+2); rxaddidx(index*4+3);

    render_x0+=xadvance;
  }

  rxend();
}

void rxdraw_text(int x, int y, int h, const char *string)
{
  if(string != 0)
  {
    rxdraw_text_ex(RX_COLOR_WHITE,x,y,h,strlen(string),string);
  }
}



#pragma warning(pop)
#endif
