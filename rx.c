/*
**
** -+- rx -+-
**
** Copyright(C) Dayan Rodriguez, 2022, All rights reserved.
**
** Simple render API for when you just want to see something.
**
** - load and render images
** - draw crappy text
**
** rx is not production ready nor production quality.
**
** Much better libraries:
** erkkah/tigr
** raysan5/raylib
**
** Merge Notes:
** - Added vector types
** - Added default white texture for rect-drawing
** - Added 'local_texture', to be renamed?
** - 'load_texture' now takes in a 'local_texture'
** - Added 'rxcreate_texture_ex' to simplify 'create' and 'load'
** - Made constant buffer be dynamic to allow for updates
** - Renamed 'rxdraw_t' to 'rxcommand_t'
** - Now commands are more productive and specific
** - Added vertex mode, made text rendering much more efficient
*/
#ifndef _RX_H
#define _RX_H
#define _CRT_SECURE_NO_WARNINGS

#ifdef _RX_STANDALONE
# include "cc\cc.c"

# define STB_IMAGE_IMPLEMENTATION
# define STBI_MALLOC(size)          ccmalloc(size)
# define STBI_REALLOC(size,memory)  ccrealloc(size,memory)
# define STBI_FREE(memory)          ccfree(memory)
# include "stb_image.h"

# define STB_IMAGE_RESIZE_IMPLEMENTATION
# define STBIR_MALLOC(size,user)         ccmalloc(size)
# define STBIR_REALLOC(size,memory,user) ccrealloc(size,memory)
# define STBIR_FREE(memory,user)         ccfree(memory)
# include "stb_image_resize.h"

# define STB_IMAGE_WRITE_IMPLEMENTATION
# define STBIW_MALLOC(size)         ccmalloc(size)
# define STBIW_REALLOC(size,memory) ccrealloc(size,memory)
# define STBIW_FREE(memory)         ccfree(memory)
# include "stb_image_write.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_malloc(size,user)         ccmalloc(size)
#define STBTT_realloc(size,memory,user) ccrealloc(size,memory)
#define STBTT_free(memory,user)         ccfree(memory)
#include "stb_truetype.h"
#endif

#define CINTERFACE
#define COBJMACROS
#define D3D11_NO_HELPERS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define _NO_CRT_STDIO_INLINE
#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "user32")
#pragma comment(lib, "Winmm")
#pragma comment(lib, "Kernel32")
#pragma comment(lib, "Advapi32")
#pragma comment(lib, "Xinput")
#pragma comment(lib, "gdi32")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3dcompiler")
#include <windows.h>
#include <Windowsx.h>
#include <hidusage.h>
#include <memoryapi.h>
#include <xinput.h>
#include <timeapi.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <intrin.h>
#include <sysinfoapi.h>
#include <winuser.h>
#include <psapi.h>
#include <ws2tcpip.h>
#include <Winsock2.h>

// #pragma warning(disable:4115)
// #include <d3dcompiler.h>
#include <dxgidebug.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi1_3.h>

#include "rxps.hlsl"
#include "rxvs.hlsl"
#include "rxm.c"

#define rxPI_F 3.14159265358979323846f

#define rxrad2deg(r) ((r)/rxPI_F*180.f)
#define rxdeg2rad(d) ((d)/180.f*rxPI_F)

#define     rxRGB8 DXGI_FORMAT_R8_UNORM
#define rxRGBA8888 DXGI_FORMAT_R8G8B8A8_UNORM

typedef ID3D11DeviceChild   * rxunknown_t;
typedef ID3D11View          * rxbindview_t;
typedef ID3D11Resource      * rxresource_t;

typedef struct rxsampler_t rxsampler_t;
typedef struct rxsampler_t
{
  ID3D11SamplerState        * Sampler;
} rxsampler_t;

typedef struct rxlocal_texture_t rxlocal_texture_t;
typedef struct rxlocal_texture_t
{  int   size_x, size_y;
   int   format;
   int   stride;
  void * memory;
} rxlocal_texture_t;

typedef struct rxtexture_t rxtexture_t;
typedef struct rxtexture_t
{          int size_x, size_y;
           int format;
  rxbindview_t bindview;
} rxtexture_t;

// Note:
typedef struct rxborrowed_t rxborrowed_t;
typedef struct rxborrowed_t
{ rxresource_t resource;
         void *memory;
          int  stride;
} rxborrowed_t;

typedef struct rxcolor_t rxcolor_t;
typedef struct rxcolor_t
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} rxcolor_t;

#define rxcolor_kWHITE (rxcolor_t){0xff,0xff,0xff,0xff}
#define rxcolor_kRED   (rxcolor_t){0xff,0xff,0xff,0xff}
#define rxcolor_kGREEN (rxcolor_t){0xff,0xff,0xff,0xff}
#define rxcolor_kBLUE  (rxcolor_t){0xff,0xff,0xff,0xff}

typedef struct rxvertex_t rxvertex_t;
typedef struct rxvertex_t
{ float x, y;
  float u, v;
  unsigned char r,g,b,a;
} rxvertex_t;

typedef struct rxuniform_t rxuniform_t;
typedef struct rxuniform_t
{
  rxmatrix_t e;
} rxuniform_t;

typedef int rxindex_t;

typedef enum rxdraw_k
{ rxdraw_kCLIP,
  rxdraw_kINDEXED,
  rxdraw_kCLEAR,
  rxdraw_kMATRIX,
  rxdraw_kSAMPLER,
  rxdraw_kTEXTURE,
} rxdraw_k;

typedef struct rxcommand_t rxcommand_t;
typedef struct rxcommand_t
{    rxdraw_k  kind;
  rxunknown_t  unknown;
  rxuniform_t  uniform;
          int  offset;
          int  length;
          int  x,y,w,h;
} rxcommand_t;

typedef struct rx_t rx_t;
typedef struct rx_t
{ unsigned  Quitted:      1;
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

  int in_vertex_mode;

  rxcommand_t *command;

  float sample_xcoord;
  float sample_ycoord;

  int color_r;
  int color_g;
  int color_b;
  int color_a;

  // Todo:
  int window_events[0x100];

  rxtexture_t     font_atlas;
  stbtt_bakedchar font_glyph[256];
  float           font_height;

  void                   *  ControlFiber;
  void                   *  MessageFiber;
  ID3D11InfoQueue        *  InfoQueue;
  ID3D11Device           *  Device;
  ID3D11DeviceContext    *  Context;
  IDXGISwapChain2        *  SwapChain;
  void                   *  FrameAwait;

  ID3D11Texture2D        *  OffscreenBuffer;
  ID3D11RenderTargetView *  OffscreenBufferView;

  ID3D11Texture2D        *  RenderBuffer;
  ID3D11RenderTargetView *  RenderBufferView;

  ID3D11RasterizerState  *  RasterizerState;
  ID3D11BlendState       *  BlendState;
  ID3D11VertexShader     *  VertexShader;
  ID3D11PixelShader      *  PixelShader;

  ID3D11InputLayout *VertexShaderInputLayout;

  ID3D11Buffer *VertexBuffer;
  ID3D11Buffer *IndexBuffer;
  ID3D11Buffer *UniformBuffer;

  rxsampler_t linear_sampler;
  rxsampler_t point_sampler;

  rxuniform_t * uniform;

   rxvertex_t * vertex_buffer;
          int   vertex_buffer_index;

    rxindex_t * index_buffer;
          int   index_buffer_index;

  rxcommand_t   command_buffer[0x1000];
  int           command_buffer_index;

  ccclocktick_t start_ticks;
  ccclocktick_t frame_ticks;

  ccclocktick_t total_ticks;
  ccclocktick_t delta_ticks;

  ccf64_t total_seconds;
  ccf64_t delta_seconds;

  rxf32x4_t   clear_color;
  rxtexture_t white;
} rx_t;

static rx_t rx;

// Todo: support different formats
rxlocal_texture_t rxload_local_texture(const char *name)
{
  rxlocal_texture_t t;
  t.format=rxRGBA8888;
  t.memory=stbi_load(name,&t.size_x,&t.size_y,0,4);
  t.stride=t.size_x*4;

  return t;
}

// Todo: to be removed!
void *rxborrow_resource(void *resource)
{
#if 0
  // Note: we could do this for extra safety
  rxunknown_t unknown=(rxunknown_t)resource;
  ID3D11ShaderResourceView *Resource;
  IUnknown_QueryInterface(unknown,&IID_ID3D11ShaderResourceView,(void**)&Resource);
#endif

  D3D11_MAPPED_SUBRESOURCE m;
  ID3D11DeviceContext_Map(rx.Context,(ID3D11Resource*)resource,0,D3D11_MAP_WRITE_DISCARD,0,&m);

  ccassert(m.pData != 0);

  return m.pData;
}

void rxreturn_resource(void *resource)
{
  ID3D11DeviceContext_Unmap(rx.Context,(ID3D11Resource*)resource,0);
}

void rxreturn(rxborrowed_t borrowed)
{
  rxreturn_resource(borrowed.resource);
}

rxborrowed_t rxborrow_texture(rxtexture_t texture)
{
  ID3D11Resource *Resource;
  ID3D11View_GetResource(texture.bindview,&Resource);

  D3D11_MAPPED_SUBRESOURCE MappedAccess;
  ID3D11DeviceContext_Map(rx.Context,Resource,0,D3D11_MAP_WRITE_DISCARD,0,&MappedAccess);

  rxborrowed_t result;
  result.resource=Resource;
  result.  stride=MappedAccess.RowPitch;
  result.  memory=MappedAccess.pData;

  ccassert(result.stride != 0);
  ccassert(result.memory != 0);
  return result;
}

void rxdelete_texture(rxtexture_t texture)
{
  ID3D11Resource *Resource;
  ID3D11View_GetResource(texture.bindview,&Resource);
  ID3D11Resource_Release(Resource);
}

// Note:
// I keep thinking about splitting these types of functions into individual 'create' and 'bind',
// but what's the point if the resource has to be created with explicit bind flags.
// For that reason, I think it's best that a resource is at least partially typed or categorized by
// its bind attributes and any bind objects be created at initialization time.
// More sensitive resource should have their own, correspondingly sensitive functions that tailor
// to any peculiar attributes.
rxtexture_t
rxcreate_texture_ex(int w, int h, int f, int s, void *m)
{
  ccassert(w >= 1 ||
    cctraceerr("invalid width"));

  ccassert(h >= 1 ||
    cctraceerr("invalid height"));

  D3D11_TEXTURE2D_DESC TextureI;
  ZeroMemory(&TextureI,sizeof(TextureI));
  TextureI.         Width=(unsigned int)w;
  TextureI.        Height=(unsigned int)h;
  TextureI.     MipLevels=1;
  TextureI.     ArraySize=1;
  TextureI.        Format=f;
  TextureI.    SampleDesc.  Count=1;
  TextureI.    SampleDesc.Quality=0;
  TextureI.         Usage=D3D11_USAGE_DYNAMIC;
  TextureI.     MiscFlags=0;
  TextureI.     BindFlags=D3D11_BIND_SHADER_RESOURCE;
  TextureI.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;

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
  result.  size_x=w;
  result.  size_y=h;
  result.  format=f;
  result.bindview=(ID3D11View*)ShaderResourceView;
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

// Todo: legit font packer!
// Todo: we will need a dedicated shader for font rendering!
// Todo: support for multiple fonts!
// Todo: support for sdf fonts!
void rxload_font(const char *name)
{ void *file=ccopenfile(name,"r");
  void *data=ccpullfile(file,0,0);
  ccassert(file!=0);
  ccassert(data!=0);
  ccclosefile(file);

  rx.font_height=64;
  int w=512;
  int h=512;
  unsigned char *buffer=ccmalloc(w*h);
  stbtt_BakeFontBitmap(data,0,rx.font_height,buffer,w,h,32,96,rx.font_glyph);

  // stbi_write_png("last_atlas.png",w,h,1,buffer,w);

  rx.font_atlas=rxcreate_texture(w,h,rxRGBA8888);

  rxborrowed_t b=rxborrow_texture(rx.font_atlas);
  unsigned char *m=b.memory;
  for(int i=0;i<w*h;++i)
  { unsigned char c=buffer[i];
    m[i*4+0]=c;
    m[i*4+1]=c;
    m[i*4+2]=c;
    m[i*4+3]=c;
  }
  rxreturn(b);

  ccfree(buffer);
}

rxcommand_t *rxcommand(rxdraw_k kind)
{
  rxcommand_t *draw=rx.command_buffer+rx.command_buffer_index++;
  draw->kind=kind;

  return draw;
}

void rxclear(rxf32x4_t clear)
{
  rx.clear_color=clear;
}

void rxdraw_matrix(rxmatrix_t matrix)
{
  rxcommand_t *draw=rxcommand(rxdraw_kMATRIX);
  draw->uniform.e=matrix;
}

void rxdraw_sampler(rxsampler_t sampler)
{
  rxcommand_t *draw=rxcommand(rxdraw_kSAMPLER);
  draw->unknown=(rxunknown_t)sampler.Sampler;
}

void rxdraw_texture(rxtexture_t texture)
{
  rxcommand_t *draw=rxcommand(rxdraw_kTEXTURE);
  draw->unknown=(rxunknown_t)texture.bindview;
}

void rxvertex_mode()
{
  ccassert(!rx.in_vertex_mode);

  rx.sample_xcoord=0;
  rx.sample_ycoord=0;

  rx.color_r=0;
  rx.color_g=0;
  rx.color_b=0;
  rx.color_a=0;

  rx.in_vertex_mode=cctrue;

  rx.command=rxcommand(rxdraw_kINDEXED);
  rx.command->offset=rx.vertex_buffer_index;
  rx.command->length=0;
}

void rxindex(rxindex_t value)
{
  ccassert(rx.in_vertex_mode);
  ccassert(rx.command!=0);

  rxindex_t *index=rx.index_buffer+rx.index_buffer_index++;
  *index=value;

  rx.command->length++;
}

void rxvertex_color_rgba(int color_r, int color_g, int color_b, int color_a)
{
  rx.color_r=color_r;
  rx.color_g=color_g;
  rx.color_b=color_b;
  rx.color_a=color_a;
}

void rxvertex_color(rxcolor_t color)
{
  rx.color_r=color.r;
  rx.color_g=color.g;
  rx.color_b=color.b;
  rx.color_a=color.a;
}

void rxvertex_texture_coords(float sample_xcoord, float sample_ycoord)
{
  rx.sample_xcoord=sample_xcoord;
  rx.sample_ycoord=sample_ycoord;
}

void rxvertex_ex(float x, float y, float xcoord, float ycoord)
{
  ccassert(rx.in_vertex_mode);
  ccassert(rx.command!=0);

  rxvertex_t *vertex=rx.vertex_buffer+rx.vertex_buffer_index++;
  vertex->x=x;
  vertex->y=y;
  vertex->u=xcoord;
  vertex->v=ycoord;
  vertex->r=(unsigned char)rx.color_r;
  vertex->g=(unsigned char)rx.color_g;
  vertex->b=(unsigned char)rx.color_b;
  vertex->a=(unsigned char)rx.color_a;
}

void rxvertex(float x, float y)
{
  rxvertex_ex(x,y,rx.sample_xcoord,rx.sample_ycoord);
}

void rxvertex_mode_end()
{
  ccassert(rx.in_vertex_mode);

  rx.in_vertex_mode=ccfalse;
  rx.command=ccnull;
}

void rxdraw_skinned_unnormalized(
  rxtexture_t texture, rxcolor_t color, float x, float y, float w, float h, float tx, float ty, float tw, float th)
{
  rxdraw_texture(texture);

  rxvertex_mode();
    rxvertex_color(color);

    rxvertex_ex(x+0,y+0,tx   ,ty+th);
    rxvertex_ex(x+0,y+h,tx   ,ty   );
    rxvertex_ex(x+w,y+h,tx+tw,ty   );
    rxvertex_ex(x+w,y+0,tx+tw,ty+th);

    rxindex(0);
    rxindex(1);
    rxindex(2);
    rxindex(0);
    rxindex(2);
    rxindex(3);
  rxvertex_mode_end();
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
  rxcolor_t white=(rxcolor_t){0xff,0xff,0xff,0xff};
  rxdraw_skinned_unnormalized(texture,white,x,y,w,h,0,0,1,1);
}

void rxdraw_rect(rxcolor_t color, float x, float y, float w, float h)
{
  rxdraw_sampler(rx.point_sampler);
  rxdraw_skinned_unnormalized(rx.white,color,x,y,w,h,0,0,1,1);
}

void rxdraw_outline(rxcolor_t color, float x, float y, float w, float h)
{
  // Todo:!
  rxdraw_rect(color,x+0,y+h,w+2,2);
  rxdraw_rect(color,x+0,y+0,w+0,2);
  rxdraw_rect(color,x+w,y+0,2,h+0);
  rxdraw_rect(color,x+0,y+0,2,h+0);
}

void rxdraw_text(int x, int y, float h, const char *string)
{
  rxtexture_t texture=rx.font_atlas;

  float xnormalize=1.f/texture.size_x;
  float ynormalize=1.f/texture.size_y;
  float render_scale=h/rx.font_height;

  float ox=(float)x;
  float oy=(float)y;

  rxdraw_texture(texture);

  rxvertex_mode();
    rxvertex_color(rxcolor_kWHITE);

  for(int index=0; *string; ++index)
  {
    int code=*string++;

    stbtt_bakedchar baked=rx.font_glyph[code-32];

    float sample_x0=xnormalize*baked.x0;
    float sample_y0=ynormalize*baked.y0;
    float sample_x1=xnormalize*baked.x1;
    float sample_y1=ynormalize*baked.y1;

    float baseline_yoffset=baked.y1-baked.y0+baked.yoff;

    float render_x0=ox;
    float render_y0=oy-baseline_yoffset*render_scale;
    float render_x1=render_x0+(baked.x1-baked.x0)*render_scale;
    float render_y1=render_y0+(baked.y1-baked.y0)*render_scale;

    rxvertex_ex(render_x0,render_y0,sample_x0,sample_y1);
    rxvertex_ex(render_x0,render_y1,sample_x0,sample_y0);
    rxvertex_ex(render_x1,render_y1,sample_x1,sample_y0);
    rxvertex_ex(render_x1,render_y0,sample_x1,sample_y1);

    rxindex(index*4+0);
    rxindex(index*4+1);
    rxindex(index*4+2);
    rxindex(index*4+0);
    rxindex(index*4+2);
    rxindex(index*4+3);

    ox+=(float)baked.xadvance*render_scale;
  }

  rxvertex_mode_end();
}

// Todo: update the viewport and frame buffer!
void rxresize(int w, int h)
{
  RECT Client;
  Client.left=0;
  Client.top=0;
  Client.right=w;
  Client.bottom=h;
  AdjustWindowRect(&Client,WS_OVERLAPPEDWINDOW,FALSE);
  SetWindowPos(rx.Window,HWND_NOTOPMOST,0,0,Client.right,Client.bottom,SWP_NOMOVE|SWP_NOACTIVATE|SWP_FRAMECHANGED);
}

void rxresize_uniformbuffer(size_t size_in_bytes)
{
  if(rx.UniformBuffer)
  {
    ID3D11Buffer_Release(rx.UniformBuffer);
  }

  D3D11_BUFFER_DESC BufferI;
  BufferI.              Usage=D3D11_USAGE_DYNAMIC;
  BufferI.     CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
  BufferI.          MiscFlags=0;
  BufferI.StructureByteStride=0;
  BufferI.          BindFlags=D3D11_BIND_CONSTANT_BUFFER;
  BufferI.          ByteWidth=(UINT)size_in_bytes;

  ID3D11Device_CreateBuffer(rx.Device,&BufferI,NULL,&rx.UniformBuffer);
}

void rxresize_indexbuffer(size_t size_in_bytes)
{
  if(rx.IndexBuffer)
  {
    ID3D11Buffer_Release(rx.IndexBuffer);
  }

  D3D11_BUFFER_DESC i;
  i.              Usage=D3D11_USAGE_DYNAMIC;
  i.     CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
  i.          MiscFlags=0;
  i.StructureByteStride=0;
  i.          BindFlags=D3D11_BIND_INDEX_BUFFER;
  i.          ByteWidth=(UINT)size_in_bytes;
  ID3D11Device_CreateBuffer(rx.Device,&i,NULL,&rx.IndexBuffer);
}

void rxresize_vertexbuffer(size_t size_in_bytes)
{
  if(rx.VertexBuffer)
  {
    ID3D11Buffer_Release(rx.VertexBuffer);
  }

  D3D11_BUFFER_DESC i;
  i.              Usage=D3D11_USAGE_DYNAMIC;
  i.     CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
  i.          MiscFlags=0;
  i.StructureByteStride=0;
  i.          BindFlags=D3D11_BIND_VERTEX_BUFFER;
  i.          ByteWidth=(UINT)size_in_bytes;
  ID3D11Device_CreateBuffer(rx.Device,&i,NULL,&rx.VertexBuffer);
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

void rxpull_resources()
{
  rx.      uniform=rxborrow_resource(rx.UniformBuffer);
  rx.vertex_buffer=rxborrow_resource(rx.VertexBuffer);
  rx. index_buffer=rxborrow_resource(rx.IndexBuffer);
}

void rxpush_resources()
{
  rxreturn_resource(rx.UniformBuffer);
  rxreturn_resource(rx. VertexBuffer);
  rxreturn_resource(rx.  IndexBuffer);
}

int rxtick()
{
  rxmatrix_t matrix=rxmatrix_identity();
  matrix.m[0][0]=+2.0f/(rx.size_x);
  matrix.m[1][1]=+2.0f/(rx.size_y);
  matrix.m[2][2]=+0.5f;
  matrix.m[3][0]=-1.0f;
  matrix.m[3][1]=-1.0f;
  rx.uniform->e=matrix;

  rxpush_resources();

  rxwindow();

  ID3D11DeviceContext_ClearRenderTargetView(rx.Context,
    rx.OffscreenBufferView,(float*)&rx.clear_color);

  int index_offset=0;
  rxcommand_t *draw;
  for (draw=rx.command_buffer;draw<rx.command_buffer+rx.command_buffer_index;++draw)
  { if(draw->kind==rxdraw_kCLIP)
    {
      // Todo:!
    } else
    if(draw->kind==rxdraw_kMATRIX)
    {
      rxuniform_t t;
      t.e=rxmatrix_mulM(draw->uniform.e,matrix);

      void *memory=rxborrow_resource(rx.UniformBuffer);
      memcpy(memory,&t,sizeof(t));
      rxreturn_resource(rx.UniformBuffer);

    } else
    if(draw->kind==rxdraw_kSAMPLER)
    {
      ccassert(draw->unknown!=0);

      ID3D11DeviceContext_PSSetSamplers(rx.Context,0,1,(ID3D11SamplerState**)&draw->unknown);
    } else
    if(draw->kind==rxdraw_kTEXTURE)
    {
      ccassert(draw->unknown!=0);

      ID3D11DeviceContext_PSSetShaderResources(rx.Context,0,1,(ID3D11ShaderResourceView**)&draw->unknown);
    } else
    if(draw->kind==rxdraw_kINDEXED)
    {
      ID3D11DeviceContext_DrawIndexed(rx.Context,draw->length,index_offset,draw->offset);
      index_offset+=draw->length;
    }
  }

  // Note: disable this if MSAA is off
  ID3D11DeviceContext_ResolveSubresource(rx.Context,
    (ID3D11Resource*)rx.RenderBuffer,0,(ID3D11Resource*)rx.OffscreenBuffer,0,DXGI_FORMAT_R8G8B8A8_UNORM);

  IDXGISwapChain_Present(rx.SwapChain,1u,0);
  WaitForSingleObjectEx(rx.FrameAwait,33,TRUE);

  if(!rx.Visible)
  {
    rx.Visible=cctrue;
    ShowWindow(rx.Window,SW_SHOW);
  }

  rx.vertex_buffer_index=0;
  rx.index_buffer_index=0;
  rx.command_buffer_index=0;

  rxpull_resources();

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
    } break;
    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
    { rx.window_events[wParam]=1;

      if(!rx.ClickFocused)
      { rx.ClickFocused=TRUE;
        SetCapture((HWND)rx.Window);
      }
    } break;
    case WM_LBUTTONUP: case WM_RBUTTONUP:
    case WM_MBUTTONUP: case WM_XBUTTONUP:
    { rx.window_events[wParam]=0;

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

void rxinit(const wchar_t *window_title)
{
#ifdef _RX_STANDALONE
  // -- Todo: remove this!
  ccdebugnone=cctrue;
  ccini();
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


  rxwindow();

  IDXGIFactory2 *DXGIFactory=NULL;
  IDXGIDevice *DXGIDevice=NULL;
  IDXGIAdapter *DXGIAdapter=NULL;
  ID3D11Device_QueryInterface(rx.Device,&IID_IDXGIDevice,(void **)&DXGIDevice);
  IDXGIDevice_GetAdapter(DXGIDevice,&DXGIAdapter);
  IDXGIAdapter_GetParent(DXGIAdapter,&IID_IDXGIFactory2,(void**)&DXGIFactory);
  IDXGIAdapter_Release(DXGIAdapter);
  IDXGIDevice_Release(DXGIDevice);

  DXGI_SWAP_CHAIN_DESC1 SwapchainInfo;
  ZeroMemory(&SwapchainInfo,sizeof(SwapchainInfo));
  SwapchainInfo.Width=rx.size_x;
  SwapchainInfo.Height=rx.size_y;
  SwapchainInfo.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
  SwapchainInfo.SampleDesc.Count=1;
  SwapchainInfo.SampleDesc.Quality=0;
  SwapchainInfo.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT; // DXGI_USAGE_UNORDERED_ACCESS
  SwapchainInfo.BufferCount=2;
  SwapchainInfo.SwapEffect=DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
  SwapchainInfo.Flags=DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH|DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

  DXGI_SWAP_CHAIN_FULLSCREEN_DESC SwapchainFullscreenInfo;
  ZeroMemory(&SwapchainFullscreenInfo,sizeof(SwapchainFullscreenInfo));
  SwapchainFullscreenInfo.RefreshRate.Numerator=1;
  SwapchainFullscreenInfo.RefreshRate.Denominator=1;
  SwapchainFullscreenInfo.Windowed=TRUE;

  IDXGIFactory2_CreateSwapChainForHwnd(DXGIFactory,(IUnknown *)rx.Device,rx.Window,
    &SwapchainInfo,&SwapchainFullscreenInfo,NULL,(IDXGISwapChain1 **)&rx.SwapChain);

  rx.FrameAwait=IDXGISwapChain2_GetFrameLatencyWaitableObject(rx.SwapChain);

  IDXGISwapChain_GetBuffer(rx.SwapChain,0,&IID_ID3D11Texture2D,(void **)&rx.RenderBuffer);
  IDXGIFactory_Release(DXGIFactory);

  D3D11_RENDER_TARGET_VIEW_DESC RenderTargetViewInfo;
  ZeroMemory(&RenderTargetViewInfo,sizeof(RenderTargetViewInfo));
  RenderTargetViewInfo.Format=DXGI_FORMAT_UNKNOWN;
  RenderTargetViewInfo.ViewDimension=D3D11_RTV_DIMENSION_TEXTURE2D;
  ID3D11Device_CreateRenderTargetView(rx.Device,(ID3D11Resource*)rx.RenderBuffer,&RenderTargetViewInfo,&rx.RenderBufferView);

  D3D11_TEXTURE2D_DESC OffscreenBuffer;
  ZeroMemory(&OffscreenBuffer,sizeof(OffscreenBuffer));
  OffscreenBuffer.Width=rx.size_x;
  OffscreenBuffer.Height=rx.size_y;
  OffscreenBuffer.MipLevels=1;
  OffscreenBuffer.ArraySize=1;
  OffscreenBuffer.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
  OffscreenBuffer.SampleDesc.Count=4;
  OffscreenBuffer.SampleDesc.Quality=1;
  OffscreenBuffer.Usage=D3D11_USAGE_DEFAULT;
  OffscreenBuffer.MiscFlags=0;
  OffscreenBuffer.BindFlags=D3D11_BIND_RENDER_TARGET;
  OffscreenBuffer.CPUAccessFlags=0;
  ID3D11Device_CreateTexture2D(rx.Device,&OffscreenBuffer,NULL,(ID3D11Texture2D**)&rx.OffscreenBuffer);

    // ID3D11Device_CheckMultisampleQualityLevels(rx.Device,
    //   OffscreenBuffer.Format,
    //   OffscreenBuffer.SampleDesc.Count,
    //   &OffscreenBuffer.SampleDesc.Quality);

  D3D11_RENDER_TARGET_VIEW_DESC OffscreenBufferView;
  ZeroMemory(&OffscreenBufferView,sizeof(OffscreenBufferView));
  OffscreenBufferView.Format=DXGI_FORMAT_UNKNOWN;
  OffscreenBufferView.ViewDimension=D3D11_RTV_DIMENSION_TEXTURE2DMS;
  ID3D11Device_CreateRenderTargetView(rx.Device,(ID3D11Resource*)rx.OffscreenBuffer,&OffscreenBufferView,
    (ID3D11RenderTargetView **)&rx.OffscreenBufferView);

  D3D11_RASTERIZER_DESC RasterizerInfo;
  ZeroMemory(&RasterizerInfo,sizeof(RasterizerInfo));
  RasterizerInfo.FillMode=D3D11_FILL_SOLID;
  RasterizerInfo.CullMode=D3D11_CULL_NONE;
  RasterizerInfo.FrontCounterClockwise=FALSE;
  RasterizerInfo.DepthBias=D3D11_DEFAULT_DEPTH_BIAS;
  RasterizerInfo.DepthBiasClamp=D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
  RasterizerInfo.SlopeScaledDepthBias=D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
  RasterizerInfo.DepthClipEnable=FALSE;
  RasterizerInfo.ScissorEnable=TRUE;
  RasterizerInfo.MultisampleEnable=TRUE;
  RasterizerInfo.AntialiasedLineEnable=TRUE;
  ID3D11Device_CreateRasterizerState(rx.Device,&RasterizerInfo,&rx.RasterizerState);

  D3D11_RENDER_TARGET_BLEND_DESC TargetBlendStateInfo;
  ZeroMemory(&TargetBlendStateInfo,sizeof(TargetBlendStateInfo));
  TargetBlendStateInfo.BlendEnable=TRUE;
  TargetBlendStateInfo.SrcBlend=D3D11_BLEND_SRC_ALPHA;
  TargetBlendStateInfo.DestBlend=D3D11_BLEND_INV_SRC_ALPHA;
  TargetBlendStateInfo.BlendOp=D3D11_BLEND_OP_ADD;
  TargetBlendStateInfo.SrcBlendAlpha=D3D11_BLEND_ZERO;
  TargetBlendStateInfo.DestBlendAlpha=D3D11_BLEND_ZERO;
  TargetBlendStateInfo.BlendOpAlpha=D3D11_BLEND_OP_ADD;
  TargetBlendStateInfo.RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;

  D3D11_BLEND_DESC BlendStateInfo;
  ZeroMemory(&BlendStateInfo,sizeof(BlendStateInfo));
  BlendStateInfo.RenderTarget[0]=TargetBlendStateInfo;
  ID3D11Device_CreateBlendState(rx.Device,&BlendStateInfo,&rx.BlendState);

#if 0
  UINT CompilationFlags=D3DCOMPILE_DEBUG|D3DCOMPILE_SKIP_OPTIMIZATION;
  ID3DBlob *ShaderBlob,*MessageBlob;
  if(SUCCEEDED(D3DCompile(ShaderCode,strlen(ShaderCode),"rx::vertex-shader",0,0,"MainVS","vs_5_0",
    CompilationFlags,0,&ShaderBlob,&MessageBlob)))
  {
  }
#endif

  D3D11_INPUT_ELEMENT_DESC LayoutElements[]=
  { (D3D11_INPUT_ELEMENT_DESC){"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    (D3D11_INPUT_ELEMENT_DESC){"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    (D3D11_INPUT_ELEMENT_DESC){"COLOR",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
  };

  ID3D11Device_CreateInputLayout(rx.Device,LayoutElements,ARRAYSIZE(LayoutElements),
    rx_vs_shader_bytecode,sizeof(rx_vs_shader_bytecode),&rx.VertexShaderInputLayout);
  ID3D11Device_CreateVertexShader(rx.Device,
    rx_vs_shader_bytecode,sizeof(rx_vs_shader_bytecode),NULL,&rx.VertexShader);
  ID3D11Device_CreatePixelShader(rx.Device,
    rx_ps_shader_bytecode,sizeof(rx_ps_shader_bytecode),NULL,&rx.PixelShader);

  D3D11_SAMPLER_DESC SamplerInfo;
  ZeroMemory(&SamplerInfo,sizeof(SamplerInfo));
  SamplerInfo.AddressU=D3D11_TEXTURE_ADDRESS_WRAP;
  SamplerInfo.AddressV=D3D11_TEXTURE_ADDRESS_WRAP;
  SamplerInfo.AddressW=D3D11_TEXTURE_ADDRESS_WRAP;

  SamplerInfo.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  ID3D11Device_CreateSamplerState(rx.Device,&SamplerInfo,&rx.linear_sampler.Sampler);

  SamplerInfo.Filter=D3D11_FILTER_MIN_MAG_MIP_POINT;
  ID3D11Device_CreateSamplerState(rx.Device,&SamplerInfo,&rx.point_sampler.Sampler);

  rxresize_uniformbuffer(sizeof(rxuniform_t));
  rxresize_indexbuffer(sizeof(rxindex_t)*0x1000*4);
  rxresize_vertexbuffer(sizeof(rxvertex_t)*0x1000*4);

  rxpull_resources();

  D3D11_VIEWPORT Viewport;
  ZeroMemory(&Viewport,sizeof(Viewport));
  Viewport.Width=(float)rx.size_x;
  Viewport.Height=(float)rx.size_y;
  Viewport.MinDepth=0;
  Viewport.MaxDepth=1;
  unsigned int Stride=sizeof(rxvertex_t);
  unsigned int Offset=0;
  ID3D11DeviceContext_RSSetState(rx.Context,rx.RasterizerState);
  ID3D11DeviceContext_RSSetViewports(rx.Context,1,&Viewport);
  ID3D11DeviceContext_OMSetBlendState(rx.Context,rx.BlendState,0x00,0xFFFFFFFu);
  ID3D11DeviceContext_OMSetRenderTargets(rx.Context,1,&rx.OffscreenBufferView,0);
  ID3D11DeviceContext_IASetInputLayout(rx.Context,rx.VertexShaderInputLayout);
  ID3D11DeviceContext_IASetVertexBuffers(rx.Context,0,1,&rx.VertexBuffer,&Stride,&Offset);
  ID3D11DeviceContext_IASetIndexBuffer(rx.Context,rx.IndexBuffer,DXGI_FORMAT_R32_UINT,0);
  ID3D11DeviceContext_IASetPrimitiveTopology(rx.Context,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  ID3D11DeviceContext_VSSetConstantBuffers(rx.Context,0,1,&rx.UniformBuffer);
  ID3D11DeviceContext_VSSetShader(rx.Context,rx.VertexShader,0x00,0);
  ID3D11DeviceContext_VSSetSamplers(rx.Context,0,1,&rx.linear_sampler.Sampler);

  ID3D11DeviceContext_PSSetConstantBuffers(rx.Context,0,1,&rx.UniformBuffer);
  ID3D11DeviceContext_PSSetShader(rx.Context,rx.PixelShader,0x00,0);
  ID3D11DeviceContext_PSSetSamplers(rx.Context,0,1,&rx.linear_sampler.Sampler);

  D3D11_RECT RootClip;
  RootClip.left=0;
  RootClip.top=0;
  RootClip.right=0xffffff;
  RootClip.bottom=0xffffff;
  ID3D11DeviceContext_RSSetScissorRects(rx.Context,1,&RootClip);

  rxclear(rxf32x4(.2,.2,.2,1));

  rx.white=rxcreate_texture(16,16,rxRGBA8888);

  rxborrowed_t t=rxborrow_texture(rx.white);
  memset(t.memory,0xff,t.stride*rx.white.size_y);
  rxreturn(t);

  rx.start_ticks=ccclocktick();
  rx.frame_ticks=rx.start_ticks;
  rxtime();
}
#endif
