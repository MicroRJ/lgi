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
*/
#ifndef _RX_H
#define _RX_H

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

#define     rxRGB8 DXGI_FORMAT_R8_UNORM
#define rxRGBA8888 DXGI_FORMAT_R8G8B8A8_UNORM

typedef struct rxsampler_t rxsampler_t;
typedef struct rxsampler_t
{
  ID3D11SamplerState        * Sampler;
} rxsampler_t;

typedef void *rxbindview_t;
typedef void *rxresource_t;

typedef struct rxtexture_t rxtexture_t;
typedef struct rxtexture_t
{ rxresource_t resource;
  rxbindview_t bindview;
           int size_x, size_y;
           int format;
} rxtexture_t;

// Note:
typedef struct rxborrowed_t rxborrowed_t;
typedef struct rxborrowed_t
{ rxresource_t resource;
         void *memory;
          int  stride;
} rxborrowed_t;

typedef struct rxvertex_t rxvertex_t;
typedef struct rxvertex_t
{ float x, y;
  unsigned int color;
  float u, v;
} rxvertex_t;

typedef int rxindex_t;

// Todo: remove this, this would be for a higher level API?
typedef enum rxdraw_k
{ rxdraw_kCLIP,
  rxdraw_kPRIM,
} rxdraw_k;

// Todo: remove this, this would be for a higher level API?
typedef struct rxdraw_t rxdraw_t;
typedef struct rxdraw_t
{     rxdraw_k kind;
  rxbindview_t bindview;
   rxsampler_t sampler;
           int offset;
           int length;
           int x,y,w,h;
} rxdraw_t;

typedef struct rxuniform_t rxuniform_t;
typedef struct rxuniform_t
{ float e[4][4];
} rxuniform_t;

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

  int        *  index_buffer;
  rxvertex_t * vertex_buffer;
  int vertex_buffer_index;
  int  index_buffer_index;

  // Todo: remove this, this would be for a higher level API?
  rxdraw_t  draw_buffer[0x100];
  int       draw_buffer_index;
} rx_t;

static rx_t rx;

// Note: this would be more internal use?
void *rxborrow_resource(void *resource)
{
  D3D11_MAPPED_SUBRESOURCE m;
  ID3D11DeviceContext_Map(rx.Context,(ID3D11Resource*)resource,0,D3D11_MAP_WRITE_DISCARD,0,&m);

  return m.pData;
}

// Note: this would be more internal use?
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
  D3D11_MAPPED_SUBRESOURCE m;
  ID3D11DeviceContext_Map(rx.Context,(ID3D11Resource*)texture.resource,0,D3D11_MAP_WRITE_DISCARD,0,&m);

  rxborrowed_t result;
  result.resource=texture.resource;
  result.stride=m.RowPitch;
  result.memory=m.pData;
  return result;
}

rxtexture_t rxcreate_texture(int w, int h, int f)
{ D3D11_TEXTURE2D_DESC i;
  ZeroMemory(&i,sizeof(i));
  i.Width=(unsigned int)w;
  i.Height=(unsigned int)h;
  i.MipLevels=1;
  i.ArraySize=1;
  i.Format=f;
  i.SampleDesc.Count=1;
  i.SampleDesc.Quality=0;
  i.Usage=D3D11_USAGE_DYNAMIC;
  i.MiscFlags=0;
  i.BindFlags=D3D11_BIND_SHADER_RESOURCE;
  i.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;

  rxtexture_t r;
  r.size_x=w;
  r.size_y=h;
  r.format=f;

  ID3D11Device_CreateTexture2D(rx.Device,&i,NULL,(ID3D11Texture2D**)&r.resource);

  D3D11_SHADER_RESOURCE_VIEW_DESC v;
  ZeroMemory(&v,sizeof(v));
  v.Format=DXGI_FORMAT_UNKNOWN;
  v.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
  v.Texture2D.MostDetailedMip=0;
  v.Texture2D.MipLevels=1;

  ID3D11Device_CreateShaderResourceView(rx.Device,
    (ID3D11Resource *)r.resource,&v,(ID3D11ShaderResourceView**)&r.bindview);
  return r;
}

rxtexture_t rxload_texture(int w, int h, DXGI_FORMAT f, int s, void *m)
{ D3D11_TEXTURE2D_DESC TextureInfo;
  ZeroMemory(&TextureInfo,sizeof(TextureInfo));
  TextureInfo.Width=(unsigned int)w;
  TextureInfo.Height=(unsigned int)h;
  TextureInfo.MipLevels=1;
  TextureInfo.ArraySize=1;
  TextureInfo.Format=f;
  TextureInfo.SampleDesc.Count=1;
  TextureInfo.SampleDesc.Quality=0;
  TextureInfo.Usage=D3D11_USAGE_DYNAMIC;
  TextureInfo.MiscFlags=0;
  TextureInfo.BindFlags=D3D11_BIND_SHADER_RESOURCE;
  TextureInfo.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;

  D3D11_SUBRESOURCE_DATA SubresourceInfo;
  ZeroMemory(&SubresourceInfo,sizeof(SubresourceInfo));
  SubresourceInfo.pSysMem=m;
  SubresourceInfo.SysMemPitch=s;

  rxtexture_t r;
  r.size_x=w;
  r.size_y=h;
  r.format=f;

  ID3D11Device_CreateTexture2D(rx.Device,&TextureInfo,&SubresourceInfo,(ID3D11Texture2D**)&r.resource);

  D3D11_SHADER_RESOURCE_VIEW_DESC ViewInfo;
  ZeroMemory(&ViewInfo,sizeof(ViewInfo));
  ViewInfo.Format=DXGI_FORMAT_UNKNOWN;
  ViewInfo.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
  ViewInfo.Texture2D.MostDetailedMip=0;
  ViewInfo.Texture2D.MipLevels=1;

  ID3D11Device_CreateShaderResourceView(rx.Device,(ID3D11Resource *)r.resource,&ViewInfo,
    (ID3D11ShaderResourceView**)&r.bindview);
  return r;
}

rxtexture_t rxload_texture_file(const char *name)
{ int x,y,n;
  unsigned char *m;
  rxtexture_t r;
  m=stbi_load(name,&x,&y,&n,4);
  r=rxload_texture(x,y,DXGI_FORMAT_R8G8B8A8_UNORM,x*4,m);
  stbi_image_free(m);
  return r;
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

  unsigned char *m=rxborrow_resource(rx.font_atlas.resource);
  for(int i=0;i<w*h;++i)
  { unsigned char c=buffer[i];
    m[i*4+0]=c;
    m[i*4+1]=c;
    m[i*4+2]=c;
    m[i*4+3]=c;
  }
  rxreturn_resource(rx.font_atlas.resource);

  ccfree(buffer);
}

void rxdraw_texture_ex(rxtexture_t texture, rxsampler_t sampler,
  float x, float y, float w, float h, float tx, float ty, float tw, float th)
{ rxdraw_t *draw=rx.draw_buffer+rx.draw_buffer_index++;
  draw->kind=rxdraw_kPRIM;
  draw->offset=rx.vertex_buffer_index;
  draw->length=6;
  draw->bindview=texture.bindview;
  draw->sampler=sampler;

  int *index=rx.index_buffer+rx.index_buffer_index;
  rx.index_buffer_index+=6;

  index[0]=0;index[3]=0;
  index[1]=1;index[4]=2;
  index[2]=2;index[5]=3;

  rxvertex_t *vert=rx.vertex_buffer+rx.vertex_buffer_index;
  rx.vertex_buffer_index+=4;

  vert[0]=(rxvertex_t){x+0,y+0,0xffffffff,tx   ,ty+th};
  vert[1]=(rxvertex_t){x+0,y+h,0xffffffff,tx   ,ty   };
  vert[2]=(rxvertex_t){x+w,y+h,0xffffffff,tx+tw,ty   };
  vert[3]=(rxvertex_t){x+w,y+0,0xffffffff,tx+tw,ty+th};
}

void rxdraw_texture(rxtexture_t texture, rxsampler_t sampler, int x, int y, int w, int h)
{
  rxdraw_texture_ex(texture,sampler,
    (float)x,(float)y,(float)w,(float)h,0,0,1,1);
}

void rxdraw_text(int x, int y, float h, const char *string)
{ float nx=1.f/rx.font_atlas.size_x;
  float ny=1.f/rx.font_atlas.size_y;
  float ox=(float)x;
  float oy=(float)y;
  float s=h/rx.font_height;
  int code;
  for(code=*string;code!=0;code=*++string)
  { stbtt_bakedchar baked=rx.font_glyph[code-32];
    float gx=(float)(baked.x0);
    float gy=(float)(baked.y0);
    float gw=(float)(baked.x1-baked.x0);
    float gh=(float)(baked.y1-baked.y0);
    float dx=ox;
    float dy=oy-(gh+baked.yoff)*s;
    rxdraw_texture_ex(rx.font_atlas,rx.linear_sampler,
      dx,dy,gw*s,gh*s,
      gx*nx,gy*ny,gw*nx,gh*ny);
    ox+=(float)baked.xadvance*s;
  }
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

void rxtick()
{
  rxreturn_resource(rx.VertexBuffer);
  rxreturn_resource(rx.IndexBuffer);

  rxwindow();

  float Color[4]={0.f,0.f,0.f,1.f};
  ID3D11DeviceContext_ClearRenderTargetView(rx.Context,rx.OffscreenBufferView,Color);
  ID3D11DeviceContext_OMSetRenderTargets(rx.Context,1,&rx.OffscreenBufferView,0);

  rxdraw_t *draw;
  int index_offset=0;
  for (draw=rx.draw_buffer;draw<rx.draw_buffer+rx.draw_buffer_index;++draw)
  { if(draw->kind==rxdraw_kCLIP)
    {
      // Todo:!
    } else
    if(draw->kind==rxdraw_kPRIM)
    { ID3D11DeviceContext_PSSetSamplers(rx.Context,0,1,&draw->sampler.Sampler);
      ID3D11DeviceContext_PSSetShaderResources(rx.Context,0,1,
        (ID3D11ShaderResourceView**)&draw->bindview);
      ID3D11DeviceContext_DrawIndexed(rx.Context,draw->length,index_offset,draw->offset);
      index_offset+=draw->length;
    }
  }

  ID3D11DeviceContext_ResolveSubresource(rx.Context,
    (ID3D11Resource*)rx.RenderBuffer,0,(ID3D11Resource*)rx.OffscreenBuffer,0,DXGI_FORMAT_R8G8B8A8_UNORM);

  IDXGISwapChain_Present(rx.SwapChain,1u,0);
  WaitForSingleObjectEx(rx.FrameAwait,33,TRUE);

  ShowWindow(rx.Window,SW_SHOW);

  rx.vertex_buffer_index=0;
  rx.index_buffer_index=0;
  rx.draw_buffer_index=0;

  rx.vertex_buffer=rxborrow_resource(rx.VertexBuffer);
  rx.index_buffer=rxborrow_resource(rx.IndexBuffer);
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

LRESULT CALLBACK
rxwindow_callback_win32(HWND NativeWindow,UINT Message,WPARAM wParam,LPARAM lParam)
{ return DefWindowProcW(NativeWindow,Message,wParam,lParam);
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
    (D3D11_INPUT_ELEMENT_DESC){"COLOR",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    (D3D11_INPUT_ELEMENT_DESC){"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
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

  {
    rxuniform_t uniform;
    uniform.e[0][0]=2.0f/(rx.size_x);
    uniform.e[0][1]=0.f;
    uniform.e[0][2]=0.f;
    uniform.e[0][3]=0.f;

    uniform.e[1][0]=0.f;
    uniform.e[1][1]=2.0f/(rx.size_y);
    uniform.e[1][2]=0.f;
    uniform.e[1][3]=0.f;

    uniform.e[2][0]=0.f;
    uniform.e[2][1]=0.f;
    uniform.e[2][2]=.5f;
    uniform.e[2][3]=0.f;

    uniform.e[3][0]=-1.f;
    uniform.e[3][1]=-1.f;
    uniform.e[3][2]=.0f;
    uniform.e[3][3]=1.f;


    D3D11_BUFFER_DESC BufferInfo;
    BufferInfo.Usage=D3D11_USAGE_DEFAULT;
    BufferInfo.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
    BufferInfo.CPUAccessFlags=0;
    BufferInfo.MiscFlags=0;
    BufferInfo.ByteWidth=sizeof(uniform);
    BufferInfo.StructureByteStride=0;

    D3D11_SUBRESOURCE_DATA SubresourceInfo;
    SubresourceInfo.pSysMem=&uniform;
    SubresourceInfo.SysMemPitch=0;
    SubresourceInfo.SysMemSlicePitch=0;
    ID3D11Device_CreateBuffer(rx.Device,&BufferInfo,&SubresourceInfo,&rx.UniformBuffer);
  }

  rxresize_indexbuffer(sizeof(rxindex_t)*1024);
  rxresize_vertexbuffer(sizeof(rxvertex_t)*1024);

  rx.vertex_buffer=rxborrow_resource(rx.VertexBuffer);
  rx.index_buffer=rxborrow_resource(rx.IndexBuffer);

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
  ID3D11DeviceContext_IASetInputLayout(rx.Context,rx.VertexShaderInputLayout);
  ID3D11DeviceContext_IASetVertexBuffers(rx.Context,0,1,&rx.VertexBuffer,&Stride,&Offset);
  ID3D11DeviceContext_IASetIndexBuffer(rx.Context,rx.IndexBuffer,DXGI_FORMAT_R32_UINT,0);
  ID3D11DeviceContext_IASetPrimitiveTopology(rx.Context,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  ID3D11DeviceContext_VSSetConstantBuffers(rx.Context,0,1,&rx.UniformBuffer);
  ID3D11DeviceContext_PSSetConstantBuffers(rx.Context,0,1,&rx.UniformBuffer);
  ID3D11DeviceContext_VSSetShader(rx.Context,rx.VertexShader,0x00,0);
  ID3D11DeviceContext_PSSetShader(rx.Context,rx.PixelShader,0x00,0);

  D3D11_RECT RootClip;
  RootClip.left=0;
  RootClip.top=0;
  RootClip.right=0xffffff;
  RootClip.bottom=0xffffff;
  ID3D11DeviceContext_RSSetScissorRects(rx.Context,1,&RootClip);
}
#endif
