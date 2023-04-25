/*
**
** -+- rx -+-
**
** Copyright(C) Dayan Rodriguez, 2022, All rights reserved.
**
** Simple render API for when you just want to see something.
**
** 'rx' is not production ready nor production quality.
**
** libraries that you should use instead:
** erkkah/tigr or raysan5/raylib
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
// todo: stop using QueryInterface as it increments the reference count?
// todo: caching the shadow maps, at different resolutions?
// todo: different light types, directional, glow effects....
// todo: bind different textures to the shader to avoid having to switch them multiple times per frame?
//
// lastest branch: march 30, 23: rx-iq
// lastest branch: april 10, 23: rx-mz
// lastest branch: april 15, 23: rx-iq
// lastest branch: april 19, 23: master
//
#ifndef _RX_H
#define _RX_H
#define _CRT_SECURE_NO_WARNINGS

#define           CINTERFACE
#define           COBJMACROS
#define     D3D11_NO_HELPERS
#define             NOMINMAX
#define  WIN32_LEAN_AND_MEAN
#define _NO_CRT_STDIO_INLINE
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
#endif//_RX_STANDALONE

#include  "rxps.hlsl"
#include  "rxvs.hlsl"

// note: this is partially provisional, until we figure what
// format we want to use for function arguments and whether
// we want or not to have different functions for different
// formats!
# ifdef _RX_NO_INT_TO_FLOAT_WARNING
#pragma warning(disable:4244)
# endif

# ifdef _RX_NO_DOUBLE_TO_FLOAT_WARNING
#pragma warning(disable:4305)
# endif

// todo: this is provisional
#ifndef RX_COMMAND_BUFFER_SIZE
#define RX_COMMAND_BUFFER_SIZE 0x1000
# endif
// note: use these as your pre-init parameters you can always change later!
#ifndef RX_INDEX_BUFFER_SIZE
#define RX_INDEX_BUFFER_SIZE 0x10000
# endif
#ifndef RX_VERTEX_BUFFER_SIZE
#define RX_VERTEX_BUFFER_SIZE 0x10000
#endif
// note: off by default!
// #ifndef RX_ENABLE_DEPTH_BUFFER
// # define RX_ENABLE_DEPTH_BUFFER
// #endif

#define rxPI_F 3.14159265358979323846f
#define rxABS(v)   ((v)<(0)?-(v):(v))
#define rxMIN(x,y) ((x)<(y)? (x):(y))
#define rxMAX(x,y) ((x)>(y)? (x):(y))

// merge: added 'rxclamp'
float rxclamp(float val, float min, float max)
{
  return val < min ? min : val > max ? max : val;
}

// todo?: there should be a macro for row-vs-col matrices!
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
#endif

// todo: to be removed!
#define     rxRGB8 DXGI_FORMAT_R8_UNORM
#define rxRGBA8888 DXGI_FORMAT_R8G8B8A8_UNORM


typedef enum rx_k
{
         rx_kINVALID,

  rx_kCOMPUTE_SHADER,
    rx_kPIXEL_SHADER,
   rx_kVERTEX_SHADER,

    rx_kINPUT_LAYOUT,

  rx_kUNIFORM_BUFFER,
   rx_kSTRUCT_BUFFER,
   rx_kVERTEX_BUFFER,
    rx_kINDEX_BUFFER,

  rx_kSAMPLER,
  rx_kTEXTURE,

  rx_kRENDER_TARGET,

  rx_kCUSTOM,
  rx_kCLIP,

  // note: push a user provided custom target
  rx_kPUSHTARGET,
  rx_kPULLTARGET,

  // note: push a user provided custom shader
  rx_kPUSHSHADER,
  rx_kPULLSHADER,

  // todo:
  rx_kENDPASS,

  rx_kCLEAR,

  rx_kUNIFORM,

  rx_kMATRIX,
  rx_kMATRIX_MULTIPLY,

  rx_kINDEXED,

  // note: window events!
  rx_kLBUTTON,
  rx_kRBUTTON,
} rx_k;

typedef struct rxcolor_t rxcolor_t;
typedef struct rxcolor_t
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} rxcolor_t;

const rxcolor_t  rxcolor_kWHITE={0xff,0xff,0xff,0xff};
const rxcolor_t   rxcolor_kGRAY={0x80,0x80,0x80,0xff};
const rxcolor_t  rxcolor_kBLACK={0x00,0x00,0x00,0xff};
const rxcolor_t    rxcolor_kRED={0xff,0x00,0x00,0xff};
const rxcolor_t  rxcolor_kGREEN={0x00,0xff,0x00,0xff};
const rxcolor_t rxcolor_kYELLOW={0xff,0xff,0x00,0xff};
const rxcolor_t   rxcolor_kBLUE={0x00,0x00,0xff,0xff};

typedef struct rxpoint16_t rxpoint16_t;
typedef struct rxpoint16_t
{
  short x,y;
} rxpoint16_t;
typedef struct rxvector2_t rxvector2_t;
typedef struct rxvector2_t
{
  float x,y;
} rxvector2_t;
typedef struct rxvector3_t rxvector3_t;
typedef struct rxvector3_t
{
  float x,y,z;
} rxvector3_t;
typedef struct rxvector4_t rxvector4_t;
typedef struct rxvector4_t
{ union
  { struct
    {
      float x,y,z,w;
    };
    struct
    {
      float r,g,b,a;
    };
    rxvector3_t xyz;
    rxvector3_t rgb;
  };
} rxvector4_t;
rxvector3_t       rxvector(float xyz);
rxvector3_t   rxvector_xyz(float x, float y, float z);
rxvector3_t    rxvector_xy(float x, float y);
rxvector3_t     rxvector_x(float x);
rxvector3_t     rxvector_y(float y);
rxvector3_t     rxvector_z(float z);
typedef struct rxmatrix_t rxmatrix_t;
typedef struct rxmatrix_t
{ float m[4][4];
} rxmatrix_t;
rxmatrix_t rxmatrix_identity();
rxmatrix_t rxmatrix_multiply(rxmatrix_t, rxmatrix_t);

typedef ID3D11DeviceChild   *  rxunknown_t;
typedef ID3D11Resource      * rxresource_t;

// note:
// One if the features that I always want to have out of the box is live-reload, especially when
// working with shaders. The way we do this is by keeping track of every file we load and whether its
// contents have changed or not, this also means that live-reload works for anything that is bound to
// a name on disk implicitly when using the default 'rxload-contents' function.
// todo!!: we should instead hash the file's contents!
typedef struct rxcontents_t rxcontents_t;
typedef struct rxcontents_t
{ unsigned  int  length;
  void         * memory;
  ccclocktick_t  loaded;
  // note: we keep track of whether the contents of this file
  // are erroneous, in which case all instances bound to this
  // file shouldn't be reloaded/restored.
  unsigned          is_erroneous: 1;
} rxcontents_t;

// note: the parameters we need to restore the instance,
// could probably have come up with a better name.
typedef struct rxrestore_t rxrestore_t;
typedef struct rxrestore_t
{
  const    char *shader_model;
  const    char *shader_entry;
} rxrestore_t;

typedef struct rxinstance_t rxinstance_t;
typedef struct rxinstance_t
{ rx_k                  sort;
  rxunknown_t         unknown;
  const    char   *    master;
  ccclocktick_t        loaded;
  rxrestore_t         restore;
} rxinstance_t;

typedef struct rxblobber_t rxblobber_t;
typedef struct rxblobber_t
{
  ID3DBlob    *unknown;

  void    *memory;
  size_t   length;
} rxblobber_t;

// note: if vertex shader, input layout stored in private data, we could
// cache the additional pointer in this struct if too slow!
// note: to get which type of shader it is we simply query the interface,
// this behavior is to be removed!
typedef struct rxshader_t rxshader_t;
typedef struct rxshader_t
{
  rxunknown_t unknown;
} rxshader_t;

typedef struct rxuniform_buffer_t rxuniform_buffer_t;
typedef struct rxuniform_buffer_t
{
  rxunknown_t unknown;
} rxuniform_buffer_t;

typedef struct rxvertex_buffer_t rxvertex_buffer_t;
typedef struct rxvertex_buffer_t
{
  rxunknown_t unknown;
} rxvertex_buffer_t;

typedef struct rxindex_buffer_t rxindex_buffer_t;
typedef struct rxindex_buffer_t
{
  rxunknown_t unknown;
} rxindex_buffer_t;

typedef struct rxstruct_buffer_t rxstruct_buffer_t;
typedef struct rxstruct_buffer_t
{
  rxunknown_t unknown;
} rxstruct_buffer_t;

typedef struct rxsampler_t rxsampler_t;
typedef struct rxsampler_t
{
  ID3D11SamplerState *unknown;
} rxsampler_t;

// todo: need better name?
typedef struct rxlocal_texture_t rxlocal_texture_t;
typedef struct rxlocal_texture_t
{  int   size_x, size_y;
   int   format;
   int   stride;
  void * memory;
} rxlocal_texture_t;

typedef struct rxtexture_t rxtexture_t;
typedef struct rxtexture_t
{         int size_x, size_y;
          int format;
  rxunknown_t unknown;
} rxtexture_t;

typedef struct rxrender_target_t rxrender_target_t;
typedef struct rxrender_target_t
{         int size_x, size_y;
          int format;
  rxunknown_t unknown;
  rxunknown_t asinput;
} rxrender_target_t;

// note: is this is good name?
typedef struct rxborrowed_t rxborrowed_t;
typedef struct rxborrowed_t
{ rxresource_t  resource;
         void * memory;
 union
 {        int   stride;
          int   length;
 };
} rxborrowed_t;

// todo: migrate to xyz
typedef struct rxvertex_t rxvertex_t;
typedef struct rxvertex_t
{ float x, y;
  float u, v;
  unsigned char r,g,b,a;
} rxvertex_t;

// sharpness: determines the harshness or sharpness of the penumbras, the greater the sharper,
// the smaller the softer.
typedef struct rxcandle_t rxcandle_t;
typedef struct rxcandle_t
{ rxvector3_t           xyz;
  rxvector3_t     direction;
  rxvector3_t         color;
  float           intensity;
  float           sharpness;
  float           threshold;
  float      radial_falloff;
  float directional_falloff;
} rxcandle_t;

typedef struct rxshadow_t rxshadow_t;
typedef struct rxshadow_t
{ float x, y;
  float w, h;
} rxshadow_t;

// note: we have some extra 'useless' info here for quick prototyping or for demoscene type stuff,
// this is what shader-toy does!
// merge: switched to using floating point values instead!
typedef struct rxuniform_t rxuniform_t;
typedef struct __declspec(align(16)) rxuniform_t
{
  // todo!!!: rename
  rxmatrix_t e;

  float    screen_xsize;
  float    screen_ysize;
  float   mouse_xcursor;
  float   mouse_ycursor;
  double  total_seconds;
  double  delta_seconds;
  int      shadow_tally;
  int      candle_tally;
  int           padding[2];
} rxuniform_t;

typedef int rxindex_t;

typedef void (*rxcustom_t)();

// todo!!: union
typedef struct rxcommand_t rxcommand_t;
typedef struct rxcommand_t
{              rx_k   kind;
         const char * name;
  rxrender_target_t   target;
         rxshader_t   shader;
        rxsampler_t   sampler;
        rxtexture_t   texture;
         rxcustom_t   custom;
         rxmatrix_t   matrix;
                int   offset;
                int   length;
                int   x,y,w,h;
      unsigned char   r,g,b,a;
                int   slot;
} rxcommand_t;

// typedef struct rxvertex_renderer_t rxvertex_renderer_t;
// typedef struct rxvertex_renderer_t
// {
// } rxvertex_renderer_t;

// todo!!: please forgive me!
typedef struct rx_t rx_t;
typedef struct rx_t
{
  // note:
  HANDLE LiveReloadDirectory;
  HANDLE LiveReloadEvent;

  // note:
            int   tick_count;
  ccclocktick_t   start_ticks;
  ccclocktick_t   frame_ticks;
  ccclocktick_t   total_ticks;
  ccclocktick_t   delta_ticks;

  // todo!!: do not make Tom Forsyth mad!
  double          total_seconds;
  double          delta_seconds;

  rxtexture_t     font_atlas;
  rxpoint16_t     font_glyph[95];
  float           font_ysize;
  float           font_xsize;

  // note: these are cc-hash-tables
  rxinstance_t       *instance_table;
  rxcontents_t       *contents_table;


  rxmatrix_t                        view_matrix;
  rxmatrix_t                       world_matrix;

  rxuniform_buffer_t             uniform_buffer;
   rxvertex_buffer_t              vertex_buffer;
    rxindex_buffer_t               index_buffer;

  rxborrowed_t                    vertex_buffer_writeonly;
    rxvertex_t                    *vertex_array;
           int                     vertex_tally;

  rxborrowed_t                      index_buffer_writeonly;
     rxindex_t                     *index_array;
           int                      index_tally;

   rxcommand_t                    command_array[RX_COMMAND_BUFFER_SIZE];
           int                    command_tally;

  rxcommand_t  *                        command;
   const char  *                   command_name;
          int                     vertex_modeon;
        float                     vertex_ytexel;
        float                     vertex_xtexel;
    rxcolor_t                     vertex_color;

  rxrender_target_t               screen_target;
  rxrender_target_t            offscreen_target;
  rxrender_target_t               effect_target;

  rxshader_t            offscreen_vertex_shader;
  rxshader_t             offscreen_pixel_shader;

  rxshader_t               effect_vertex_shader;
  rxshader_t                effect_pixel_shader;

           int                 shadow_threshold;
           int                     shadow_tally;
  rxborrowed_t          shadow_buffer_writeonly;
  rxstruct_buffer_t               shadow_buffer;
    rxshadow_t                   * shadow_array;

           int                 candle_threshold;
           int                     candle_tally;
  rxborrowed_t          candle_buffer_writeonly;
  rxstruct_buffer_t               candle_buffer;
    rxcandle_t                   * candle_array;

  rxrender_target_t                      target;



  rxrender_target_t                target_stack[4];
                int                target_index;

  rxshader_t                       shader_stack[4];
         int                       shader_index;

         rxshader_t               vertex_shader;
         rxshader_t                pixel_shader;
         rxshader_t              compute_shader;
         rxshader_t                      shader;

  unsigned  Quitted:      1;
  unsigned  Visible:      1;
  unsigned  Resizable:    1;
  unsigned  Decorated:    1;
  unsigned  Floating:     1;
  unsigned  FocusOnShow:  1;
  unsigned  EventHolder:  1;
  unsigned  ClickFocused: 1;
  HWND      Window;
  int       size_x;
  int       size_y;
  int       center_x;
  int       center_y;

  // todo!!: remove!
  float clear_r;
  float clear_g;
  float clear_b;
  float clear_a;

  // todo:
  int window_events[0x100];

  int xcursor;
  int ycursor;

  void                 *  ControlFiber;
  void                 *  MessageFiber;

  ID3D11InfoQueue         *InfoQueue;
  ID3D11Device            *Device;
  ID3D11DeviceContext     *Context;

  IDXGISwapChain2         *SwapChain;
  void                    *FrameAwait;

  ID3D11DepthStencilState  *DepthBufferState;
  ID3D11Texture2D          *DepthBufferStore;
  ID3D11DepthStencilView   *DepthBuffer;
  ID3D11RasterizerState    *RasterizerState;
  ID3D11BlendState         *BlendState;

  rxshader_t default_vertex_shader;
  rxshader_t  default_pixel_shader;

  rxsampler_t        linear_sampler;
  rxsampler_t         point_sampler;

  rxtexture_t    white;
} rx_t;

ccglobal rx_t rx;

void rxdelete_unknown(rxunknown_t unknown)
{
  if(unknown != 0)
    unknown->lpVtbl->Release(unknown);
}

void rxdelete_instance(rxunknown_t unknown)
{
  ccassert(unknown != 0);

  rxinstance_t *i=cctblremP(rx.instance_table,unknown);
  ccassert(ccerrnon());

  ccassert(i->unknown != NULL);

  ID3D11DeviceChild_Release(i->unknown);

  i->unknown = NULL;
}

rxinstance_t *rxlookup_instance(rxunknown_t u)
{
  rxinstance_t *i = 0;

  if(u != 0)
  {
    i=cctblgetP(rx.instance_table,u);
    ccassert(ccerrnon());
  }

  return i;
}

rxinstance_t *rxcreate_instance(rxinstance_t *i, rx_k sort, rxunknown_t unknown)
{
  // ccdebuglog("%i, %p", sort, unknown);

  if(!i)
  {
    i=cctblputP(rx.instance_table,unknown);
    ccassert(ccerrnon());

  } else
    ccassert(i->sort == sort);

  i->      sort=sort;
  i->   unknown=unknown;
  return i;
}

void rxreturn_resource(void *resource)
{
  ID3D11DeviceContext_Unmap(rx.Context,(ID3D11Resource*)resource,0);
}

void rxreturn(rxborrowed_t borrowed)
{
  rxreturn_resource(borrowed.resource);
}

// todo: ensure we're doing this 'safely'!
void rxdelete_vertex_buffer(rxvertex_buffer_t buffer)
{
  rxdelete_unknown(buffer.unknown);
}

// todo: ensure we're doing this 'safely'!
void rxdelete_index_buffer(rxindex_buffer_t buffer)
{
  rxdelete_unknown(buffer.unknown);
}

// todo: ensure we're doing this 'safely'!
void rxdelete_uniform_buffer(rxuniform_buffer_t buffer)
{
  rxdelete_unknown(buffer.unknown);
}

// todo: ensure we're doing this 'safely'!
void rxdelete_texture(rxtexture_t texture)
{
  // todo: ensure this is a valid view
  ID3D11Resource *Resource;
  ID3D11View_GetResource((ID3D11View*)texture.unknown,&Resource);

  ID3D11Resource_Release(Resource);
}

int rxcontents_registered(const char *name)
{
  cctblgetS(rx.contents_table,name);

  return ccerrnon();
}

rxcontents_t *rxreload_contents(const char *name)
{
  ccassert(name    != 0);
  ccassert(name[0] != 0);

  rxcontents_t *contents=cctblsetS(rx.contents_table,name);
  ccassert(ccerrnon());

  void *file=ccopenfile(name,"r");

  unsigned long length=0;
  void *memory=ccpullfile(file,0,&length);

  ccclosefile(file);

  if(length != 0 && memory != 0)
  {
    if((contents->length == 0) || (contents->memory == 0) ||
        (contents->length != length) || memcmp(memory,contents->memory,length))
    {
      ccdebuglog("'%s': file contents reloaded",name);

      contents->      loaded=rx.total_ticks;
      contents->is_erroneous=ccfalse;
    }

    if(contents->memory)
    {
      ccfree(contents->memory);
    }

    contents->memory=memory;
    contents->length=length;
  } else
    cctracewar("'%s': file contents could not be read",name);

  return contents;
}

// todo!:
int rxunknown_typeof_compute_shader(rxunknown_t unknown)
{
  ID3D11ComputeShader *I=NULL;
  if(SUCCEEDED(IUnknown_QueryInterface(unknown,&IID_ID3D11ComputeShader,&I)))
    ID3D11DeviceChild_Release(I);
  return I!=NULL;
}

// todo!:
int rxunknown_typeof_vertex_shader(rxunknown_t unknown)
{
  ID3D11VertexShader *I=NULL;
  if(SUCCEEDED(IUnknown_QueryInterface(unknown,&IID_ID3D11VertexShader,&I)))
    ID3D11DeviceChild_Release(I);
  return I!=NULL;
}

// todo!:
int rxunknown_typeof_pixel_shader(rxunknown_t unknown)
{
  ID3D11PixelShader *I=NULL;
  if(SUCCEEDED(IUnknown_QueryInterface(unknown,&IID_ID3D11PixelShader,&I)))
    ID3D11DeviceChild_Release(I);
  return I!=NULL;
}

int rxshader_typeof_compute(rxshader_t shader)
{
  return rxunknown_typeof_compute_shader(shader.unknown);
}

int rxshader_typeof_vertex(rxshader_t shader)
{
  return rxunknown_typeof_vertex_shader(shader.unknown);
}

int rxshader_typeof_pixel(rxshader_t shader)
{
  return rxunknown_typeof_pixel_shader(shader.unknown);
}

// note: possibly temporary
void rxdriver_clear_render_target(rxrender_target_t target, float *color)
{ ID3D11RenderTargetView *RenderTarget;
  IUnknown_QueryInterface(target.unknown,&IID_ID3D11RenderTargetView,&RenderTarget);
  ID3D11DeviceContext_ClearRenderTargetView(rx.Context,RenderTarget,color);
}

// note: possibly temporary
void rxdriver_stage_render_target(rxrender_target_t target)
{ ID3D11RenderTargetView *RenderTarget;
  IUnknown_QueryInterface(target.unknown,&IID_ID3D11RenderTargetView,&RenderTarget);
  ID3D11DeviceContext_OMSetRenderTargets(rx.Context,1,&RenderTarget,NULL);
}

// note: possibly temporary
void rxdriver_stage_viewport(float w, float h)
{ D3D11_VIEWPORT Viewport;
  Viewport.TopLeftX=0;
  Viewport.TopLeftY=0;
  Viewport.   Width=w;
  Viewport.  Height=h;
  Viewport.MinDepth=0;
  Viewport.MaxDepth=1;
  ID3D11DeviceContext_RSSetViewports(rx.Context,1,&Viewport);
}

// todo!!: to be removed? stored separately?
ID3D11InputLayout *rxshader_query_input_layout_d3d(rxunknown_t shader)
{ ID3D11InputLayout *InputLayout=NULL;
  UINT PrivateDataSize=sizeof(InputLayout);
  ID3D11DeviceChild_GetPrivateData(shader,&IID_ID3D11InputLayout,&PrivateDataSize,&InputLayout);
  ccassert(InputLayout != 0 && InputLayout->lpVtbl != 0);
  return InputLayout;
}

void rxshader_store_input_layout_d3d(rxunknown_t shader, ID3D11InputLayout *InputLayout)
{ ccassert(InputLayout != 0 && InputLayout->lpVtbl != 0);
  UINT PrivateDataSize=sizeof(InputLayout);
  ID3D11DeviceChild_SetPrivateData(shader,&IID_ID3D11InputLayout,PrivateDataSize,&InputLayout);
  // rxshader_query_input_layout_d3d(shader);
}

void rxdelete_shader(rxunknown_t unknown)
{
  if(rxunknown_typeof_vertex_shader(unknown))
  {
    ID3D11InputLayout *InputLayout=rxshader_query_input_layout_d3d(unknown);

    if(InputLayout != NULL)
    {
      rxdelete_instance((rxunknown_t)InputLayout);
      // IUnknown_Release(InputLayout);
    }

  }

  // todo!!:
  rxdelete_unknown(unknown);
}


rxshader_t rxload_vertex_shader(rxinstance_t *,const char *, const char *);
rxshader_t  rxload_pixel_shader(rxinstance_t *,const char *, const char *);

rxblobber_t rxcompile_shader_file(const char *name, const char *main, const char *model);

void rxdriver_stage_shader(rxshader_t shader)
{
  rxinstance_t *i=rxlookup_instance(shader.unknown);

  int should_reload=ccfalse;

  // if(rx.live_reload)
  if(i->master != 0)
  {
    rxcontents_t *c=cctblgetS(rx.contents_table,i->master);

    should_reload= ccerrnit() || (!c->is_erroneous && c->loaded > i->loaded);
  }

  // todo!!:
  if(rxunknown_typeof_vertex_shader(i->unknown) &&
      (i->unknown != rx.vertex_shader.unknown || should_reload))
  {
    if(should_reload)
    {
      rxunknown_t u=i->unknown;
      rxshader_t n=rxload_vertex_shader(i,NULL,NULL);

      if(n.unknown != NULL)
      {
        rxdelete_shader(u);

        ccassert(i->unknown != 0);
      }
    }

    rx.shader=rx.vertex_shader=shader;

    ID3D11DeviceContext_VSSetShader(rx.Context,(ID3D11VertexShader*)i->unknown,0x00,0);
    ID3D11DeviceContext_IASetInputLayout(rx.Context,rxshader_query_input_layout_d3d(i->unknown));
    goto leave;
  }

  if(rxunknown_typeof_pixel_shader(i->unknown) &&
      (i->unknown != rx.pixel_shader.unknown || should_reload))
  {
    if(should_reload)
    {
      rxunknown_t u=i->unknown;
      rxshader_t n=rxload_pixel_shader(i,NULL,NULL);

      if(n.unknown != NULL)
      {
        rxdelete_shader(u);

        ccassert(i->unknown != 0);
      }
    }

    rx.shader=rx.pixel_shader=shader;

    ID3D11DeviceContext_PSSetShader(rx.Context,(ID3D11PixelShader*)i->unknown,0x00,0);
    goto leave;
  }

leave:;

}

void rxdriver_stage_sampler(rxsampler_t sampler, int slot)
{
  ccassert(sampler.unknown != 0);

  // todo!!:
  if(rxshader_typeof_vertex(rx.shader))
  {
    ID3D11DeviceContext_VSSetSamplers(rx.Context,slot,1,(ID3D11SamplerState**)&sampler.unknown);
    goto leave;
  }

  if(rxshader_typeof_pixel(rx.shader))
  {
    ID3D11DeviceContext_PSSetSamplers(rx.Context,slot,1,(ID3D11SamplerState**)&sampler.unknown);
    goto leave;
  }

  if(rxshader_typeof_compute(rx.shader))
  {
    ID3D11DeviceContext_CSSetSamplers(rx.Context,slot,1,(ID3D11SamplerState**)&sampler.unknown);
    goto leave;
  }

leave:;
}

void rxdriver_stage_texture(rxtexture_t texture, int slot)
{
  ccassert(texture.unknown != 0);

  // todo!!: this should only be done in debug mode
  ID3D11ShaderResourceView *View;
  if(SUCCEEDED(
      IUnknown_QueryInterface(texture.unknown,&IID_ID3D11ShaderResourceView,&View)))
  {
    // todo!!:
    if(rxshader_typeof_vertex(rx.shader))
    {
      ID3D11DeviceContext_VSSetShaderResources(rx.Context,slot,1,&View);
      goto leave;
    }

    if(rxshader_typeof_pixel(rx.shader))
    {
      ID3D11DeviceContext_PSSetShaderResources(rx.Context,slot,1,&View);
      goto leave;
    }

    if(rxshader_typeof_compute(rx.shader))
    {
      ID3D11DeviceContext_CSSetShaderResources(rx.Context,slot,1,&View);
      goto leave;
    }
  }

leave:;
}


void rxcommand_label(const char *name)
{
  ccassert(rx.command_name == 0);

  rx.command_name=name;
}

rxcommand_t *rxdraw_command(rx_k kind)
{
  if(rx.command_tally >= RX_COMMAND_BUFFER_SIZE)
  {
    cctracewar("'%i': command limit reached", rx.command_tally);

    rx.command_tally = RX_COMMAND_BUFFER_SIZE - 1;
  }

  rxcommand_t *command=rx.command_array+rx.command_tally++;
  command->kind=kind;
  command->name=rx.command_name;

  rx.command_name=ccnull;
  return command;
}

void rxdraw_end()
{
  rxdraw_command(rx_kENDPASS);
}

void rxqueue_custom_command(rxcustom_t custom)
{
  rxcommand_t *draw=rxdraw_command(rx_kCUSTOM);
  draw->custom=custom;
}

// todo: properly integrate this!
void rxdraw_matrix(rxmatrix_t matrix)
{
  rxcommand_t *draw=rxdraw_command(rx_kMATRIX);
  draw->matrix=matrix;
}

// todo: properly integrate this!
void rxdraw_matrix_multiply(rxmatrix_t matrix)
{
  rxcommand_t *draw=rxdraw_command(rx_kMATRIX_MULTIPLY);
  draw->matrix=matrix;
}

// todo: this should be a command instead, and it should use 'rxcolor_t'
void rxclear(float r, float g, float b, float a)
{
  rx.clear_r=r;
  rx.clear_g=g;
  rx.clear_b=b;
  rx.clear_a=a;
}

void rxqueue_sampler_command_ex(rxsampler_t sampler, int slot)
{
  rxcommand_t *draw=rxdraw_command(rx_kSAMPLER);
  draw->sampler=sampler;
  draw->slot=slot;
}

void rxqueue_sampler_command(rxsampler_t sampler)
{
  rxqueue_sampler_command_ex(sampler,0);
}

void rxqueue_texture_command_ex(rxtexture_t texture, int slot)
{
  rxcommand_t *draw=rxdraw_command(rx_kTEXTURE);
  draw->texture=texture;
  draw->slot=slot;
}

void rxqueue_texture_command(rxtexture_t texture)
{
  rxqueue_texture_command_ex(texture,0);
}

void rxqueue_shader_command(rxshader_t shader)
{
  rxcommand_t *draw=rxdraw_command(rx_kPUSHSHADER);
  draw->shader=shader;
}

void rxqueue_uniform_command(rxuniform_buffer_t buffer)
{
  rxcommand_t *draw=rxdraw_command(rx_kUNIFORM);
  // draw->unknown=buffer.unknown;
}

void rxvertex_mode_end()
{
  ccassert(rx.vertex_modeon != 0);

  rx.vertex_modeon=ccfalse;
  rx.command=ccnull;
}

void rxvertex_mode()
{ ccassert(!rx.vertex_modeon);

  rx.vertex_ytexel=0;
  rx.vertex_xtexel=0;
  rx.vertex_color=rxcolor_kBLACK;
  rx.command=rxdraw_command(rx_kINDEXED);
  rx.command->offset=rx.vertex_tally;
  rx.command->length=0;
  rx.vertex_modeon=cctrue;
}

void rxsubmit_index(rxindex_t value)
{
  ccassert(rx.vertex_modeon != 0);
  ccassert(rx.command != 0);

  rxindex_t *index=rx.index_array+rx.index_tally++;
  *index=value;

  rx.command->length++;
}

void rxvertex_color_rgba(int color_r, int color_g, int color_b, int color_a)
{
  rx.vertex_color.r=(unsigned char)color_r;
  rx.vertex_color.g=(unsigned char)color_g;
  rx.vertex_color.b=(unsigned char)color_b;
  rx.vertex_color.a=(unsigned char)color_a;
}

void rxvertex_color(rxcolor_t color)
{
  rx.vertex_color=color;
}

void rxsubmit_vertex_ex(
  float x, float y, float xcoord, float ycoord, rxcolor_t color)
{
  ccassert(rx.vertex_modeon != 0);
  ccassert(rx.command != 0);

  rxvertex_t vertex;
  vertex.x=x;
  vertex.y=y;
  vertex.u=xcoord;
  vertex.v=ycoord;
  vertex.r=color.r;
  vertex.g=color.g;
  vertex.b=color.b;
  vertex.a=color.a;

  rx.vertex_array[rx.vertex_tally++]=vertex;
}

void rxsubmit_vertex_xy(float x, float y)
{
  rxsubmit_vertex_ex(x,y,rx.vertex_xtexel,rx.vertex_ytexel,rx.vertex_color);
}

void rxsubmit_vertex_tex(float x, float y, float xcoord, float ycoord)
{
  rxsubmit_vertex_ex(x,y,xcoord,ycoord,rx.vertex_color);
}

// todo: actually check that the user set a texture!
// todo: this has a major flaw, we can't render rotated laterals by passing in the width and height,
// forcing us to use transforms to render lines!
void rxdraw_skinned_preset_unnormalized(
  rxcolor_t color, float x, float y, float w, float h, float tx, float ty, float tw, float th)
{
  rxvertex_mode();
    rxvertex_color(color);
    rxsubmit_vertex_tex(x+0,y+0,tx   ,ty+th);
    rxsubmit_vertex_tex(x+0,y+h,tx   ,ty   );
    rxsubmit_vertex_tex(x+w,y+h,tx+tw,ty   );
    rxsubmit_vertex_tex(x+w,y+0,tx+tw,ty+th);
    rxsubmit_index(0); rxsubmit_index(1); rxsubmit_index(2);
    rxsubmit_index(0); rxsubmit_index(2); rxsubmit_index(3);
  rxvertex_mode_end();
}

void rxdraw_skinned_unnormalized(
  rxtexture_t texture, rxcolor_t color, float x, float y, float w, float h, float tx, float ty, float tw, float th)
{
  // note: consume our label and propagate to other commands!
  const char *label=rx.command_name;
  rx.command_name=0;

  rxcommand_label(label);
  rxqueue_texture_command(texture);

  rxcommand_label(label);
  rxdraw_skinned_preset_unnormalized(color,x,y,w,h,tx,ty,tw,th);
}

void rxdraw_skinned_ex(
  rxtexture_t texture, rxcolor_t color, float x, float y, float w, float h, float tx, float ty, float tw, float th)
{
  int size_x=texture.size_x;
  int size_y=texture.size_y;
  rxdraw_skinned_unnormalized(texture,color,x,y,w,h,tx/(float)size_x,ty/(float)size_y,tw/(float)size_x,th/(float)size_y);
}

void rxdraw_skinned(rxtexture_t texture, float x, float y, float w, float h)
{
  rxdraw_skinned_unnormalized(texture,rxcolor_kWHITE,x,y,w,h,0,0,1,1);
}

void rxdraw_rect(rxcolor_t color, float x, float y, float w, float h)
{
  rxqueue_sampler_command(rx.point_sampler);
  rxdraw_skinned_unnormalized(rx.white,color,x,y,w,h,0,0,1,1);
}


// todo!!: this is disgusting!
void rxdraw_circle(rxcolor_t color, float x, float y, float r)
{
  rxqueue_texture_command(rx.white);

  rxvertex_mode();
  { rxvertex_color(color);

    for(int i=0; i<360; ++i)
    {
      float ex,ey;
      ex = x;
      ey = y;

      rxsubmit_vertex_tex(ex,ey,0,0);

      ex=x + sinf((i - .5f) * (rxPI_F/180)) * r;
      ey=y + cosf((i - .5f) * (rxPI_F/180)) * r;

      rxsubmit_vertex_tex(ex,ey,0,1);

      ex=x + sinf((i + .5f) * (rxPI_F/180)) * r;
      ey=y + cosf((i + .5f) * (rxPI_F/180)) * r;

      rxsubmit_vertex_tex(ex,ey,1,1);

      rxsubmit_index(i*3+0); rxsubmit_index(i*3+1); rxsubmit_index(i*3+2);
    }
  }
  rxvertex_mode_end();
}

// merge: ++ 'rxdraw_line'
void rxdraw_line(rxcolor_t color, float thickness, float x0, float y0, float x1, float y1)
{
  float xdirection=x1-x0;
  float ydirection=y1-y0;
  float length=sqrtf(xdirection*xdirection + ydirection*ydirection);

  float xnormal=.5f * thickness * -ydirection/length;
  float ynormal=.5f * thickness * +xdirection/length;

  rxqueue_texture_command(rx.white);
  rxvertex_mode();
    rxvertex_color(color);
    rxsubmit_vertex_tex(x0-xnormal,y0-ynormal,0,1);
    rxsubmit_vertex_tex(x0+xnormal,y0+ynormal,0,0);
    rxsubmit_vertex_tex(x1+xnormal,y1+ynormal,1,0);
    rxsubmit_vertex_tex(x1-xnormal,y1-ynormal,1,1);
    rxsubmit_index(0); rxsubmit_index(1); rxsubmit_index(2);
    rxsubmit_index(0); rxsubmit_index(2); rxsubmit_index(3);
  rxvertex_mode_end();
}

void rxdraw_outline(rxcolor_t color, float x, float y, float w, float h)
{
  const char *label=rx.command_name;
  rx.command_name=ccnull;

  int b=2;

  rxcommand_label(label);
  rxdraw_rect(color,(float)(x-b),(float)(y+h),(float)(w+b*2),(float)(b));
  rxcommand_label(label);
  rxdraw_rect(color,(float)(x-b),(float)(y-b),(float)(w+b*2),(float)(b));
  rxcommand_label(label);
  rxdraw_rect(color,(float)(x-b),(float)(y+0),(float)(b),(float)(h+0));
  rxcommand_label(label);
  rxdraw_rect(color,(float)(x+w),(float)(y+0),(float)(b),(float)(h+0));
}

float rxdraw_text_length(float h, const char *string)
{
  float result = ccCstrlenS(string) * rx.font_xsize;
  result *= h/rx.font_ysize;
  return result;
}

void rxdraw_text_ex(rxcolor_t color, int x, int y, int h, const char *string)
{
  float xnormalize=1.f/rx.font_atlas.size_x;
  float ynormalize=1.f/rx.font_atlas.size_y;
  float render_scale=h/rx.font_ysize;

  float ox=(float)x;
  float oy=(float)y;

  // todo: this should restore the state!
  rxqueue_texture_command(rx.font_atlas);
  rxqueue_sampler_command(rx.point_sampler);

  rxvertex_mode();
  rxvertex_color(color);

  // todo!!: this actually something we have to get back to
  float sample_xsize=rx.font_ysize*xnormalize;
  float sample_ysize=rx.font_ysize*ynormalize;

  float render_xsize=rx.font_ysize*render_scale;
  float render_ysize=rx.font_ysize*render_scale;

  float xadvance=rx.font_xsize*render_scale;

  for(int index=0; *string; ++index)
  {
    int code=*string++;
    rxpoint16_t baked=rx.font_glyph[code-32];

    float sample_x0=xnormalize*baked.x;
    float sample_y0=ynormalize*baked.y;
    float sample_x1=sample_x0+sample_xsize;
    float sample_y1=sample_y0+sample_ysize;

    float render_x0=ox;
    float render_y0=oy;
    float render_x1=render_x0+render_xsize;
    float render_y1=render_y0+render_ysize;

    rxsubmit_vertex_tex(render_x0,render_y0,sample_x0,sample_y1);
    rxsubmit_vertex_tex(render_x0,render_y1,sample_x0,sample_y0);
    rxsubmit_vertex_tex(render_x1,render_y1,sample_x1,sample_y0);
    rxsubmit_vertex_tex(render_x1,render_y0,sample_x1,sample_y1);

    rxsubmit_index(index*4+0); rxsubmit_index(index*4+1); rxsubmit_index(index*4+2);
    rxsubmit_index(index*4+0); rxsubmit_index(index*4+2); rxsubmit_index(index*4+3);

    ox+=xadvance;
  }

  rxvertex_mode_end();
}

void rxdraw_text(int x, int y, int h, const char *string)
{
  rxdraw_text_ex(rxcolor_kWHITE,x,y,h,string);
}

rxborrowed_t rxborrow_typeless_buffer(rxunknown_t);
rxborrowed_t rxborrow_uniform_buffer(rxuniform_buffer_t buffer)
{
  // todo: actually check that it is a constant buffer!
  return rxborrow_typeless_buffer(buffer.unknown);
}

rxborrowed_t rxborrow_typeless_buffer(rxunknown_t);
rxborrowed_t rxborrow_vertex_buffer(rxvertex_buffer_t buffer)
{
  // todo: actually check that it is a vertex buffer!
  return rxborrow_typeless_buffer(buffer.unknown);
}

rxborrowed_t rxborrow_typeless_buffer(rxunknown_t);
rxborrowed_t rxborrow_index_buffer(rxindex_buffer_t buffer)
{
  // todo: actually check that it is a index buffer!
  return rxborrow_typeless_buffer(buffer.unknown);
}
rxborrowed_t rxborrow_typeless_buffer(rxunknown_t);
rxborrowed_t rxborrow_struct_buffer(rxstruct_buffer_t buffer)
{
  ID3D11Resource *resource;
  ID3D11View_GetResource((ID3D11View*)buffer.unknown,&resource);

  // todo: actually check that it is a struct buffer!
  return rxborrow_typeless_buffer((rxunknown_t)resource);
}

void rxmemcpy_uniform_buffer(rxuniform_buffer_t uniform, void *memory, size_t length)
{
  rxborrowed_t b=rxborrow_uniform_buffer(uniform);
  memcpy(b.memory,memory,length);
  rxreturn(b);
}

// todo: support different formats
rxlocal_texture_t rxload_local_texture(const char *name)
{
  rxlocal_texture_t t;
  t.format=rxRGBA8888;
  t.memory=stbi_load(name,&t.size_x,&t.size_y,0,4);
  t.stride=t.size_x*4;

  return t;
}

rxborrowed_t rxborrow_texture(rxtexture_t texture)
{
  rxborrowed_t result;
  result.resource=0;
  result.  stride=0;
  result.  memory=0;

  // todo: debugonly! ensures this is a valid shader resource view!
  ID3D11ShaderResourceView *View;
  if(SUCCEEDED(
      IUnknown_QueryInterface(texture.unknown,&IID_ID3D11ShaderResourceView,&View)))
  {
    ID3D11Resource *Resource;
    ID3D11ShaderResourceView_GetResource(View,&Resource);

    D3D11_MAPPED_SUBRESOURCE MappedAccess;
    ID3D11DeviceContext_Map(rx.Context,Resource,0,D3D11_MAP_WRITE_DISCARD,0,&MappedAccess);

    result.resource=Resource;
    result.  stride=MappedAccess.RowPitch;
    result.  memory=MappedAccess.pData;

    ccassert(result.stride != 0);
    ccassert(result.memory != 0);
  }

  return result;
}

rxuniform_buffer_t
rxcreate_uniform_buffer(unsigned int size_in_bytes)
{
  size_in_bytes=(size_in_bytes+15)/16*16;

  D3D11_BUFFER_DESC BufferI;
  BufferI.              Usage=D3D11_USAGE_DYNAMIC;
  BufferI.     CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
  BufferI.          MiscFlags=0;
  BufferI.StructureByteStride=0;
  BufferI.          BindFlags=D3D11_BIND_CONSTANT_BUFFER;
  BufferI.          ByteWidth=(UINT)size_in_bytes;

  ID3D11Buffer *Buffer;
  ID3D11Device_CreateBuffer(rx.Device,&BufferI,NULL,&Buffer);

  rxuniform_buffer_t result;
  result.unknown=(rxunknown_t)(Buffer);
  return result;
}

rxborrowed_t
rxborrow_typeless_buffer(rxunknown_t buffer)
{
  rxborrowed_t result;
  result.resource=0;
  result.  length=0;
  result.  memory=0;

  // note: ensures this is a valid buffer!
  ID3D11Resource *Resource;
  if(SUCCEEDED(
      IUnknown_QueryInterface(buffer,&IID_ID3D11Buffer,&Resource)))
  {
    D3D11_MAPPED_SUBRESOURCE MappedAccess;
    ID3D11DeviceContext_Map(rx.Context,Resource,0,D3D11_MAP_WRITE_DISCARD,0,&MappedAccess);

    result.resource=Resource;
    result.  length=MappedAccess.RowPitch;
    result.  memory=MappedAccess.pData;

    ccassert(result.length != 0);
    ccassert(result.memory != 0);
  }

  return result;
}

// ++ merge: add this to shorten the code a little bit? is this silly?
D3D11_TEXTURE2D_DESC
rxtexture2d_legend_d3d11(
          int                  width,
          int                 height,
  DXGI_FORMAT                 format,
  D3D11_USAGE                  usage,
          int             bind_flags,
          int       cpu_access_flags,
          int multisamples_per_pixel,
          int                quality )
{
  ccassert(width >= 1 || cctraceerr("invalid width"));

  ccassert(height >= 1 || cctraceerr("invalid height"));

  D3D11_TEXTURE2D_DESC I;
  I.                 Width=width;
  I.                Height=height;
  I.             MipLevels=1;
  I.             ArraySize=1;
  I.                Format=format;
  I.SampleDesc.      Count=multisamples_per_pixel;
  I.SampleDesc.    Quality=quality;
  I.                 Usage=usage;
  I.             BindFlags=bind_flags;
  I.        CPUAccessFlags=cpu_access_flags;
  I.             MiscFlags=0;
  return I;
}

// todo!: this has to be reworked and possibly removed
rxrender_target_t
rxcreate_render_target(int w, int h, int f)
{
  ID3D11Texture2D          *Texture;
  ID3D11RenderTargetView   *View;
  ID3D11ShaderResourceView *ShaderView;

  rxrender_target_t result;
  result. size_x=w;
  result. size_y=h;
  result. format=f;
  result.unknown=0;

  D3D11_TEXTURE2D_DESC I;
  I=rxtexture2d_legend_d3d11(w,h,f,D3D11_USAGE_DEFAULT,
    // note: not sure if the we care enough about the performance
    // implications of binding to two separate pipeline stages, this is
    // something that I'd have to research...
    D3D11_BIND_RENDER_TARGET|
      D3D11_BIND_SHADER_RESOURCE,0,1,0);

  // note: see ID3D11Device_CheckMultisampleQualityLevels() for safety

  if(SUCCEEDED(ID3D11Device_CreateTexture2D(rx.Device,&I,NULL,&Texture)))
  {
    D3D11_RENDER_TARGET_VIEW_DESC V;
    ZeroMemory(&V,sizeof(V));
    V.Format       =DXGI_FORMAT_UNKNOWN;
    V.ViewDimension=D3D11_RTV_DIMENSION_TEXTURE2DMS;

    if(SUCCEEDED(ID3D11Device_CreateRenderTargetView(rx.Device,
        (ID3D11Resource*)Texture,&V,(ID3D11RenderTargetView **)&View)))
    {

      if(SUCCEEDED(ID3D11Device_CreateShaderResourceView(rx.Device,
          (ID3D11Resource*)Texture,NULL,&ShaderView)))
      {
        result.unknown=(rxunknown_t)View;
        result.asinput=(rxunknown_t)ShaderView;
      }
    }
  }
  return result;
}

rxtexture_t
rxcreate_texture_ex(int w, int h, int f, int s, void *m)
{
  D3D11_TEXTURE2D_DESC TextureI;
  TextureI=rxtexture2d_legend_d3d11(w,h,f,
    D3D11_USAGE_DYNAMIC,D3D11_BIND_SHADER_RESOURCE,D3D11_CPU_ACCESS_WRITE,1,0);

  D3D11_SUBRESOURCE_DATA SubresourceI;
  ZeroMemory(&SubresourceI,sizeof(SubresourceI));
  SubresourceI.    pSysMem=m;
  SubresourceI.SysMemPitch=s;

  ID3D11Texture2D *Texture2D;
  ID3D11Device_CreateTexture2D(rx.Device,&TextureI,m?&SubresourceI:NULL,&Texture2D);

  D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewI;
  ZeroMemory(&ShaderResourceViewI,sizeof(ShaderResourceViewI));
  ShaderResourceViewI.       Format=DXGI_FORMAT_UNKNOWN;
  ShaderResourceViewI.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
  ShaderResourceViewI.    Texture2D.MostDetailedMip=0;
  ShaderResourceViewI.    Texture2D.      MipLevels=1;

  ID3D11ShaderResourceView *ShaderResourceView;
  ID3D11Device_CreateShaderResourceView(rx.Device,
    (ID3D11Resource *)Texture2D,&ShaderResourceViewI,&ShaderResourceView);

  rxtexture_t result;
  result. size_x=w;
  result. size_y=h;
  result. format=f;
  result.unknown=(rxunknown_t)ShaderResourceView;
  return result;
}

rxtexture_t rxcreate_texture(int w, int h, int f)
{
  return rxcreate_texture_ex(w,h,f,0,0);
}

rxtexture_t rxload_texture(rxlocal_texture_t local)
{
  return rxcreate_texture_ex(local.size_x,local.size_y,local.format,local.stride,local.memory);
}

rxtexture_t rxload_texture_file(const char *name)
{
  rxlocal_texture_t local=rxload_local_texture(name);

  rxtexture_t texture=rxload_texture(local);

  stbi_image_free(local.memory);

  return texture;
}

void rxdelete_blobbler(rxblobber_t bytecode)
{
  if(bytecode.unknown)
    bytecode.unknown->lpVtbl->Release(bytecode.unknown);
}

rxblobber_t rxcompile_shader(
  unsigned int length, void *memory, const char *name, const char *entry, const char *model)
{

  rxblobber_t blobber=(rxblobber_t){0};

  if(length != 0 && memory != 0)
  {
    // note: this is not a registered instance, who cares?
    ID3DBlob *BytecodeBlob,*MessagesBlob;
    if(SUCCEEDED(
        D3DCompile(memory,length,name,0,0,entry,model,
          RX_SHADER_COMPILATION_FLAGS,0,&BytecodeBlob,&MessagesBlob)))
    {
      blobber.unknown=BytecodeBlob;
      blobber. memory=BytecodeBlob->lpVtbl->GetBufferPointer(BytecodeBlob);
      blobber. length=BytecodeBlob->lpVtbl->   GetBufferSize(BytecodeBlob);
    } else
    {
      ccprintf("<!4%s!>\r\n",
        (char*)(MessagesBlob->lpVtbl->GetBufferPointer(MessagesBlob)));

      cctracewar("'%s': there were compilation errors",name);
    }
  }

  return blobber;
}

// note: blobber deferred to calling function
rxblobber_t rxcompile_shader_file_ex(
  rxcontents_t *c, const char *master, const char *entry, const char *model)
{
  rxblobber_t b=rxcompile_shader(c->length,c->memory,master,entry,model);

  c->is_erroneous=cctrue;
  if(b.unknown != 0 && b.memory != 0 && b.length != 0)
    c->is_erroneous=ccfalse;
  return b;
}

rxblobber_t rxcompile_shader_file(const char *master, const char *entry, const char *model)
{
  return rxcompile_shader_file_ex(rxreload_contents(master),master,entry,model);
}

rxshader_t rxcreate_vertex_shader(rxinstance_t *i, rxblobber_t bytecode)
{
  ID3D11VertexShader     * VertexShader = NULL;
  ID3DBlob               *     BlobPart = NULL;
  ID3D11ShaderReflection *   Reflection = NULL;
  ID3D11InputLayout      *  InputLayout = NULL;

  if(FAILED(ID3D11Device_CreateVertexShader(rx.Device,
      bytecode.memory,bytecode.length,NULL,&VertexShader)))
        goto error_create_shader;

  if(FAILED(D3DGetBlobPart(bytecode.memory,bytecode.length,
      D3D_BLOB_INPUT_SIGNATURE_BLOB,0,&BlobPart)))
        goto error_input_signature;

  if(FAILED(D3DReflect(bytecode.memory,bytecode.length,
      &IID_ID3D11ShaderReflection,(void**)&Reflection)))
        goto error_reflection;

  D3D11_SHADER_DESC ShaderInfo;
  Reflection->lpVtbl->GetDesc(Reflection,&ShaderInfo);

  D3D11_SIGNATURE_PARAMETER_DESC ElementInfo;

  // todo: what's the max size of an element array?
  D3D11_INPUT_ELEMENT_DESC ElementArray[0x20];

  for(int ElementIndex=0;ElementIndex<ShaderInfo.InputParameters;++ElementIndex)
  {
    Reflection->lpVtbl->GetInputParameterDesc(Reflection,ElementIndex,&ElementInfo);

    D3D11_INPUT_ELEMENT_DESC *Element=ElementArray+ElementIndex;
    Element-> SemanticName=ElementInfo.SemanticName;
    Element->SemanticIndex=ElementInfo.SemanticIndex;

    // todo: support other formats!
    if(ElementInfo.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
    {
      switch(ElementInfo.Mask)
      { case 0b0001: Element->Format=0;                          break;
        case 0b0011: Element->Format=DXGI_FORMAT_R32G32_FLOAT;   break;
        case 0b0111: Element->Format=0;                          break;
        case 0b1111: Element->Format=DXGI_FORMAT_R8G8B8A8_UNORM; break;
        default:
          ccassert(!"not impl");
      }
    } else
      ccassert(!"not implemented");

    Element->           InputSlot=0;
    Element->   AlignedByteOffset=D3D11_APPEND_ALIGNED_ELEMENT;
    Element->      InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA;
    Element->InstanceDataStepRate=0;
  }

  if(FAILED(ID3D11Device_CreateInputLayout(rx.Device,
      ElementArray,ShaderInfo.InputParameters,bytecode.memory,bytecode.length,&InputLayout)))
        goto error_input_layout;

  ccassert(InputLayout != 0);

  rxshader_t result=(rxshader_t){(rxunknown_t)(VertexShader)};

  rxcreate_instance(i,rx_kVERTEX_SHADER,(rxunknown_t)VertexShader);
  // todo!!!:
  rxcreate_instance(0,rx_kINPUT_LAYOUT,(rxunknown_t)InputLayout);

  rxshader_store_input_layout_d3d(result.unknown,InputLayout);

goto leave;

error_create_shader:
  cctracewar("failed to create vertex shader");
  goto error;
error_input_signature:
  cctracewar("there was a problem extracting the input signature");
  goto error;
error_reflection:
  cctracewar("there was a problem with the reflection interface");
  goto error;
error_input_layout:
  cctracewar("there was a problem creating the input layout");
  goto error;
error:

  result=(rxshader_t){0};

leave:
  return result;
}

rxshader_t rxcreate_pixel_shader(rxinstance_t *i, rxblobber_t bytecode)
{
  ID3D11PixelShader *DeviceChild = NULL;

  if(SUCCEEDED(ID3D11Device_CreatePixelShader(rx.Device,
      bytecode.memory,bytecode.length,NULL,&DeviceChild)))
  {
    rxcreate_instance(i,rx_kPIXEL_SHADER,(rxunknown_t)DeviceChild);
  }

  rxshader_t result=(rxshader_t){(rxunknown_t)(DeviceChild)};
  return result;
}

// note: loads or re-loads a pixel shader
rxshader_t rxload_pixel_shader(
  rxinstance_t *i, const char *master, const char *entry)
{
  // todo!!: we should get this from the device!
  char *model="ps_5_0";

  if(i != 0)
  {
    master=master?master:i->master;
    entry=entry?entry:i->restore.shader_entry;
    model=model?model:i->restore.shader_model;
  }

  ccassert(master != 0);
  ccassert( entry != 0);
  ccassert( model != 0);

  rxcontents_t *c=rxreload_contents(master);
  rxblobber_t b=rxcompile_shader_file_ex(c,master,entry,model);

  rxshader_t r=(rxshader_t){0};

  if((b.memory != 0) && (b.length != 0))
  {
    r=rxcreate_pixel_shader(i,b);

    rxdelete_blobbler(b);

    if(r.unknown != 0)
    {
      if(i == 0)
        i=rxlookup_instance(r.unknown);

      i->unknown=r.unknown;
      i->loaded =c->loaded;
      i->master =master;
      i->restore.shader_model=model;
      i->restore.shader_entry=entry;
    }
  }
  return r;
}

// note: loads or re-loads a vertex shader
rxshader_t rxload_vertex_shader(
  rxinstance_t *i, const char *master, const char *entry)
{
  // todo!!: we should get this from the device!
  char *model="vs_5_0";

  if(i != 0)
  {
    master=master?master:i->master;
    entry=entry?entry:i->restore.shader_entry;
    model=model?model:i->restore.shader_model;
  }

  ccassert(master != 0);
  ccassert( entry != 0);
  ccassert( model != 0);

  rxcontents_t *c=rxreload_contents(master);
  rxblobber_t b=rxcompile_shader_file_ex(c,master,entry,model);

  rxshader_t r=(rxshader_t){0};

  if((b.memory != 0) && (b.length != 0))
  {
    r=rxcreate_vertex_shader(i,b);

    rxdelete_blobbler(b);

    if(r.unknown != 0)
    {
      if(i == 0)
        i=rxlookup_instance(r.unknown);

      i->unknown=r.unknown;
      i->loaded =c->loaded;
      i->master =master;
      i->restore.shader_model=model;
      i->restore.shader_entry=entry;
    }
  }

  return r;
}

// todo: update the viewport and frame buffer!
void rxresize(int w, int h)
{
  RECT Client;
  Client.  left=0;
  Client.   top=0;
  Client. right=w;
  Client.bottom=h;
  AdjustWindowRect(&Client,WS_OVERLAPPEDWINDOW,FALSE);
  SetWindowPos(rx.Window,HWND_NOTOPMOST,0,0,Client.right,Client.bottom,SWP_NOMOVE|SWP_NOACTIVATE|SWP_FRAMECHANGED);
}

void rxresize_uniformbuffer(size_t size_in_bytes)
{
  rxdelete_uniform_buffer(rx.uniform_buffer);

  D3D11_BUFFER_DESC BufferI;
  BufferI.              Usage=D3D11_USAGE_DYNAMIC;
  BufferI.     CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
  BufferI.          MiscFlags=0;
  BufferI.StructureByteStride=0;
  BufferI.          BindFlags=D3D11_BIND_CONSTANT_BUFFER;
  BufferI.          ByteWidth=(UINT)size_in_bytes;
  ID3D11Device_CreateBuffer(rx.Device,&BufferI,NULL,(ID3D11Buffer**)&rx.uniform_buffer.unknown);
}

rxindex_buffer_t rxcreate_index_buffer(size_t index_size, size_t index_count)
{
  D3D11_BUFFER_DESC i;
  i.              Usage=D3D11_USAGE_DYNAMIC;
  i.     CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
  i.          MiscFlags=0;
  i.StructureByteStride=0;
  i.          BindFlags=D3D11_BIND_INDEX_BUFFER;
  i.          ByteWidth=(UINT)(index_size * index_count);

  ID3D11Buffer *Buffer;
  ID3D11Device_CreateBuffer(rx.Device,&i,NULL,&Buffer);

  rxindex_buffer_t result;
  result.unknown=(rxunknown_t)Buffer;
  return result;
}

rxvertex_buffer_t rxcreate_vertex_buffer(size_t vertex_size, size_t vertex_count)
{
  D3D11_BUFFER_DESC i;
  i.              Usage=D3D11_USAGE_DYNAMIC;
  i.     CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
  i.          MiscFlags=0;
  i.StructureByteStride=0;
  i.          BindFlags=D3D11_BIND_VERTEX_BUFFER;
  i.          ByteWidth=(UINT)(vertex_size * vertex_count);

  ID3D11Buffer *Buffer;
  ID3D11Device_CreateBuffer(rx.Device,&i,NULL,&Buffer);

  rxvertex_buffer_t result;
  result.unknown=(rxunknown_t)Buffer;
  return result;
}

rxstruct_buffer_t rxcreate_struct_buffer(size_t struct_size, size_t struct_count)
{
  D3D11_BUFFER_DESC i;
  i.              Usage=D3D11_USAGE_DYNAMIC;
  i.     CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
  i.          MiscFlags=D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
  i.StructureByteStride=(UINT)(struct_size);
  i.          BindFlags=D3D11_BIND_SHADER_RESOURCE;
  i.          ByteWidth=(UINT)(struct_size * struct_count);

  ID3D11Buffer *Buffer;
  ID3D11Device_CreateBuffer(rx.Device,&i,NULL,&Buffer);

  D3D11_SHADER_RESOURCE_VIEW_DESC D;
  D.             Format=DXGI_FORMAT_UNKNOWN;
  D.      ViewDimension=D3D11_SRV_DIMENSION_BUFFER;
  D.Buffer.FirstElement=0;
  D.Buffer. NumElements=(UINT)(struct_count);

  ID3D11ShaderResourceView *View;
  ID3D11Device_CreateShaderResourceView(rx.Device,(ID3D11Resource*)Buffer,&D,&View);

  rxstruct_buffer_t result;
  result.unknown=(rxunknown_t)View;

  return result;
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

void rxwindow()
{
  MSG message;
  while(PeekMessage(&message,NULL,0,0,PM_REMOVE))
  { TranslateMessage(&message);
    DispatchMessageW(&message);
  }

  RECT client;
  GetClientRect(rx.Window,&client);
  rx.size_x=client.right-client.left;
  rx.size_y=client.bottom-client.top;
  rx.center_x=rx.size_x>>1;
  rx.center_y=rx.size_y>>1;
}

rxcandle_t *rxeffect_candle()
{
  return rx.candle_array + rx.candle_tally++ % rx.candle_threshold;
}

rxshadow_t *rxeffect_shadow()
{
  return rx.shadow_array + rx.shadow_tally++ % rx.shadow_threshold;
}

void rxrestore_render_stack()
{
  rx.shader_index=0;
  rx.target_index=0;

  rx.target=rx.offscreen_target;

  // todo:
  float clear_color[]={.2f,.2f,.2f,1.f};

  rxdriver_clear_render_target(rx.  offscreen_target,clear_color);
  rxdriver_clear_render_target(rx.effect_target,clear_color);

  rxdriver_stage_render_target(rx.target);

  rxdriver_stage_shader(rx.default_vertex_shader);
  rxdriver_stage_shader(rx. default_pixel_shader);

  rx.world_matrix=rxmatrix_identity();
  rx. view_matrix=rxmatrix_identity();
  rx.view_matrix.m[0][0]=+ 2.0f/(rx.size_x);
  rx.view_matrix.m[1][1]=+ 2.0f/(rx.size_y);
  rx.view_matrix.m[2][2]=+ 0.5f;
  rx.view_matrix.m[3][0]=- 1.0f;
  rx.view_matrix.m[3][1]=- 1.0f;
}

// todo!!: this is possibly the slowest thing ever, don't write games like this!
void rxdefault_render_pass()
{ rx.vertex_buffer_writeonly=rxborrow_vertex_buffer(rx.vertex_buffer);
  rx. index_buffer_writeonly= rxborrow_index_buffer(rx. index_buffer);
  rx.           vertex_array=rx.vertex_buffer_writeonly.memory;
  rx.            index_array=rx. index_buffer_writeonly.memory;
  rx. vertex_tally=0;
  rx.  index_tally=0;
  rx.command_tally=0;

  // note: prepare our resources
  rx.shadow_buffer_writeonly=rxborrow_struct_buffer(rx.shadow_buffer);
  rx.candle_buffer_writeonly=rxborrow_struct_buffer(rx.candle_buffer);

  rx.shadow_array=rx.shadow_buffer_writeonly.memory;
  rx.candle_array=rx.candle_buffer_writeonly.memory;

  // todo!!!: the shader should take the number of effects and shadows!
  memset(rx.shadow_array,rx.shadow_threshold,sizeof(*rx.shadow_array)*rx.shadow_threshold);
  memset(rx.candle_array,rx.candle_threshold,sizeof(*rx.candle_array)*rx.candle_threshold);

  rx.shadow_tally=0;
  rx.candle_tally=0;
}

// todo!!: this is possibly the slowest thing ever, don't write games like this!
void rxdefault_render_pass_end()
{
  rxreturn(rx.vertex_buffer_writeonly);
  rxreturn(rx. index_buffer_writeonly);

  rx.vertex_array=ccnull;
  rx. index_array=ccnull;

  rxreturn(rx.shadow_buffer_writeonly);
  rxreturn(rx.candle_buffer_writeonly);

  rx.shadow_array=ccnull;
  rx.candle_array=ccnull;

  rxuniform_t t;
  t.            e=rxmatrix_multiply(rx.world_matrix,rx.view_matrix);
  t. screen_xsize=(float)(rx.size_x);
  t. screen_ysize=(float)(rx.size_y);
  t.mouse_xcursor=(float)(rx.xcursor) / rx.size_x; // todo!!:
  t.mouse_ycursor=(float)(rx.ycursor) / rx.size_y; // todo!!:
  t.total_seconds=rx.total_seconds;
  t.delta_seconds=rx.delta_seconds;
  t. shadow_tally=rx.shadow_tally;
  t. candle_tally=rx.candle_tally;

  rxmemcpy_uniform_buffer(rx.uniform_buffer,&t,sizeof(t));
}


// ccdebuglog("command - %s",draw->master?draw->master:"unnamed");
int rxexec_command(rxcommand_t *draw, int index_offset)
{
  int result = 0;

  switch(draw->kind)
  {
    case rx_kCLIP:
    break;
    case rx_kPULLTARGET:
    {
      rx.target=rx.target_stack[--rx.target_index];
    } break;
    case rx_kPUSHTARGET:
    {
      rx.target_stack[rx.target_index++]=rx.target;
      rx.target=draw->target;

      rxdriver_stage_render_target(draw->target);

    } break;
    case rx_kCLEAR:
    { float color[]={draw->r,draw->g,draw->b,draw->a};
      ID3D11DeviceContext_ClearRenderTargetView(rx.Context,
        (ID3D11RenderTargetView*)rx.target.unknown,color);
    } break;
    case rx_kCUSTOM:
    {
      ccassert(draw->custom!=0);
      draw->custom();
    } break;
    // todo: this is to be revised!
    case rx_kMATRIX:
    {
      rx.world_matrix=draw->matrix;
    } break;
    // todo: this is to be revised!
    case rx_kMATRIX_MULTIPLY:
    {
      rx.world_matrix=rxmatrix_multiply(rx.world_matrix,draw->matrix);
    } break;
    case rx_kPUSHSHADER:
    {
      ccassert(draw->shader.unknown!=0);

      ccassert(rx.shader_index < ccCarrlenL(rx.shader_stack));

      rx.shader_stack[rx.shader_index++]=rx.shader;
      rxdriver_stage_shader(draw->shader);
    } break;
    case rx_kPULLSHADER:
    { ccassert(rx.shader_index > 0);

      rxshader_t shader=rx.shader_stack[--rx.shader_index];
      rxdriver_stage_shader(shader);
    } break;
    case rx_kUNIFORM:
    {
#if 0
      ccassert(draw->unknown!=0);
      ccassert(rx.LastShaderSetByUser !=0 ||
        cctraceerr("'no shader': expected a previous command to have been a shader"));
      if(SUCCEEDED(IUnknown_QueryInterface(draw->unknown,&IID_ID3D11Buffer,&rx.LastBufferSetByUser)))
      {
        ID3D11DeviceChild *DummyShader;
        if(SUCCEEDED(IUnknown_QueryInterface(rx.LastShaderSetByUser,&IID_ID3D11VertexShader,&DummyShader)))
          ID3D11DeviceContext_VSSetConstantBuffers(rx.Context,0,1,(ID3D11Buffer**)&rx.LastBufferSetByUser);
        else
        if(SUCCEEDED(IUnknown_QueryInterface(rx.LastShaderSetByUser,&IID_ID3D11PixelShader,&DummyShader)))
          ID3D11DeviceContext_PSSetConstantBuffers(rx.Context,0,1,(ID3D11Buffer**)&rx.LastBufferSetByUser);
        else
          cctracewar("'rxqueue_shader_command()': expected vertex or pixel shader",0);
      } else
          cctraceerr("'rxqueue_shader_command()': invalid interface, expected buffer",0);
#endif
    } break;
    case rx_kSAMPLER:
    {

      rxdriver_stage_sampler(draw->sampler,draw->slot);

    } break;
    case rx_kTEXTURE:
    {
      rxdriver_stage_texture(draw->texture,draw->slot);
    } break;
    case rx_kINDEXED:
    {
      ID3D11DeviceContext_DrawIndexed(rx.Context,draw->length,index_offset,draw->offset);
      result += draw->length;

    } break;
  }

  return result;
}

void rxpull_live_reload_changes()
{
  if(!rx.LiveReloadDirectory)
    return;

  FILE_NOTIFY_INFORMATION *Entry;
                     char *EntryCursor;

  ccglobal char   EntryBuffer[(sizeof(* Entry) + MAX_PATH) << 4];
  ccglobal  int IsEventActive;

  OVERLAPPED Overlapped;
  ZeroMemory(&Overlapped,sizeof(Overlapped));
  Overlapped.hEvent=rx.LiveReloadEvent;

  if(!IsEventActive)
  {
    memset(EntryBuffer,0,sizeof(EntryBuffer));

    if(ReadDirectoryChangesW(
        rx.LiveReloadDirectory,EntryBuffer,sizeof(EntryBuffer),
          TRUE, // note: watch the sub-tree as well!
          FILE_NOTIFY_CHANGE_LAST_WRITE,NULL,&Overlapped,NULL))
    {
      IsEventActive = TRUE;
    }
  }

  if(IsEventActive)
  { if(WAIT_OBJECT_0 == WaitForSingleObject(rx.LiveReloadEvent,0))
    { DWORD BytesRead=0;
      if(GetOverlappedResult(rx.LiveReloadDirectory,&Overlapped,&BytesRead,FALSE))
      { for(EntryCursor=EntryBuffer;EntryCursor<EntryBuffer+sizeof(EntryBuffer);)
        { Entry=(FILE_NOTIFY_INFORMATION*)(EntryCursor);

          char FileName[MAX_PATH+4];
          memset(FileName,0,sizeof(FileName));

          if(WideCharToMultiByte(CP_UTF8,0,Entry->FileName,Entry->FileNameLength,
              FileName,sizeof(FileName),NULL,FALSE))
          {
            if(rxcontents_registered(FileName))
            {
              rxreload_contents(FileName);
            }
          }

          if(!Entry->NextEntryOffset)
            break;

          EntryCursor += Entry->NextEntryOffset;
        }

        IsEventActive = FALSE;
      }
    }
  }

}

int rxtick()
{
  rx.tick_count++;

  rxpull_live_reload_changes();

  rxwindow();


  // todo: clear_color
#if 0
#ifdef RX_ENABLE_DEPTH_BUFFER
  ID3D11DeviceContext_ClearDepthStencilView(
    rx.Context,rx.DepthBuffer,D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.0f,0);
#endif
#endif

  // todo: this is extremely unsafe and unpredictable, get something more robust!
  rxdraw_end();

  rxdraw_skinned_preset_unnormalized(
    rxcolor_kWHITE,0,0,(float)(rx.size_x),(float)(rx.size_y),0,0,1,1);

  // todo: this is extremely unsafe and unpredictable, get something more robust!
  rxdraw_end();

  // note:
  rxdefault_render_pass_end();

  int index_offset=0;

  rxcommand_t *command=rx.command_array;
  {

    while(command->kind!=rx_kENDPASS)
    {
      index_offset += rxexec_command(command,index_offset);
      ++ command;
    }

    command++;
  }

  // todo!!: this is still in the works!
  // note: apply our 'effect' render pass to the last target
  if(rx.effect_vertex_shader.unknown != 0 &&
     rx. effect_pixel_shader.unknown != 0)
  {
    // rxreload_shader(&rx.effect_vertex_shader);
    // rxreload_shader(&rx. effect_pixel_shader);

           rxshader_t vertex_shader=rx.effect_vertex_shader;
           rxshader_t  pixel_shader=rx. effect_pixel_shader;
    rxrender_target_t  input_target=rx.              target;
    rxrender_target_t        target=rx.       effect_target;
    rxstruct_buffer_t shadow_buffer=rx.shadow_buffer;
    rxstruct_buffer_t candle_buffer=rx.candle_buffer;

    float clear_color[]={.3f,.3f,.4f,1};
    rxdriver_stage_render_target(target);

    rxdriver_stage_shader(vertex_shader);
    rxdriver_stage_shader( pixel_shader);

    // todo!:
    ID3D11ShaderResourceView *ShaderResourceView[3];
    IUnknown_QueryInterface( input_target.asinput,&IID_ID3D11ShaderResourceView,&ShaderResourceView[0]);
    IUnknown_QueryInterface(shadow_buffer.unknown,&IID_ID3D11ShaderResourceView,&ShaderResourceView[1]);
    IUnknown_QueryInterface(candle_buffer.unknown,&IID_ID3D11ShaderResourceView,&ShaderResourceView[2]);
    ID3D11DeviceContext_PSSetShaderResources(rx.Context,0,ccCarrlenL(ShaderResourceView),ShaderResourceView);

    {
      while(command->kind!=rx_kENDPASS)
      {
        index_offset += rxexec_command(command,index_offset);
        ++ command;
      }

      command++;
    }

    // note: unbind the render target that we previously bound to the pixel shader,
    // this will allow it to be used as a render target once again.
    // todo: we only have to unbind the first one!
    memset(ShaderResourceView,0,sizeof(ShaderResourceView));
    ID3D11DeviceContext_PSSetShaderResources(rx.Context,0,ccCarrlenL(ShaderResourceView),ShaderResourceView);
  } else
  {
    rxrender_target_t o=rx.effect_target;
    rxrender_target_t i=rx.target;

    ID3D11Resource *OutputResource;
    ID3D11View_GetResource((ID3D11View*)o.unknown,&OutputResource);
    ID3D11Resource *InputResource;
    ID3D11View_GetResource((ID3D11View*)i.unknown,&InputResource);

    ID3D11DeviceContext_CopyResource(rx.Context,OutputResource,InputResource);
  }

  // note: finally, move the post-process target to our screen buffer to display
  {
    ID3D11Resource *InputResource;
    ID3D11View_GetResource((ID3D11View*)rx.effect_target.unknown,&InputResource);

    ID3D11Resource *OutputResource;
    ID3D11View_GetResource((ID3D11View*)rx.screen_target.unknown,&OutputResource);

    // todo!!: replace with CopySubresource!
    ID3D11DeviceContext_ResolveSubresource(rx.Context,
      OutputResource,0,InputResource,0,DXGI_FORMAT_R8G8B8A8_UNORM);
  }

  IDXGISwapChain_Present(rx.SwapChain,1u,0);

  // todo!:
  WaitForSingleObjectEx(rx.FrameAwait,INFINITE,TRUE);

  if(!rx.Visible)
  {
    rx.Visible=cctrue;
    ShowWindow(rx.Window,SW_SHOW);
  }

  rxrestore_render_stack();

  rxdefault_render_pass();

  rxtime();
  return !rx.Quitted;
}

int rxwindow_event_win32(UINT Message, WPARAM wParam, LPARAM lParam)
{ switch(Message)
  { case WM_CLOSE:
    case WM_QUIT:
    { PostQuitMessage(0);
      rx.Quitted=TRUE;
    } break;
    case WM_MOUSEMOVE:
    {
      int xcursor=GET_X_LPARAM(lParam);
      int ycursor=GET_Y_LPARAM(lParam);

      rx.xcursor=xcursor;
      rx.ycursor=rx.size_y-ycursor;
    } break;
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
    { rx.window_events[rx_kLBUTTON]=1;
      if(!rx.ClickFocused)
      { rx.ClickFocused=TRUE;
        SetCapture((HWND)rx.Window);
      }
    } break;
    case WM_LBUTTONUP: case WM_RBUTTONUP:
    case WM_MBUTTONUP: case WM_XBUTTONUP:
    { rx.window_events[rx_kLBUTTON]=0;

      rx.ClickFocused=FALSE;
      ReleaseCapture();
    } break;
    case WM_KEYUP:
    case WM_KEYDOWN:
    { rx.window_events[wParam]=Message==WM_KEYDOWN;
    } break;
    default:
    { return FALSE;
    } break;
  }
  return TRUE;
}

LRESULT CALLBACK
rxwindow_callback_win32(HWND Window,UINT Message,WPARAM wParam,LPARAM lParam)
{
  LRESULT result=rxwindow_event_win32(Message,wParam,lParam);

  if(!result)
  {
    result=DefWindowProcW(Window,Message,wParam,lParam);
  }

  return result;
}

void rxinit_default_font();

void rxinit(const wchar_t *window_title)
{
#ifdef _RX_STANDALONE
  ccinit();
#endif


  UINT DriverModeFlags=
    D3D11_CREATE_DEVICE_DEBUG| // -- Note: COMMENT THIS OUT TO USE INTEL'S GRAPHIC ANALYZER
    D3D11_CREATE_DEVICE_SINGLETHREADED|D3D11_CREATE_DEVICE_BGRA_SUPPORT;
  D3D_FEATURE_LEVEL DriverFeatureMenu[2][2]=
  { {D3D_FEATURE_LEVEL_11_1,D3D_FEATURE_LEVEL_11_0},
    {D3D_FEATURE_LEVEL_10_1,D3D_FEATURE_LEVEL_10_0},
  };
  D3D_FEATURE_LEVEL DriverSelectedFeatureLevel;
  if(SUCCEEDED(D3D11CreateDevice(NULL,D3D_DRIVER_TYPE_HARDWARE,0,DriverModeFlags,DriverFeatureMenu[0],
      ARRAYSIZE(DriverFeatureMenu[0]),D3D11_SDK_VERSION,&rx.Device,&DriverSelectedFeatureLevel,&rx.Context))||
     SUCCEEDED(D3D11CreateDevice(NULL,D3D_DRIVER_TYPE_WARP,0,DriverModeFlags,DriverFeatureMenu[1],
      ARRAYSIZE(DriverFeatureMenu[1]),D3D11_SDK_VERSION,&rx.Device,&DriverSelectedFeatureLevel,&rx.Context)))
  { if((DriverModeFlags&D3D11_CREATE_DEVICE_DEBUG))
    { if(SUCCEEDED(IProvideClassInfo_QueryInterface(rx.Device,&IID_ID3D11InfoQueue,(void**)&rx.InfoQueue)))
      { ID3D11InfoQueue_SetBreakOnSeverity(rx.InfoQueue, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        ID3D11InfoQueue_SetBreakOnSeverity(rx.InfoQueue, D3D11_MESSAGE_SEVERITY_ERROR,      TRUE);
        ID3D11InfoQueue_SetBreakOnSeverity(rx.InfoQueue, D3D11_MESSAGE_SEVERITY_WARNING,    TRUE);
      }
    }
  }

  WNDCLASSW WindowClass;
  ZeroMemory(&WindowClass,sizeof(WindowClass));
  WindowClass.lpfnWndProc=rxwindow_callback_win32;
  WindowClass.hInstance=GetModuleHandleW(NULL);
  WindowClass.lpszClassName=window_title;
  RegisterClassW(&WindowClass);

  rx.Window=CreateWindowExW(WS_EX_NOREDIRECTIONBITMAP,WindowClass.lpszClassName,window_title,
    WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
      NULL,NULL,WindowClass.hInstance,NULL);

  SetCursor(LoadCursorA(NULL,IDC_ARROW));

  rxwindow();

  // todo: maybe call this a screen_render_target_t ?
  {
    // note: we can use the adapter to enumerate display devices,
    // this might come useful to the user!
    IDXGIFactory2 * DXGIFactory=NULL;
    IDXGIDevice   *  DXGIDevice=NULL;
    IDXGIAdapter  * DXGIAdapter=NULL;
    ID3D11Device_QueryInterface(rx.Device,&IID_IDXGIDevice,(void **)&DXGIDevice);
    IDXGIDevice_GetAdapter(DXGIDevice,&DXGIAdapter);
    IDXGIAdapter_GetParent(DXGIAdapter,&IID_IDXGIFactory2,(void**)&DXGIFactory);
    IDXGIAdapter_Release(DXGIAdapter);
    IDXGIDevice_Release(DXGIDevice);

    // todo: in case we ever need this, 'DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT'
    DXGI_SWAP_CHAIN_DESC1 SwapchainInfo;
    ZeroMemory(&SwapchainInfo,sizeof(SwapchainInfo));
    SwapchainInfo.      Width=rx.size_x;
    SwapchainInfo.     Height=rx.size_y;
    SwapchainInfo.     Format=DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapchainInfo.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT; // DXGI_USAGE_UNORDERED_ACCESS
    SwapchainInfo.BufferCount=2;
    SwapchainInfo. SwapEffect=DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    SwapchainInfo.      Flags=DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH|DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
    SwapchainInfo. SampleDesc.  Count=1;
    SwapchainInfo. SampleDesc.Quality=0;

    // todo: allow the user to place a cap on the frame rate?
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC SwapchainFullscreenInfo;
    ZeroMemory(&SwapchainFullscreenInfo,sizeof(SwapchainFullscreenInfo));
    SwapchainFullscreenInfo.RefreshRate.  Numerator=1;
    SwapchainFullscreenInfo.RefreshRate.Denominator=1;
    SwapchainFullscreenInfo.Windowed=TRUE;

    ID3D11Texture2D          *RenderBuffer;
    ID3D11RenderTargetView   *RenderBufferView;

    IDXGIFactory2_CreateSwapChainForHwnd(DXGIFactory,(IUnknown *)rx.Device,rx.Window,
      &SwapchainInfo,&SwapchainFullscreenInfo,NULL,(IDXGISwapChain1 **)&rx.SwapChain);

    rx.FrameAwait=IDXGISwapChain2_GetFrameLatencyWaitableObject(rx.SwapChain);
    IDXGISwapChain_GetBuffer(rx.SwapChain,0,&IID_ID3D11Texture2D,(void **)&RenderBuffer);
    IDXGIFactory_Release(DXGIFactory);

    D3D11_RENDER_TARGET_VIEW_DESC RenderTargetViewInfo;
    ZeroMemory(&RenderTargetViewInfo,sizeof(RenderTargetViewInfo));
    RenderTargetViewInfo.       Format=DXGI_FORMAT_UNKNOWN;
    RenderTargetViewInfo.ViewDimension=D3D11_RTV_DIMENSION_TEXTURE2D;
    ID3D11Device_CreateRenderTargetView(rx.Device,(ID3D11Resource*)RenderBuffer,&RenderTargetViewInfo,&RenderBufferView);

    // todo!:
    rx.screen_target. size_x=SwapchainInfo. Width;
    rx.screen_target. size_y=SwapchainInfo.Height;
    rx.screen_target. format=SwapchainInfo.Format;
    rx.screen_target.unknown=(rxunknown_t)RenderBufferView;
    rx.screen_target.asinput=0;

    // todo?: make separate targets for these, these are intermediate targets, i guess?
    rx.offscreen_target=rxcreate_render_target(
      rx.screen_target.size_x,rx.screen_target.size_y,rx.screen_target.format);

    rx.effect_target=rxcreate_render_target(
      rx.screen_target.size_x,rx.screen_target.size_y,rx.screen_target.format);

    // todo: pre-compile!
    rx.effect_vertex_shader=rxload_vertex_shader(NULL,"light.hlsl","MainVS");
    rx. effect_pixel_shader= rxload_pixel_shader(NULL,"light.hlsl","MainPS");

    rx.default_vertex_shader=
      rxcreate_vertex_shader(NULL,(rxblobber_t){0,(void*)rx_vs_shader_bytecode,sizeof(rx_vs_shader_bytecode)});
    rx.default_pixel_shader=
      rxcreate_pixel_shader(NULL,(rxblobber_t){0,(void*)rx_ps_shader_bytecode,sizeof(rx_ps_shader_bytecode)});

    rxrestore_render_stack();
  }

  // note: create default resources
  // todo: allow the user to turn effects and shadows on or off!
  {
    rx.candle_threshold=16;
    rx.shadow_threshold=256;

    rx.candle_buffer=
      rxcreate_struct_buffer(sizeof(rxcandle_t),rx.candle_threshold);

    rx.shadow_buffer=
      rxcreate_struct_buffer(sizeof(rxshadow_t),rx.shadow_threshold);
  }


#ifdef RX_ENABLE_DEPTH_BUFFER
  D3D11_DEPTH_STENCIL_DESC DepthBufferStateConfig;
  DepthBufferStateConfig.     DepthEnable=FALSE;
  DepthBufferStateConfig.  DepthWriteMask=D3D11_DEPTH_WRITE_MASK_ALL;
  DepthBufferStateConfig.       DepthFunc=D3D11_COMPARISON_LESS;
  DepthBufferStateConfig.   StencilEnable=FALSE;
  DepthBufferStateConfig. StencilReadMask=0xFF;
  DepthBufferStateConfig.StencilWriteMask=0xFF;
  DepthBufferStateConfig.FrontFace.      StencilFailOp=D3D11_STENCIL_OP_KEEP;
  DepthBufferStateConfig.FrontFace. StencilDepthFailOp=D3D11_STENCIL_OP_DECR;
  DepthBufferStateConfig.FrontFace.      StencilPassOp=D3D11_STENCIL_OP_KEEP;
  DepthBufferStateConfig.FrontFace.        StencilFunc=D3D11_COMPARISON_ALWAYS;
  DepthBufferStateConfig. BackFace.      StencilFailOp=D3D11_STENCIL_OP_KEEP;
  DepthBufferStateConfig. BackFace. StencilDepthFailOp=D3D11_STENCIL_OP_DECR;
  DepthBufferStateConfig. BackFace.      StencilPassOp=D3D11_STENCIL_OP_KEEP;
  DepthBufferStateConfig. BackFace.        StencilFunc=D3D11_COMPARISON_ALWAYS;
  ID3D11Device_CreateDepthStencilState(rx.Device,&DepthBufferStateConfig,&rx.DepthBufferState);

  D3D11_TEXTURE2D_DESC DepthBufferStoreConfig;
  DepthBufferStoreConfig.         Width=(UINT)(rx.size_x);
  DepthBufferStoreConfig.        Height=(UINT)(rx.size_y);
  DepthBufferStoreConfig.     MipLevels=1;
  DepthBufferStoreConfig.     ArraySize=1;
  DepthBufferStoreConfig.        Format=DXGI_FORMAT_D32_FLOAT;
  DepthBufferStoreConfig.         Usage=D3D11_USAGE_DEFAULT;
  DepthBufferStoreConfig.     BindFlags=D3D11_BIND_DEPTH_STENCIL;
  DepthBufferStoreConfig.CPUAccessFlags=0;
  DepthBufferStoreConfig.     MiscFlags=0;
  DepthBufferStoreConfig.SampleDesc.  Count=1;
  DepthBufferStoreConfig.SampleDesc.Quality=0;
  ID3D11Device_CreateTexture2D(rx.Device,&DepthBufferStoreConfig,0x0,&rx.DepthBufferStore);

  ID3D11Device_CreateDepthStencilView(rx.Device,(ID3D11Resource *)rx.DepthBufferStore,NULL,&rx.DepthBuffer);
#endif

  D3D11_RASTERIZER_DESC RasterizerInfo;
  ZeroMemory(&RasterizerInfo,sizeof(RasterizerInfo));
  RasterizerInfo.             FillMode=D3D11_FILL_SOLID;
  RasterizerInfo.             CullMode=D3D11_CULL_NONE;
  RasterizerInfo.FrontCounterClockwise=FALSE;
  RasterizerInfo.            DepthBias=D3D11_DEFAULT_DEPTH_BIAS;
  RasterizerInfo.       DepthBiasClamp=D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
  RasterizerInfo. SlopeScaledDepthBias=D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
  RasterizerInfo.      DepthClipEnable=FALSE;
  RasterizerInfo.        ScissorEnable=FALSE;
  RasterizerInfo.    MultisampleEnable=FALSE;
  RasterizerInfo.AntialiasedLineEnable=FALSE;
  ID3D11Device_CreateRasterizerState(rx.Device,&RasterizerInfo,&rx.RasterizerState);

  D3D11_BLEND_DESC BlendStateInfo;
  ZeroMemory(&BlendStateInfo,sizeof(BlendStateInfo));
  for(int I=0; I<1; ++I)
  {
    BlendStateInfo.RenderTarget[I].          BlendEnable=TRUE;
    BlendStateInfo.RenderTarget[I].             SrcBlend=D3D11_BLEND_SRC_ALPHA;
    BlendStateInfo.RenderTarget[I].            DestBlend=D3D11_BLEND_INV_SRC_ALPHA;
    BlendStateInfo.RenderTarget[I].              BlendOp=D3D11_BLEND_OP_ADD;

    BlendStateInfo.RenderTarget[I].        SrcBlendAlpha=D3D11_BLEND_ZERO;
    BlendStateInfo.RenderTarget[I].       DestBlendAlpha=D3D11_BLEND_ZERO;
    BlendStateInfo.RenderTarget[I].         BlendOpAlpha=D3D11_BLEND_OP_ADD;
    BlendStateInfo.RenderTarget[I].RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;
  }
  ID3D11Device_CreateBlendState(rx.Device,&BlendStateInfo,&rx.BlendState);



  // D3D11_INPUT_ELEMENT_DESC LayoutElements[]=
  // { (D3D11_INPUT_ELEMENT_DESC){"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
  //   (D3D11_INPUT_ELEMENT_DESC){"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
  //   (D3D11_INPUT_ELEMENT_DESC){"COLOR",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
  // };
  // ID3D11Device_CreateInputLayout(rx.Device,LayoutElements,ARRAYSIZE(LayoutElements),
  //   rx_vs_shader_bytecode,sizeof(rx_vs_shader_bytecode),&rx.VertexShaderInputLayout);
  // ID3D11Device_CreateVertexShader(rx.Device,
  //   rx_vs_shader_bytecode,sizeof(rx_vs_shader_bytecode),NULL,&rx.VertexShader);
  // ID3D11Device_CreatePixelShader(rx.Device,
  //   rx_ps_shader_bytecode,sizeof(rx_ps_shader_bytecode),NULL,&rx.PixelShader);

  D3D11_SAMPLER_DESC SamplerInfo;
  ZeroMemory(&SamplerInfo,sizeof(SamplerInfo));
  SamplerInfo.AddressU=D3D11_TEXTURE_ADDRESS_WRAP;
  SamplerInfo.AddressV=D3D11_TEXTURE_ADDRESS_WRAP;
  SamplerInfo.AddressW=D3D11_TEXTURE_ADDRESS_WRAP;

  SamplerInfo.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  ID3D11Device_CreateSamplerState(rx.Device,&SamplerInfo,&rx.linear_sampler.unknown);

  SamplerInfo.Filter=D3D11_FILTER_MIN_MAG_MIP_POINT;
  ID3D11Device_CreateSamplerState(rx.Device,&SamplerInfo,&rx.point_sampler.unknown);

  rxresize_uniformbuffer(sizeof(rxuniform_t));
  rx. index_buffer= rxcreate_index_buffer( sizeof(rxindex_t), RX_INDEX_BUFFER_SIZE);
  rx.vertex_buffer=rxcreate_vertex_buffer(sizeof(rxvertex_t),RX_VERTEX_BUFFER_SIZE);

  rxdefault_render_pass();

  // note: configure the default state

  rxdriver_stage_viewport((float)(rx.size_x),(float)(rx.size_y));

  unsigned int Stride=sizeof(rxvertex_t);
  unsigned int Offset=0;
  ID3D11DeviceContext_RSSetState(rx.Context,rx.RasterizerState);
  ID3D11DeviceContext_OMSetBlendState(rx.Context,rx.BlendState,0x00,0xFFFFFFFu);
  ID3D11DeviceContext_OMSetDepthStencilState(rx.Context,rx.DepthBufferState,1);

  // rxdriver_stage_render_target(rx.offscreen_buffer);

  // ID3D11DeviceContext_IASetInputLayout(rx.Context,rx.VertexShaderInputLayout);

  ID3D11DeviceContext_IASetVertexBuffers(rx.Context,0,1,
    (ID3D11Buffer**)&rx.vertex_buffer.unknown,&Stride,&Offset);

  ID3D11DeviceContext_IASetIndexBuffer(rx.Context,
    (ID3D11Buffer*)rx.index_buffer.unknown,DXGI_FORMAT_R32_UINT,0);

  ID3D11DeviceContext_IASetPrimitiveTopology(rx.Context,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  ID3D11DeviceContext_VSSetConstantBuffers(rx.Context,0,1,
    (ID3D11Buffer**)&rx.uniform_buffer.unknown);

  // ID3D11DeviceContext_VSSetShader(rx.Context,rx.VertexShader,0x00,0);

  ID3D11DeviceContext_VSSetSamplers(rx.Context,0,1,&rx.linear_sampler.unknown);

  ID3D11DeviceContext_PSSetConstantBuffers(rx.Context,0,1,
    (ID3D11Buffer**)&rx.uniform_buffer.unknown);

  // ID3D11DeviceContext_PSSetShader(rx.Context,rx.PixelShader,0x00,0);

  ID3D11DeviceContext_PSSetSamplers(rx.Context,0,1,&rx.linear_sampler.unknown);

  D3D11_RECT RootClip;
  RootClip.left  =0;
  RootClip.top   =0;
  RootClip.right =0xffffff;
  RootClip.bottom=0xffffff;
  ID3D11DeviceContext_RSSetScissorRects(rx.Context,1,&RootClip);

  rxclear(.2f,.2f,.2f,1.f);

  rx.white=rxcreate_texture(16,16,rxRGBA8888);

  rxborrowed_t t=rxborrow_texture(rx.white);
  memset(t.memory,0xff,t.stride*rx.white.size_y);
  rxreturn(t);

  rx.start_ticks=ccclocktick();
  rx.frame_ticks=rx.start_ticks;
  rxtime();

  rxinit_default_font();


  // note: shader reloading
  {
    rx.LiveReloadDirectory=CreateFileA(".",
      FILE_LIST_DIRECTORY,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,
        OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,NULL);
    rx.LiveReloadEvent=CreateEvent(NULL,FALSE,0,NULL);
  }

  rxpull_live_reload_changes();
}

void load_bitmap8(unsigned char *memory, int stride, unsigned char *source)
{
  for(int y=0; y<16; ++y)
  { unsigned short bitrow=*source++;

    rxcolor_t *color=(rxcolor_t*)memory;
    for(int x=0; x<8; ++x)
    { unsigned char bitpixel=255 * (bitrow & 0x1);
      color->r = bitpixel;
      color->g = bitpixel;
      color->b = bitpixel;
      color->a = bitpixel;
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

  rxtexture_t texture=rxcreate_texture(512,256,rxRGBA8888);

  rxborrowed_t b=rxborrow_texture(texture);

            int  stride = b.stride;
  unsigned char *memory = b.memory;

  int index;
  for(index=0; index<ccCarrlenL(encoded); ++index)
  { int xcursor=index % 32 * 16;
    int ycursor=index / 32 * 16;

    unsigned char *write;

    write=memory+stride*ycursor+xcursor*sizeof(rxcolor_t);

    load_bitmap8(write,stride,encoded[index]);

    rxpoint16_t p;
    p.x=(short)xcursor;
    p.y=(short)ycursor;

    rx.font_glyph[index]=p;
  }

  rxreturn(b);

  rx.font_atlas=texture;
  rx.font_ysize=16;
  rx.font_xsize= 6;
}

rxvector3_t rxvector_xyz(float x, float y, float z)
{
  rxvector3_t r;
  r.x = x;
  r.y = y;
  r.z = z;
  return r;
}

rxvector3_t rxvector_xy(float x, float y)
{
  return rxvector_xyz(x,y,0);
}

rxvector3_t rxvector_x(float x)
{
  return rxvector_xyz(x,0,0);
}

rxvector3_t rxvector_y(float y)
{
  return rxvector_xyz(0,y,0);
}

rxvector3_t rxvector_z(float z)
{
  return rxvector_xyz(0,0,z);
}

rxvector3_t rxvector(float xyz)
{
  return rxvector_xyz(xyz,xyz,xyz);
}

float rxvector_dot(rxvector3_t a, rxvector3_t b)
{
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

rxvector3_t rxvector_cross(rxvector3_t a, rxvector3_t b)
{
  // note: this is how I memorize the cross product formula, think of rotations around an axis.
  return rxvector_xyz
    ( a.y*b.z - a.z*b.y,    // x-axis -> z/y plane
      a.z*b.x - a.x*b.z,    // y-axis -> x/z plane
      a.x*b.y - a.y*b.x );  // z-axis -> y/x plane
}

float rxvector_length(rxvector3_t a)
{
  return sqrtf(rxvector_dot(a,a));
}

rxvector3_t rxvector_add(rxvector3_t a, rxvector3_t b)
{
  rxvector3_t r;
  r.x = a.x+b.x;
  r.y = a.y+b.y;
  r.z = a.z+b.z;
  return r;
}

rxvector3_t rxvector_sub(rxvector3_t a, rxvector3_t b)
{
  rxvector3_t r;
  r.x = a.x-b.x;
  r.y = a.y-b.y;
  r.z = a.z-b.z;
  return r;
}

rxvector3_t rxvector_mul(rxvector3_t a, rxvector3_t b)
{
  rxvector3_t r;
  r.x = a.x*b.x;
  r.y = a.y*b.y;
  r.z = a.z*b.z;
  return r;
}

rxvector3_t rxvector_downscale(rxvector3_t a, float b)
{
  rxvector3_t r;
  r.x = a.x/b;
  r.y = a.y/b;
  r.z = a.z/b;
  return r;
}

rxvector3_t rxvector_scale(rxvector3_t a, float b)
{
  rxvector3_t r;
  r.x = a.x*b;
  r.y = a.y*b;
  r.z = a.z*b;
  return r;
}

rxvector3_t rxvector_negate(rxvector3_t a)
{
  rxvector3_t r;
  r.x = - a.x;
  r.y = - a.y;
  r.z = - a.z;
  return r;
}

rxvector3_t rxvector_min(rxvector3_t a, float min)
{
  rxvector3_t r;
  r.x = a.x > min ? min : a.x;
  r.y = a.y > min ? min : a.y;
  r.z = a.z > min ? min : a.z;
  return r;
}

rxvector3_t rxvector_max(rxvector3_t a, float max)
{
  rxvector3_t r;
  r.x = a.x < max ? max : a.x;
  r.y = a.y < max ? max : a.y;
  r.z = a.z < max ? max : a.z;
  return r;
}

rxvector3_t rxvector_abs(rxvector3_t a)
{
  rxvector3_t r;
  r.x = a.x < 0 ? - a.x : a.x;
  r.y = a.y < 0 ? - a.y : a.y;
  r.z = a.z < 0 ? - a.z : a.z;
  return r;
}


rxvector3_t rxvector_normalize(rxvector3_t a)
{
  float length=rxvector_length(a);

  if(length != 0)
    a=rxvector_scale(a,1/length);

  return a;
}

rxmatrix_t rxmatrix_identity()
{
  rxmatrix_t r;
  r.m[0][0]=1.f;r.m[1][0]=0.f;r.m[2][0]=0.f;r.m[3][0]=0.f;
  r.m[0][1]=0.f;r.m[1][1]=1.f;r.m[2][1]=0.f;r.m[3][1]=0.f;
  r.m[0][2]=0.f;r.m[1][2]=0.f;r.m[2][2]=1.f;r.m[3][2]=0.f;
  r.m[0][3]=0.f;r.m[1][3]=0.f;r.m[2][3]=0.f;r.m[3][3]=1.f;
  return r;
}

// todo!!: remove loop
rxmatrix_t rxmatrix_multiply(rxmatrix_t a, rxmatrix_t b)
{ rxmatrix_t result;
  for(int r=0; r<4; ++r)
  { for(int c=0; c<4; ++c)
    {  result.m[r][c] =
        (a.m[r][0]*b.m[0][c]) +
        (a.m[r][1]*b.m[1][c]) +
        (a.m[r][2]*b.m[2][c]) +
        (a.m[r][3]*b.m[3][c]);
    }
  }
  return result;
}

// todo!!: this could be made faster, just usind perp!
rxmatrix_t rxmatrix_rotZ(float angle)
{ rxmatrix_t result=rxmatrix_identity();
  float cosres=cosf(angle);
  float sinres=sinf(angle);
  result.m[0][0]=+cosres;
  result.m[0][1]=+sinres;
  result.m[1][0]=-sinres;
  result.m[1][1]=+cosres;
  return result;
}

rxmatrix_t rxmatrix_translate_xyz(float x, float y, float z)
{ rxmatrix_t result=rxmatrix_identity();
  result.m[3][0]=x;
  result.m[3][1]=y;
  result.m[3][2]=z;
  return result;
}

rxmatrix_t rxmatrix_flip_vertically()
{ rxmatrix_t result=rxmatrix_identity();
  result.m[1][1]=-1.f;
  return result;
}
#endif
