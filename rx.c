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
** erkkah/tigr or raysan5/raylib or virtually any other library that isn't
** this one.
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
//
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
#endif//_RX_STANDALONE

#include  "rxps.hlsl"
#include  "rxvs.hlsl"

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
#ifndef RX_COMMAND_BUFFER_SIZE
#define RX_COMMAND_BUFFER_SIZE 0x10000
# endif//RX_COMMAND_BUFFER_SIZE
#ifndef RX_INDEX_BUFFER_SIZE
#define RX_INDEX_BUFFER_SIZE 0x10000*8
# endif//RX_INDEX_BUFFER_SIZE
#ifndef RX_VERTEX_BUFFER_SIZE
#define RX_VERTEX_BUFFER_SIZE 0x10000*8
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

typedef enum rx_k
{
         rx_kINVALID,

  rx_kCOMPUTE_SHADER,
    rx_kPIXEL_SHADER,
   rx_kVERTEX_SHADER,
  rx_kSAMPLER,
  rx_kTEXTURE,
  rx_kMODE2D,
  rx_kMODE3D,
  rx_kENABLE_DEPTH_TESTING,
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
  // rx_kCLEAR,
  rx_kUNIFORM,
  /* Breaking changes */
  rx_kMATRIX,
  rx_kINDEXED,
} rx_k;

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
  rx_kLBUTTON,
  rx_kRBUTTON,
  rx_kMBUTTON,
  rx_kMVWHEEL,
  rx_kMHWHEEL,

  rx_kKEY_SPACE = ' ',
  rx_kKEY_A     = 'A',
  rx_kKEY_Z     = 'Z',
  rx_kKEY_0     = '0',
  rx_kKEY_9     = '9',
};

typedef ID3D11DeviceChild *rxunknown_t;

#include "rxobject.h"

typedef struct rxblobber_t rxblobber_t;
typedef struct rxblobber_t
{
  ID3DBlob    *unknown;

  void        *memory;
  size_t       length;
} rxblobber_t;

// note: is this is good name?
typedef struct rxborrowed_t rxborrowed_t;
typedef struct rxborrowed_t
{ ID3D11Resource * resource;
            void * memory;
  union
  {          int   stride;
             int   length;
  };
} rxborrowed_t;

#include "rx.texture.h"
#include "rx.sampler.h"
#include "rx.shader.h"
#include "rx.buffer.h"

// sharpness: determines the harshness or sharpness of the penumbras, the greater the sharper,
// the smaller the softer.
typedef struct rxcandle_t rxcandle_t;
typedef struct rxcandle_t
{ rxvec3_t           xyz;
  rxvec3_t     direction;
  rxvec3_t         color;
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

#include "rx.command.h"

// todo!!: please forgive me!
typedef struct rx_t rx_t;
typedef struct rx_t
{
  HANDLE DefaultCursor;

  // note:
  HANDLE   LiveReloadDirectory;
  HANDLE   LiveReloadEvent;
  void    *ControlFiber;
  void    *MessageFiber;



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
  rxvec2i16_t     font_glyph[95];
  float           font_ysize;
  float           font_xsize;

  // note: these are cc-hash-tables
  rxarticle_t       *instance_table;
  rxterminal_t      *contents_table;

  /* These are only used for the matrix stack and not accessible through the
    command interface directly nor affect the matrix used for rendering however,
    the actual matrix that is at render time is accessible through the command interface
    directly - XXX - the one called rj */
  rxmatrix_t   view_matrix;
  rxmatrix_t  world_matrix;

  // rxmatrix_t  matrix;

  rxuniform_buffer_t             uniform_buffer;
   rxvertex_buffer_t              vertex_buffer;
    rxindex_buffer_t               index_buffer;

  rxborrowed_t                    vertex_buffer_writeonly;
    rxvertex_t                    *vertex_array;
           int                     vertex_tally;

  rxborrowed_t                      index_buffer_writeonly;
     rxindex_t                     *index_array;
           int                      index_tally;

   rxcom_t                    command_array[RX_COMMAND_BUFFER_SIZE];
           int                    command_tally;

  rxcom_t  *                        command;
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




  ID3D11InfoQueue         *InfoQueue;
  ID3D11Device            *Device;
  ID3D11DeviceContext     *Context;

  IDXGISwapChain2         *SwapChain;
  void                    *FrameAwait;

  ID3D11DepthStencilState  *the_stencil_state_on;
  ID3D11DepthStencilState  *the_stencil_state_off;
  ID3D11DepthStencilState  *the_stencil_state;
  ID3D11Texture2D          *the_stencil_texture;
  ID3D11DepthStencilView   *the_stencil_view;

  ID3D11RasterizerState    *RasterizerState;
  ID3D11BlendState         *BlendState;

  rxshader_t default_vertex_shader;
  rxshader_t  default_pixel_shader;

  rxsampler_t        linear_sampler;
  rxsampler_t         point_sampler;

  rxtexture_t    white;

  int enable_depth_testing;
  int enable_depth_stencil;
  int enable_color_blending;

  /* input handling members, these get updated once every tick */

  short    chr;
  /* prob find better way to store this - xxx rj */
  char     key_lst[0x100];
  char     key    [0x100];
  unsigned is_ctrl: 1;
  unsigned is_menu: 1;
  unsigned is_shft: 1;

  /* todo */
  union
  { struct
    { int xcursor;
      int ycursor;
      int yscroll;
      int xscroll;

      int  xclick;
      int  yclick;

      int btn_old;
      int btn_cur;
    };
    struct
    { int xcursor;
      int ycursor;
      int yscroll;
      int xscroll;

      int  xclick;
      int  yclick;

      int btn_old;
      int btn_cur;
    } mice[1];
  };
} rx_t;


/* the source of all evil is here */
ccglobal rx_t rx;


/* todo: rename */
#ifndef       WAS_DOWN
#define       WAS_DOWN(x) ((rx.btn_old & (1 << x)) != 0)
# endif
#ifndef        IS_DOWN
#define        IS_DOWN(x) ((rx.btn_cur & (1 << x)) != 0)
# endif
#ifndef IS_CLICK_LEAVE
#define IS_CLICK_LEAVE(x) !IS_DOWN(x) &&  WAS_DOWN(x)
# endif
#ifndef IS_CLICK_ENTER
#define IS_CLICK_ENTER(x)  IS_DOWN(x) && !WAS_DOWN(x)
# endif

int
rxisctrl()
{
  return rx.is_ctrl;
}

int
rxismenu()
{
	return rx.is_menu;
}

int
rxisshft()
{
  return rx.is_shft;
}

int
rxtstbtn(int x)
{
  return IS_DOWN(x);
}

int
rxtstkey(int x)
{
  return rx.key[x] != 0;
}

int rxchr()
{
  return rx.chr;
}

#include "rxobject.cc"

void rxinvalidate_contents(void)
{
  if(!rx.LiveReloadDirectory)
    return;

  FILE_NOTIFY_INFORMATION *Entry;
                     char *EntryCursor;

  ccglobal char   EntryBuffer[(sizeof(*Entry)+MAX_PATH) << 4];
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
      {
        for(EntryCursor=EntryBuffer;
              EntryCursor<EntryBuffer+sizeof(EntryBuffer);)
        { Entry=(FILE_NOTIFY_INFORMATION*)(EntryCursor);

          char FileName[MAX_PATH+4];
          memset(FileName,0,sizeof(FileName));

          if(WideCharToMultiByte(CP_UTF8,0,Entry->FileName,Entry->FileNameLength,
              FileName,sizeof(FileName),NULL,FALSE))
          {
            rxlinker_labelsadd_terminal(FileName,rxlabel_kINVALIDATED);
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

int
rxunknown_typeof_compute_shader(rxunknown_t unknown)
{
  ID3D11ComputeShader *I=NULL;
  if(SUCCEEDED(IUnknown_QueryInterface(unknown,&IID_ID3D11ComputeShader,&I)))
    ID3D11DeviceChild_Release(I);
  return I!=NULL;
}

int
rxunknown_typeof_vertex_shader(rxunknown_t unknown)
{
  ID3D11VertexShader *I=NULL;
  if(SUCCEEDED(IUnknown_QueryInterface(unknown,&IID_ID3D11VertexShader,&I)))
    ID3D11DeviceChild_Release(I);
  return I!=NULL;
}

int
rxunknown_typeof_pixel_shader(rxunknown_t unknown)
{
  ID3D11PixelShader *I=NULL;
  if(SUCCEEDED(IUnknown_QueryInterface(unknown,&IID_ID3D11PixelShader,&I)))
    ID3D11DeviceChild_Release(I);
  return I!=NULL;
}

int
rxshader_typeof_compute(rxshader_t shader)
{
  return rxunknown_typeof_compute_shader(shader.unknown);
}

int
rxshader_typeof_vertex(rxshader_t shader)
{
  return rxunknown_typeof_vertex_shader(shader.unknown);
}

int
rxshader_typeof_pixel(rxshader_t shader)
{
  return rxunknown_typeof_pixel_shader(shader.unknown);
}

void
rxdelete_blobbler(
  rxblobber_t bytecode)
{
  if(bytecode.unknown)
  {
    ID3D11DeviceChild_Release(bytecode.unknown);
  }
}

rxblobber_t
rxcompile_shader_bytecode(
  unsigned int   length,
          void * memory,
    const char *  entry,
    const char *  model,
    const char * master)
{
  rxblobber_t blobber=(rxblobber_t){0};

  if(length != 0 && memory != 0)
  {
    ID3DBlob *BytecodeBlob,*MessagesBlob;

    if(SUCCEEDED(
        D3DCompile(memory,length,master,0,0,entry,model,
          RX_SHADER_COMPILATION_FLAGS,0,&BytecodeBlob,&MessagesBlob)))
    {
      blobber.unknown=BytecodeBlob;
      blobber. memory=BytecodeBlob->lpVtbl->GetBufferPointer(BytecodeBlob);
      blobber. length=BytecodeBlob->lpVtbl->   GetBufferSize(BytecodeBlob);

      ccdebuglog("'%s': compiled shader",master);
    } else
    {
      ccprintf("<!4%s!>\r\n",
        (char*)(MessagesBlob->lpVtbl->GetBufferPointer(MessagesBlob)));
      cctracewar("'%s': there were compilation errors",master);
    }
  }

  return blobber;
}

rxshader_t
rxcreate_shader(
  rx_k type, size_t bytecode_length, void *bytecode_memory)
{
  ID3D11DeviceChild      *      Shader = NULL;
  ID3DBlob               *    BlobPart = NULL;
  ID3D11ShaderReflection *  Reflection = NULL;
  ID3D11InputLayout      * InputLayout = NULL;

  if(type==rx_kVERTEX_SHADER)
  { if(FAILED(ID3D11Device_CreateVertexShader(rx.Device,
        bytecode_memory,bytecode_length,NULL,(ID3D11VertexShader**)&Shader)))
    {
      cctracewar("create_vertex_shader::error");
      goto leave;
    }
  } else
  if(type==rx_kPIXEL_SHADER)
  { if(FAILED(ID3D11Device_CreatePixelShader(rx.Device,
        bytecode_memory,bytecode_length,NULL,(ID3D11PixelShader**)&Shader)))
    {
      cctracewar("create_pixel_shader::error");
      goto leave;
    }
  }

  if(FAILED(
      D3DGetBlobPart(bytecode_memory,bytecode_length,
        D3D_BLOB_INPUT_SIGNATURE_BLOB,0,&BlobPart)))
  {
    cctracewar("extract_input_signature::error");
    goto leave;
  }

  if(FAILED(
      D3DReflect(bytecode_memory,bytecode_length,
        &IID_ID3D11ShaderReflection,(void**)&Reflection)))
  {
    cctracewar("reflection_interface::error");
    goto leave;
  }

  D3D11_SHADER_DESC ShaderInfo;
  Reflection->lpVtbl->GetDesc(Reflection,&ShaderInfo);

  D3D11_SIGNATURE_PARAMETER_DESC ElemSig;

  // todo: what's the max size of an element array?
  D3D11_INPUT_ELEMENT_DESC ElementArray[0x20];

  for(int ElementIndex=0;ElementIndex<ShaderInfo.InputParameters;++ElementIndex)
  {
    Reflection->lpVtbl->GetInputParameterDesc(Reflection,ElementIndex,&ElemSig);

    D3D11_INPUT_ELEMENT_DESC *Element=ElementArray+ElementIndex;
    Element-> SemanticName=ElemSig.SemanticName;
    Element->SemanticIndex=ElemSig.SemanticIndex;

    /* This is something that we have to revisit, for instance, say I wanted to
     support RGBA color as 4 chars instead of four floats for greater memory efficiency,
     there's not a straight forward way to know the mapping of the vertex layout we're
     using and map it to the layout of the input signature
     - XXX - the one called rj */
    if(ElemSig.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
    {
      switch(ElemSig.Mask)
      { case 0b0001: Element->Format=0;                              break;
        case 0b0011: Element->Format=DXGI_FORMAT_R32G32_FLOAT;       break;
        case 0b0111: Element->Format=0;                              break;
        case 0b1111: Element->Format=DXGI_FORMAT_R32G32B32A32_FLOAT; break;
        default:
          ccassert(!"not implemented");
      }
    } else
      ccassert(!"not implemented");

    Element->           InputSlot=0;
    Element->   AlignedByteOffset=D3D11_APPEND_ALIGNED_ELEMENT;
    Element->      InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA;
    Element->InstanceDataStepRate=0;
  }

  if(FAILED(
      ID3D11Device_CreateInputLayout(rx.Device,
        ElementArray,ShaderInfo.InputParameters,
          bytecode_memory,bytecode_length,&InputLayout)))
  {
    cctracewar("create_input_layout::error");
    goto leave;
  }

  rxarticle_attach(
    rxarticle_create(Shader),rxlinkage_kVERTEX_LAYOUT,InputLayout);

  // ID3D11DeviceChild_SetPrivateData(Shader,&IID_ID3D11InputLayout,sizeof(InputLayout),&InputLayout);

leave:
  return (rxshader_t){(rxunknown_t)(Shader)};
}

rxshader_t
rxload_vertex_shader(
  const char *entry, const char *master)
{
  // todo!!: we should get this from the device!

  char *model="vs_5_0";

  ccassert(master != 0);
  ccassert( entry != 0);
  ccassert( model != 0);

  rxshader_t r=(rxshader_t){(rxunknown_t)(ccnull)};

  rxterminal_t *c=rxlinker_resolve_terminal(master);

  if((c != 0) && (c->labels & rxlabel_kLOADED) && (~c->labels & rxlabel_kERRONEOUS))
  {
    ccassert(c->length != 0);
    ccassert(c->memory != 0);

    rxblobber_t b=rxcompile_shader_bytecode(c->length,c->memory,entry,model,master);

    c->labels|=rxlabel_kERRONEOUS;

    if((b.memory != 0) && (b.length != 0))
    {
      r=rxcreate_shader(rx_kVERTEX_SHADER,b.length,b.memory);

      rxdelete_blobbler(b);

      c->labels^=rxlabel_kERRONEOUS;
    }
  }
  return r;
}

rxshader_t
rxload_pixel_shader(
  const char *entry, const char *master)
{
  // todo!!: we should get this from the device!

  char *model="ps_5_0";

  ccassert(master != 0);
  ccassert( entry != 0);
  ccassert( model != 0);

  rxshader_t r=(rxshader_t){(rxunknown_t)(ccnull)};

  rxterminal_t *c=rxlinker_resolve_terminal(master);

  if((c != 0) && (c->labels & rxlabel_kLOADED) && (~c->labels & rxlabel_kERRONEOUS))
  {
    ccassert(c->length != 0);
    ccassert(c->memory != 0);

    rxblobber_t b=rxcompile_shader_bytecode(c->length,c->memory,entry,model,master);

    c->labels|=rxlabel_kERRONEOUS;

    if((b.memory != 0) && (b.length != 0))
    {
      r=rxcreate_shader(rx_kPIXEL_SHADER,b.length,b.memory);

      rxdelete_blobbler(b);

      c->labels^=rxlabel_kERRONEOUS;
    }
  }
  return r;
}

#include "rx.shader.cc"

void rxdelete_unknown(rxunknown_t unknown)
{
  if(unknown != 0)
    unknown->lpVtbl->Release(unknown);
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

#define _RXUNIFORM_BUFFER_IMPLEMENTATION
#include "rx.buffer.cc"

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


#include "rx.command.cc"

void rxdraw_end()
{
  rxcomadd(rx_kENDPASS);
}

void rxqueue_custom_command(rxcustom_t custom)
{
  rxcom_t *draw=rxcomadd(rx_kCUSTOM);
  draw->custom=custom;
}

// todo: properly integrate this!
void rxdraw_matrix(rxmatrix_t matrix)
{
  rxcom_t *draw=rxcomadd(rx_kMATRIX);
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


void rxqueue_shader_command(rxshader_t shader)
{
  rxcom_t *draw=rxcomadd(rx_kPUSHSHADER);
  draw->shader=shader;
}

void rxqueue_uniform_command(rxuniform_buffer_t buffer)
{
  rxcom_t *draw=rxcomadd(rx_kUNIFORM);
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
  rx.vertex_color=RX_COLOR_BLACK;
  rx.command=rxcomadd(rx_kINDEXED);
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

void rxsubmit_vertex_array(rxvertex_t *array, int tally)
{
  memcpy(
   &rx.vertex_array[
    rx.vertex_tally],array,sizeof(*array)*tally);

  rx.vertex_tally += tally;
}

void rxsubmit_vertex(rxvertex_t vertex)
{
  rx.vertex_array[
  rx.vertex_tally] = vertex;

  rx.vertex_tally += 1;
}

void rxsubmit_vertex_ex(
  float x, float y, float z, float w, float xcoord, float ycoord, rxcolor_t rgba)
{
  ccassert(rx.vertex_modeon != 0);
  ccassert(rx.command       != 0);

  rxvertex_t vertex;
  vertex.x=x;
  vertex.y=y;
  vertex.z=z;
  vertex.w=w;

  vertex.u=xcoord;
  vertex.v=ycoord;

  vertex.rgba=rgba;

  rx.vertex_array[rx.vertex_tally++]=vertex;
}

void rx_vertex_xyz(float x, float y, float z)
{
  rxsubmit_vertex_ex(x,y,z,1.,rx.vertex_xtexel,rx.vertex_ytexel,rx.vertex_color);
}

void rx_vertex_xyz_color(float x, float y, float z, rxcolor_t color)
{
  rxsubmit_vertex_ex(x,y,z,1.,rx.vertex_xtexel,rx.vertex_ytexel,color);
}

void rxsubmit_vertex_xy(float x, float y)
{
  rxsubmit_vertex_ex(x,y,10,1.,rx.vertex_xtexel,rx.vertex_ytexel,rx.vertex_color);
}

void rxsubmit_vertex_tex(float x, float y, float xcoord, float ycoord)
{
  rxsubmit_vertex_ex(x,y,1.,1.,xcoord,ycoord,rx.vertex_color);
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

  rxcomtag(label);
  rxtexture_bind(texture);

  rxcomtag(label);
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
  rxdraw_skinned_unnormalized(texture,RX_COLOR_WHITE,x,y,w,h,0,0,1,1);
}

void rxtexture_scaled(rxtexture_t texture, float x, float y, float scale_x, float scale_y)
{
  rxdraw_skinned_unnormalized(texture,RX_COLOR_WHITE,x,y,
    texture.size_x * scale_x,
    texture.size_y * scale_y,0,0,1,1);
}

void rxdraw_rect(rxcolor_t color, float x, float y, float w, float h)
{
  rxsampler_bind(rx.point_sampler);
  rxdraw_skinned_unnormalized(rx.white,color,x,y,w,h,0,0,1,1);
}

// todo!!: this is disgusting!
void rxdraw_circle(rxcolor_t color, float x, float y, float r)
{
  rxtexture_bind(rx.white);

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
  float xdist=x1-x0;
  float ydist=y1-y0;
  float length=sqrtf(xdist*xdist + ydist*ydist);

  float xnormal=.5f * thickness * -ydist/length;
  float ynormal=.5f * thickness * +xdist/length;

  rxtexture_bind(rx.white);
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
/* todo: this has to be re-visited for sub-pixel rendering */
void rxdraw_outline(rxcolor_t color, float x, float y, float w, float h)
{
  const char *label=rx.command_name;
  rx.command_name=ccnull;

  rxcomtag(label);
  rxdraw_rect(color,x-.5,y+h-.5,w+.5,1.);

  rxcomtag(label);
  rxdraw_rect(color,x-.5,y+0-.5,w+.5,1.);

  rxcomtag(label);
  rxdraw_rect(color,x+0-.5,y-.5,1.,h+.5);

  rxcomtag(label);
  rxdraw_rect(color,x+w-.5,y-.5,1.,h+.5);
}

float rxdraw_text_length(float h, const char *string)
{
  float result = ccCstrlenS(string) * rx.font_xsize;
  result *= h/rx.font_ysize;
  return result;
}

/* remove */
float rxchrxsz(int ysize)
{
  return rx.font_xsize * (ysize / rx.font_ysize);
}

void
rxdraw_text_run(
  int x/*starting position x*/,
  int y/*starting position y*/,
  int h/* the raster height of the font in pixels*/,
  void *user,
  int (*callback)(/*return whether the continue the run */
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
  rxtexture_bind(rx.font_atlas);
  rxsampler_bind(rx.point_sampler);


  /* XXX we have to get back to this */
  float sample_xsize=rx.font_ysize*xnormalize;
  float sample_ysize=rx.font_ysize*ynormalize;
  float render_xsize=rx.font_ysize*render_scale;
  float render_ysize=rx.font_ysize*render_scale;

  float xadvance=rx.font_xsize*render_scale;

  /* begin immediate vertex mode */
  rxvertex_mode();

  int index;
  int  code;
  for( index =0; callback(user,index,&code,&rx.vertex_color/* we just pass in the vertex color directly */);
       index+=1 )
  {
    rxvec2i16_t baked = rx.font_glyph[code-32];

    float sample_x0=baked.x*xnormalize;
    float sample_y0=baked.y*ynormalize;
    float sample_x1=sample_x0+sample_xsize;
    float sample_y1=sample_y0+sample_ysize;
    float render_x1=render_x0+render_xsize;
    float render_y1=render_y0+render_ysize;

    rxsubmit_vertex_tex(render_x0,render_y0,sample_x0,sample_y1);
    rxsubmit_vertex_tex(render_x0,render_y1,sample_x0,sample_y0);
    rxsubmit_vertex_tex(render_x1,render_y1,sample_x1,sample_y0);
    rxsubmit_vertex_tex(render_x1,render_y0,sample_x1,sample_y1);
    rxsubmit_index(index*4+0); rxsubmit_index(index*4+1); rxsubmit_index(index*4+2);
    rxsubmit_index(index*4+0); rxsubmit_index(index*4+2); rxsubmit_index(index*4+3);

    render_x0+=xadvance;
  }

  rxvertex_mode_end();
}

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

  /* XXX this should restore the state */
  rxtexture_bind(rx.font_atlas);
  rxsampler_bind(rx.point_sampler);

  rxvertex_mode();
  rxvertex_color(color);

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
    rxsubmit_vertex_tex(render_x0,render_y0,sample_x0,sample_y1);
    rxsubmit_vertex_tex(render_x0,render_y1,sample_x0,sample_y0);
    rxsubmit_vertex_tex(render_x1,render_y1,sample_x1,sample_y0);
    rxsubmit_vertex_tex(render_x1,render_y0,sample_x1,sample_y1);
    rxsubmit_index(index*4+0); rxsubmit_index(index*4+1); rxsubmit_index(index*4+2);
    rxsubmit_index(index*4+0); rxsubmit_index(index*4+2); rxsubmit_index(index*4+3);

    render_x0+=xadvance;
  }

  rxvertex_mode_end();
}

void rxdraw_text(int x, int y, int h, const char *string)
{
  if(string != 0)
  {
    rxdraw_text_ex(RX_COLOR_WHITE,x,y,h,strlen(string),string);
  }
}

#include "rx.texture.cc"
#include "rx.sampler.cc"

rxrender_target_t
rxcreate_render_target(int w, int h, int f)
{
  rxrender_target_t the_render_target;

  the_render_target.texture = rxtexture_create_untyped(w,h,f,0,NULL,
    D3D11_USAGE_DEFAULT,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET,0,1,0);

  return the_render_target;
}

void rxrender_target_apply(rxrender_target_t target, float *ClearColor)
{
  rxarticle_t *article=cctblgetP(rx.instance_table,target.unknown);
  ccassert(ccerrnon());

  // todo!!: debug only
  ID3D11RenderTargetView *View[1];
  if(SUCCEEDED(IUnknown_QueryInterface(
      article->linkage[rxlinkage_kRENDER_TARGET_VIEW],&IID_ID3D11RenderTargetView,&View[0])))
  {
    if(ClearColor != 0)
    {
      ID3D11DeviceContext_ClearRenderTargetView(rx.Context,View[0],ClearColor);
    }

    /* Clear the depth buffer */
#ifdef _RX_ENABLE_DEPTH_STENCIL

    ccassert(rx.the_stencil_view != NULL);
    ID3D11DeviceContext_ClearDepthStencilView(
      rx.Context,rx.the_stencil_view,D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.0f,0);

#endif

    ID3D11DeviceContext_OMSetRenderTargets(rx.Context,ccCarrlenL(View),View,rx.the_stencil_view);
  } else
    ccassert(!"error");
}

void rxrender_target_copy(rxrender_target_t l, rxrender_target_t r)
{
  ID3D11Resource *Resource[2];
  Resource[0]=(ID3D11Resource*)(l.unknown);
  Resource[1]=(ID3D11Resource*)(r.unknown);

  // todo: debug only!
  if(SUCCEEDED(
      IUnknown_QueryInterface(l.unknown,&IID_ID3D11Resource,&Resource[0])))
  {
    // todo: debug only!
    if(SUCCEEDED(
        IUnknown_QueryInterface(r.unknown,&IID_ID3D11Resource,&Resource[1])))
    {
      ID3D11DeviceContext_CopyResource(rx.Context,Resource[0],Resource[1]);

      IUnknown_Release(Resource[1]);
    }

    IUnknown_Release(Resource[0]);
  }
}

// todo: update the view-port and frame buffer!



void rxtime()
{
  ccclocktick_t ticks=ccclocktick();

  rx.total_ticks=ticks-rx.start_ticks;
  rx.total_seconds=ccclocksecs(rx.total_ticks);

  rx.delta_ticks=ticks-rx.frame_ticks;
  rx.delta_seconds=ccclocksecs(rx.delta_ticks);

  rx.frame_ticks=ticks;
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
  float clear_color[]={.0f,.0f,.0f,1.f};

  rxrender_target_apply(rx.target,clear_color);

  rxshader_apply(rx.default_vertex_shader);
  rxuniform_buffer_bind_ex(rx.uniform_buffer,0);

  rxshader_apply(rx. default_pixel_shader);
  rxuniform_buffer_bind_ex(rx.uniform_buffer,0);

  rx.vertex_buffer_writeonly = rxborrow_vertex_buffer(rx.vertex_buffer);
  rx. index_buffer_writeonly =  rxborrow_index_buffer(rx. index_buffer);
  rx.           vertex_array=rx.vertex_buffer_writeonly.memory;
  rx.            index_array=rx. index_buffer_writeonly.memory;
  rx. vertex_tally=0;
  rx.  index_tally=0;
  rx.command_tally=0;

  // note: prepare our resources
  rx.shadow_buffer_writeonly = rxborrow_struct_buffer(rx.shadow_buffer);
  rx.candle_buffer_writeonly = rxborrow_struct_buffer(rx.candle_buffer);

  rx.shadow_array=rx.shadow_buffer_writeonly.memory;
  rx.candle_array=rx.candle_buffer_writeonly.memory;

  // todo!!!: the shader should take the number of effects and shadows!
  memset(rx.shadow_array,rx.shadow_threshold,sizeof(*rx.shadow_array)*rx.shadow_threshold);
  memset(rx.candle_array,rx.candle_threshold,sizeof(*rx.candle_array)*rx.candle_threshold);

  rx.shadow_tally = 0;
  rx.candle_tally = 0;

  rxdriver_stage_viewport((float)(rx.size_x),(float)(rx.size_y));

  unsigned int Stride=sizeof(rxvertex_t);
  unsigned int Offset=0;
  ID3D11DeviceContext_RSSetState(rx.Context,rx.RasterizerState);
  ID3D11DeviceContext_OMSetBlendState(rx.Context,rx.BlendState,0x00,0xFFFFFFFu);


  ID3D11DeviceContext_IASetVertexBuffers(rx.Context,0,1,
    (ID3D11Buffer**)&rx.vertex_buffer.unknown,&Stride,&Offset);
  ID3D11DeviceContext_IASetIndexBuffer(rx.Context,
    (ID3D11Buffer*)rx.index_buffer.unknown,DXGI_FORMAT_R32_UINT,0);
  // D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
  // D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
  ID3D11DeviceContext_IASetPrimitiveTopology(rx.Context,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  ID3D11DeviceContext_VSSetSamplers(rx.Context,0,1,&rx.linear_sampler.unknown);
  ID3D11DeviceContext_PSSetSamplers(rx.Context,0,1,&rx.linear_sampler.unknown);

  rx3d();
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
}


#include "rx.win32.cc"

int rxtick()
{
  rx.tick_count += 1;

  // rxpull_live_reload_changes();

  rxwindow_poll();

  SetCursor(rx.DefaultCursor);

  // todo: this is extremely unsafe and unpredictable, get something more robust!
  rxdraw_end();

  rxdraw_skinned_preset_unnormalized(
    RX_COLOR_WHITE,0,0,1,1,0,0,1,1);

  // todo: this is extremely unsafe and unpredictable, get something more robust!
  rxdraw_end();

  // note:
  rxdefault_render_pass_end();

  int index_offset=0;

  rxcom_t *command=rx.command_array;
  {

    while(command->kind!=rx_kENDPASS)
    {
      index_offset += rxcomexc(command,index_offset);
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
    rxrender_target_apply(target,clear_color);

    rxshader_apply(vertex_shader);
    rxshader_apply( pixel_shader);

    // todo!:
    ID3D11ShaderResourceView *ShaderResourceView[3];

    rxarticle_t *article=cctblgetP(rx.instance_table,input_target.unknown);
    ccassert(ccerrnon());

    IUnknown_QueryInterface(
      article->linkage[rxlinkage_kSHADER_RESOURCE_VIEW],&IID_ID3D11ShaderResourceView,&ShaderResourceView[0]);
    IUnknown_QueryInterface(
      shadow_buffer.unknown,&IID_ID3D11ShaderResourceView,&ShaderResourceView[1]);
    IUnknown_QueryInterface(
      candle_buffer.unknown,&IID_ID3D11ShaderResourceView,&ShaderResourceView[2]);

    ID3D11DeviceContext_PSSetShaderResources(rx.Context,0,ccCarrlenL(ShaderResourceView),ShaderResourceView);

    {
      while(command->kind!=rx_kENDPASS)
      {
        index_offset += rxcomexc(command,index_offset);
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
    rxrender_target_copy(rx.effect_target,rx.target);
  }

  // note: finally, move the post-process target to our screen buffer to display
  {
    rxrender_target_copy(rx.screen_target,rx.effect_target);
    // todo!!: replace with CopySubresource!
    // ID3D11DeviceContext_ResolveSubresource(rx.Context,
    //   OutputResource,0,InputResource,0,DXGI_FORMAT_R8G8B8A8_UNORM);
  }

  IDXGISwapChain_Present(rx.SwapChain,1u,0);

  // todo!:
  WaitForSingleObjectEx(rx.FrameAwait,INFINITE,TRUE);

  rxwindow_enable();

  rxrestore_render_stack();

  rxtime();
  return !rx.Quitted;
}

ID3D11DepthStencilState *
rxcreate_simple_stencil_state_d3d(
  int depth_enabled, int stencil_enabled)
{
  D3D11_DEPTH_STENCIL_DESC the_info;
  the_info.     DepthEnable=depth_enabled;
  the_info.   StencilEnable=stencil_enabled;
  the_info.  DepthWriteMask=D3D11_DEPTH_WRITE_MASK_ALL;
  the_info.       DepthFunc=D3D11_COMPARISON_LESS;
  the_info. StencilReadMask=D3D11_DEFAULT_STENCIL_READ_MASK;
  the_info.StencilWriteMask=D3D11_DEFAULT_STENCIL_WRITE_MASK;
  the_info.FrontFace.      StencilFailOp=D3D11_STENCIL_OP_KEEP;
  the_info.FrontFace. StencilDepthFailOp=D3D11_STENCIL_OP_DECR;
  the_info.FrontFace.      StencilPassOp=D3D11_STENCIL_OP_KEEP;
  the_info.FrontFace.        StencilFunc=D3D11_COMPARISON_ALWAYS;
  the_info. BackFace.      StencilFailOp=D3D11_STENCIL_OP_KEEP;
  the_info. BackFace. StencilDepthFailOp=D3D11_STENCIL_OP_DECR;
  the_info. BackFace.      StencilPassOp=D3D11_STENCIL_OP_KEEP;
  the_info. BackFace.        StencilFunc=D3D11_COMPARISON_ALWAYS;

  ID3D11DepthStencilState *the_depth_stencil_state = NULL;
  ID3D11Device_CreateDepthStencilState(rx.Device,&the_info,&the_depth_stencil_state);

  return the_depth_stencil_state;
}


void rxinit_default_font();

void rxinit(const wchar_t *window_title)
{
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

  rxwindow_create(window_title);

  rx.DefaultCursor=LoadCursorA(NULL,IDC_ARROW);
  SetCursor(rx.DefaultCursor);

  typedef BOOL WINAPI _YYY_(void);
  typedef BOOL WINAPI _XXX_(DPI_AWARENESS_CONTEXT);
  HMODULE User32 = LoadLibraryA("user32.dll");
  _XXX_ *XXX = (_XXX_ *) GetProcAddress(User32, "SetProcessDPIAwarenessContext");
  _YYY_ *YYY = (_YYY_ *) GetProcAddress(User32, "SetProcessDPIAware");

  if(XXX) XXX(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
  else
  if(YYY) YYY();

  FreeLibrary(User32);


  rxwindow_poll();

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

    rxarticle_attach(
      rxarticle_create(RenderBuffer),
        rxlinkage_kRENDER_TARGET_VIEW,RenderBufferView);
    // todo!:
    rx.screen_target. size_x=SwapchainInfo. Width;
    rx.screen_target. size_y=SwapchainInfo.Height;
    rx.screen_target. format=SwapchainInfo.Format;
    rx.screen_target.unknown=(rxunknown_t)RenderBuffer;

    // rx.screen_target.asinput=0;

    // todo?: make separate targets for these, these are intermediate targets, i guess?
    rx.offscreen_target=rxcreate_render_target(
      rx.screen_target.size_x,rx.screen_target.size_y,rx.screen_target.format);

    rx.effect_target=rxcreate_render_target(
      rx.screen_target.size_x,rx.screen_target.size_y,rx.screen_target.format);

    // todo: pre-compile!
    rx.effect_vertex_shader=rxload_vertex_shader("MainVS","light.hlsl");
    rx. effect_pixel_shader= rxload_pixel_shader("MainPS","light.hlsl");

    rx.default_vertex_shader=
      rxcreate_shader(rx_kVERTEX_SHADER,sizeof(rx_vs_shader_bytecode),(void*)rx_vs_shader_bytecode);
    rx.default_pixel_shader=
      rxcreate_shader(rx_kPIXEL_SHADER,sizeof(rx_ps_shader_bytecode),(void*)rx_ps_shader_bytecode);

    rx.uniform_buffer=rxuniform_buffer_create(sizeof(rxshader_builtin_uniform_t),NULL);
    rx.  index_buffer=  rxcreate_index_buffer(sizeof(rxindex_t)  ,RX_INDEX_BUFFER_SIZE);
    rx. vertex_buffer= rxcreate_vertex_buffer(sizeof(rxvertex_t) ,RX_VERTEX_BUFFER_SIZE);

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

#ifdef _RX_ENABLE_DEPTH_STENCIL
  rx.the_stencil_state_on  = rxcreate_simple_stencil_state_d3d(TRUE,TRUE);
  rx.the_stencil_state_off = rxcreate_simple_stencil_state_d3d(FALSE,FALSE);

  D3D11_TEXTURE2D_DESC the_stencil_texture_info;
  the_stencil_texture_info.         Width=(UINT)(rx.size_x);
  the_stencil_texture_info.        Height=(UINT)(rx.size_y);
  the_stencil_texture_info.     MipLevels=1;
  the_stencil_texture_info.     ArraySize=1;
  the_stencil_texture_info.        Format=DXGI_FORMAT_D32_FLOAT;
  the_stencil_texture_info.         Usage=D3D11_USAGE_DEFAULT;
  the_stencil_texture_info.     BindFlags=D3D11_BIND_DEPTH_STENCIL;
  the_stencil_texture_info.CPUAccessFlags=0;
  the_stencil_texture_info.     MiscFlags=0;
  the_stencil_texture_info.SampleDesc.  Count=1;
  the_stencil_texture_info.SampleDesc.Quality=0;
  ID3D11Device_CreateTexture2D(rx.Device,&the_stencil_texture_info,0x0,&rx.the_stencil_texture);

  ID3D11Device_CreateDepthStencilView(rx.Device,(ID3D11Resource*)(rx.the_stencil_texture),NULL,
    &rx.the_stencil_view);

  ccassert(rx.the_stencil_view != NULL);
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
  RasterizerInfo.        ScissorEnable=TRUE;
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

  rxrestore_render_stack();


  D3D11_RECT RootClip;
  RootClip.left  =0;
  RootClip.top   =0;
  RootClip.right =0xffffff;
  RootClip.bottom=0xffffff;
  ID3D11DeviceContext_RSSetScissorRects(rx.Context,1,&RootClip);

  rxclear(.2f,.2f,.2f,1.f);

  rx.white=rxtexture_create(16,16,rxRGBA8888);

  rxborrowed_t t=rxtexture_borrow(rx.white);
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

  // rxpull_live_reload_changes();
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

#pragma warning(pop)
#endif

