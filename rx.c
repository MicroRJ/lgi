/*
**
** -+- rx -+-
**
** Copyright(C) J. Dayan Rodriguez, 2022, All rights reserved.
**
** Simple render API for when you just want to see something.
**
** rx is not production ready nor production quality, please
** use 'raylib' instead.
**
*/
#ifndef RX_H
#define RX_H
#define CINTERFACE
#define COBJMACROS
#define D3D11_NO_HELPERS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
// #define _NO_CRT_STDIO_INLINE
#pragma warning(disable:4115)
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

#include <dxgidebug.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <dxgi1_3.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize.h"

typedef struct rxsampler_t rxsampler_t;
typedef struct rxsampler_t
{
  ID3D11SamplerState        * Sampler;
} rxsampler_t;

typedef struct rxtexture_t rxtexture_t;
typedef struct rxtexture_t
{ ID3D11Texture2D           * Texture2D;
  ID3D11ShaderResourceView  * Access;
} rxtexture_t;

typedef struct rxvertex_t rxvertex_t;
typedef struct rxvertex_t
{ float x, y;
  unsigned int color;
  float u, v;
} rxvertex_t;

typedef int rxindex_t;

typedef enum rxdraw_k
{ rxdraw_kCLIP,
  rxdraw_kPRIM,
} rxdraw_k;

typedef struct rxdraw_t rxdraw_t;
typedef struct rxdraw_t
{    rxdraw_k kind;
  rxtexture_t texture;
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
{ unsigned                  Quitted:      1;
  unsigned                  Visible:      1;
  unsigned                  Resizable:    1;
  unsigned                  Decorated:    1;
  unsigned                  Floating:     1;
  unsigned                  FocusOnShow:  1;
  unsigned                  EventHolder:  1;
  unsigned                  ClickFocused: 1;
  HWND                      Window;
  void                   *  ControlFiber;
  void                   *  MessageFiber;
  ID3D11InfoQueue        *  InfoQueue;
  ID3D11Device           *  Device;
  ID3D11DeviceContext    *  Context;
  IDXGISwapChain2        *  SwapChain;
  void                   *  FrameAwait;
  ID3D11Texture2D        *  BackBuffer;
  ID3D11RenderTargetView *  RenderTargetView;
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

  int         index_buffer[0x1000];
  rxvertex_t vertex_buffer[0x1000];
  rxdraw_t     draw_buffer[0x100];

  int vertex_buffer_index;
  int index_buffer_index;
  int draw_buffer_index;
} rx_t;
static rx_t rx;

void rxdraw(rxtexture_t texture, rxsampler_t sampler, float x, float y, float w, float h)
{
  rxdraw_t *draw=rx.draw_buffer+rx.draw_buffer_index++;
  draw->kind=rxdraw_kPRIM;
  draw->offset=rx.vertex_buffer_index;
  draw->length=6;
  draw->texture=texture;
  draw->sampler=sampler;

  int *index=rx.index_buffer+rx.index_buffer_index;
  rx.index_buffer_index+=6;

  index[0]=0;index[3]=0;
  index[1]=1;index[4]=2;
  index[2]=2;index[5]=3;

  rxvertex_t *vert=rx.vertex_buffer+rx.vertex_buffer_index;
  rx.vertex_buffer_index+=4;

  vert[0]=(rxvertex_t){x+0,y+0,0xffffffff,0,1};
  vert[1]=(rxvertex_t){x+0,y+h,0xffffffff,0,0};
  vert[2]=(rxvertex_t){x+w,y+h,0xffffffff,1,0};
  vert[3]=(rxvertex_t){x+w,y+0,0xffffffff,1,1};
}

void *rxborrow_texture(rxtexture_t texture)
{
  D3D11_MAPPED_SUBRESOURCE m;
  ID3D11DeviceContext_Map(rx.Context,(ID3D11Resource*)texture.Texture2D,0,D3D11_MAP_WRITE_DISCARD,0,&m);

  return m.pData;
}

void rxreturn_texture(rxtexture_t texture)
{
  ID3D11DeviceContext_Unmap(rx.Context,(ID3D11Resource*)texture.Texture2D,0);
}

rxtexture_t rxcreate_texture(int w, int h)
{
  D3D11_TEXTURE2D_DESC i;
  ZeroMemory(&i,sizeof(i));
  i.Width=(unsigned int)w;
  i.Height=(unsigned int)h;
  i.MipLevels=1;
  i.ArraySize=1;
  i.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
  i.SampleDesc.Count=1;
  i.SampleDesc.Quality=0;
  i.Usage=D3D11_USAGE_DYNAMIC;
  i.MiscFlags=0;
  i.BindFlags=D3D11_BIND_SHADER_RESOURCE;
  i.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;

  rxtexture_t r;
  ID3D11Device_CreateTexture2D(rx.Device,&i,NULL,&r.Texture2D);

  D3D11_SHADER_RESOURCE_VIEW_DESC v;
  ZeroMemory(&v,sizeof(v));
  v.Format=DXGI_FORMAT_UNKNOWN;
  v.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
  v.Texture2D.MostDetailedMip=0;
  v.Texture2D.MipLevels=1;

  ID3D11Device_CreateShaderResourceView(rx.Device,(ID3D11Resource *)r.Texture2D,&v,&r.Access);

  return r;
}

rxtexture_t rxload_texture(int w, int h, int s, void *m)
{ D3D11_TEXTURE2D_DESC TextureInfo;
  ZeroMemory(&TextureInfo,sizeof(TextureInfo));
  TextureInfo.Width=(unsigned int)w;
  TextureInfo.Height=(unsigned int)h;
  TextureInfo.MipLevels=1;
  TextureInfo.ArraySize=1;
  TextureInfo.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
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
  ID3D11Device_CreateTexture2D(rx.Device,&TextureInfo,&SubresourceInfo,&r.Texture2D);

  D3D11_SHADER_RESOURCE_VIEW_DESC ViewInfo;
  ZeroMemory(&ViewInfo,sizeof(ViewInfo));
  ViewInfo.Format=DXGI_FORMAT_UNKNOWN;
  ViewInfo.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
  ViewInfo.Texture2D.MostDetailedMip=0;
  ViewInfo.Texture2D.MipLevels=1;

  ID3D11Device_CreateShaderResourceView(rx.Device,(ID3D11Resource *)r.Texture2D,&ViewInfo,&r.Access);
  return r;
}

rxtexture_t rxload_texture_file(const char *name)
{
  int x,y,n;
  unsigned char *m;
  rxtexture_t r;

  m=stbi_load(name,&x,&y,&n,4);
  r=rxload_texture(x,y,x*4,m);
  stbi_image_free(m);
  return r;
}

void rxtick()
{
  MSG Message;
  while(PeekMessage(&Message,NULL,0,0,PM_REMOVE))
  { TranslateMessage(&Message);
    DispatchMessageW(&Message);
  }

  RECT WindowClient;
  GetClientRect(rx.Window,&WindowClient);

  UINT WindowWidth,WindowHeight;
  WindowWidth=WindowClient.right-WindowClient.left;
  WindowHeight=WindowClient.bottom-WindowClient.top;

  float Color[4]={1.f,0.f,0.f,1.f};
  ID3D11DeviceContext_ClearRenderTargetView(rx.Context,rx.RenderTargetView,Color);

  if(!rx.UniformBuffer)
  {
    rxuniform_t uniform;
    uniform.e[0][0]=2.0f/(WindowWidth);
    uniform.e[0][1]=0.f;
    uniform.e[0][2]=0.f;
    uniform.e[0][3]=0.f;

    uniform.e[1][0]=0.f;
    uniform.e[1][1]=2.0f/(WindowHeight);
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

  if(!rx.IndexBuffer)
  { D3D11_BUFFER_DESC BufferInfo;
    BufferInfo.Usage=D3D11_USAGE_DYNAMIC;
    BufferInfo.BindFlags=D3D11_BIND_INDEX_BUFFER;
    BufferInfo.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
    BufferInfo.MiscFlags=0;
    BufferInfo.ByteWidth=sizeof(rx.index_buffer);
    BufferInfo.StructureByteStride=0;

    D3D11_SUBRESOURCE_DATA SubresourceInfo;
    SubresourceInfo.pSysMem=rx.index_buffer;
    SubresourceInfo.SysMemPitch=0;
    SubresourceInfo.SysMemSlicePitch=0;
    ID3D11Device_CreateBuffer(rx.Device,&BufferInfo,&SubresourceInfo,&rx.IndexBuffer);
  }

  if(!rx.VertexBuffer)
  { D3D11_BUFFER_DESC BufferInfo;
    BufferInfo.Usage=D3D11_USAGE_DYNAMIC;
    BufferInfo.BindFlags=D3D11_BIND_VERTEX_BUFFER;
    BufferInfo.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
    BufferInfo.MiscFlags=0;
    BufferInfo.ByteWidth=sizeof(rx.vertex_buffer);
    BufferInfo.StructureByteStride=0;

    D3D11_SUBRESOURCE_DATA SubresourceInfo;
    SubresourceInfo.pSysMem=rx.vertex_buffer;
    SubresourceInfo.SysMemPitch=0;
    SubresourceInfo.SysMemSlicePitch=0;
    ID3D11Device_CreateBuffer(rx.Device,&BufferInfo,&SubresourceInfo,&rx.VertexBuffer);
  }

  D3D11_MAPPED_SUBRESOURCE VertexMapped,IndexMapped;
  ID3D11DeviceContext_Map(rx.Context,(ID3D11Resource*)rx.VertexBuffer,0,D3D11_MAP_WRITE_DISCARD,0,&VertexMapped);
  ID3D11DeviceContext_Map(rx.Context,(ID3D11Resource*)rx.IndexBuffer,0,D3D11_MAP_WRITE_DISCARD,0,&IndexMapped);
  memcpy(VertexMapped.pData,rx.vertex_buffer,sizeof(rx.vertex_buffer));
  memcpy(IndexMapped.pData,rx.index_buffer,sizeof(rx.index_buffer));
  ID3D11DeviceContext_Unmap(rx.Context,(ID3D11Resource*)rx.VertexBuffer,0);
  ID3D11DeviceContext_Unmap(rx.Context,(ID3D11Resource*)rx.IndexBuffer,0);


  D3D11_VIEWPORT Viewport;
  ZeroMemory(&Viewport,sizeof(Viewport));
  Viewport.Width=(float)WindowWidth;
  Viewport.Height=(float)WindowHeight;
  Viewport.MinDepth=0;
  Viewport.MaxDepth=1;

  ID3D11DeviceContext_RSSetState(rx.Context,rx.RasterizerState);
  ID3D11DeviceContext_RSSetViewports(rx.Context,1,&Viewport);
  ID3D11DeviceContext_OMSetRenderTargets(rx.Context,1,&rx.RenderTargetView,0);

  unsigned int Stride=sizeof(rxvertex_t);
  unsigned int Offset=0;
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

  rxdraw_t *draw;
  int index_offset=0;
  for (draw=rx.draw_buffer;draw<rx.draw_buffer+rx.draw_buffer_index;++draw)
  { if(draw->kind==rxdraw_kCLIP)
    {
      D3D11_RECT R;//  = { (u32) ClipMin.X, (u32) ClipMin.Y, (u32) ClipMax.X, (u32) ClipMax.Y };
      ID3D11DeviceContext_RSSetScissorRects(rx.Context,1,&R);
    } else
    if(draw->kind==rxdraw_kPRIM)
    { ID3D11DeviceContext_PSSetSamplers(rx.Context,0,1,&draw->sampler.Sampler);
      ID3D11DeviceContext_PSSetShaderResources(rx.Context,0,1,&draw->texture.Access);
      ID3D11DeviceContext_DrawIndexed(rx.Context,draw->length,index_offset,draw->offset);
      index_offset+=draw->length;
    }
  }

  IDXGISwapChain_Present(rx.SwapChain,1u,0);
  WaitForSingleObjectEx(rx.FrameAwait,33,TRUE);

  ShowWindow(rx.Window,SW_SHOW);

  rx.vertex_buffer_index=0;
  rx.index_buffer_index=0;
  rx.draw_buffer_index=0;
}

LRESULT CALLBACK
rxwindow_callback_win32(HWND NativeWindow,UINT Message,WPARAM wParam,LPARAM lParam)
{ // rxwindow_t *window=(rxwindow_t*)GetWindowLongPtrA(NativeWindow,GWLP_USERDATA);
  return DefWindowProcW(NativeWindow,Message,wParam,lParam);
}

void rxinit(const wchar_t *WindowTitle)
{ UINT DriverModeFlags=
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
  WindowClass.lpszClassName=WindowTitle;
  RegisterClassW(&WindowClass);

  rx.Window=CreateWindowExW(WS_EX_NOREDIRECTIONBITMAP,WindowClass.lpszClassName,WindowTitle,
    WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
      NULL,NULL,WindowClass.hInstance,NULL);

  SetLastError(S_OK);
  SetWindowLongPtrA(rx.Window,GWLP_USERDATA,(LONG_PTR)&rx);
  SetLastError(S_OK);
  GetWindowLongPtrA(rx.Window,GWLP_USERDATA);

  RECT WindowClient;
  UINT WindowWidth,WindowHeight;
  GetClientRect(rx.Window,&WindowClient);
  WindowWidth=WindowClient.right-WindowClient.left;
  WindowHeight=WindowClient.bottom-WindowClient.top;

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
  SwapchainInfo.Width=WindowWidth;
  SwapchainInfo.Height=WindowHeight;
  SwapchainInfo.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
  SwapchainInfo.SampleDesc.Count=1;
  SwapchainInfo.SampleDesc.Quality=0;
  SwapchainInfo.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT; // DXGI_USAGE_UNORDERED_ACCESS
  SwapchainInfo.BufferCount=2;
  SwapchainInfo.SwapEffect= DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
  SwapchainInfo.Flags=DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH|DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

  DXGI_SWAP_CHAIN_FULLSCREEN_DESC SwapchainFullscreenInfo;
  ZeroMemory(&SwapchainFullscreenInfo,sizeof(SwapchainFullscreenInfo));
  SwapchainFullscreenInfo.RefreshRate.Numerator=1;
  SwapchainFullscreenInfo.RefreshRate.Denominator=1;
  SwapchainFullscreenInfo.Windowed=TRUE;

  IDXGIFactory2_CreateSwapChainForHwnd(DXGIFactory,(IUnknown *)rx.Device,rx.Window,
    &SwapchainInfo,&SwapchainFullscreenInfo,NULL,(IDXGISwapChain1 **)&rx.SwapChain);

  rx.FrameAwait=IDXGISwapChain2_GetFrameLatencyWaitableObject(rx.SwapChain);

  IDXGISwapChain_GetBuffer(rx.SwapChain,0,&IID_ID3D11Texture2D,(void **)&rx.BackBuffer);
  IDXGIFactory_Release(DXGIFactory);

  D3D11_RENDER_TARGET_VIEW_DESC RenderTargetViewInfo;
  ZeroMemory(&RenderTargetViewInfo,sizeof(RenderTargetViewInfo));
  RenderTargetViewInfo.Format=DXGI_FORMAT_UNKNOWN;
  RenderTargetViewInfo.ViewDimension=D3D11_RTV_DIMENSION_TEXTURE2D;
  ID3D11Device_CreateRenderTargetView(rx.Device,(ID3D11Resource*)rx.BackBuffer,&RenderTargetViewInfo,&rx.RenderTargetView);

  D3D11_RASTERIZER_DESC RasterizerInfo;
  ZeroMemory(&RasterizerInfo,sizeof(RasterizerInfo));
  RasterizerInfo.FillMode=D3D11_FILL_SOLID;
  RasterizerInfo.CullMode=D3D11_CULL_NONE;
  RasterizerInfo.FrontCounterClockwise=0;
  RasterizerInfo.DepthBias=0;
  RasterizerInfo.DepthBiasClamp=0;
  RasterizerInfo.SlopeScaledDepthBias=0;
  RasterizerInfo.DepthClipEnable=TRUE;
  RasterizerInfo.ScissorEnable=TRUE;
  RasterizerInfo.MultisampleEnable=FALSE;
  RasterizerInfo.AntialiasedLineEnable=FALSE;
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

  const char *ShaderCode=
  "cbuffer ConstBuffer : register(b0)"                                      "\r\n"
  "{ float4x4 ProjectionMatrix;"                                            "\r\n"
  "};"                                                                      "\r\n"
  "struct VS_INPUT"                                                         "\r\n"
  "{ float2 pos : POSITION;"                                                "\r\n"
  "  float4 col : COLOR0;"                                                  "\r\n"
  "  float2 uv  : TEXCOORD0;"                                               "\r\n"
  "};"                                                                      "\r\n"
  "struct PS_INPUT"                                                         "\r\n"
  "{ float4 pos : SV_POSITION;"                                             "\r\n"
  "  float4 col : COLOR0;"                                                  "\r\n"
  "  float2 uv  : TEXCOORD0;"                                               "\r\n"
  "};"                                                                      "\r\n"
  "PS_INPUT MainVS(VS_INPUT input)"                                         "\r\n"
  "{ PS_INPUT output;"                                                      "\r\n"
  "  output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));"  "\r\n"
  "  output.col = input.col;"                                               "\r\n"
  "  output.uv  = input.uv;"                                                "\r\n"
  "  return output;"                                                        "\r\n"
  "}"                                                                       "\r\n"
  "SamplerState      sampler0;"                                             "\r\n"
  "Texture2D<float4> texture0;"                                             "\r\n"
  "float4 MainPS(PS_INPUT input) : SV_Target"                               "\r\n"
  "{ float4 out_col = texture0.Sample(sampler0, input.uv);"                 "\r\n"
  "  return out_col;"                                                       "\r\n"
  "}"                                                                       "\r\n";

  UINT CompilationFlags=D3DCOMPILE_DEBUG|D3DCOMPILE_SKIP_OPTIMIZATION;
  ID3DBlob *ShaderBlob,*MessageBlob;

  if(SUCCEEDED(D3DCompile(ShaderCode,strlen(ShaderCode),"rx::vertex-shader",0,0,"MainVS","vs_5_0",
    CompilationFlags,0,&ShaderBlob,&MessageBlob)))
  { ID3D11Device_CreateVertexShader(rx.Device,ID3D10Blob_GetBufferPointer(ShaderBlob),
      ID3D10Blob_GetBufferSize(ShaderBlob),NULL,&rx.VertexShader);
  } else
  { OutputDebugStringA((char*)ID3D10Blob_GetBufferPointer(MessageBlob));
    OutputDebugStringA("\r\n");
  }

  D3D11_INPUT_ELEMENT_DESC LayoutElements[]=
  { (D3D11_INPUT_ELEMENT_DESC){"POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    (D3D11_INPUT_ELEMENT_DESC){"COLOR",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    (D3D11_INPUT_ELEMENT_DESC){"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
  };

  ID3D11Device_CreateInputLayout(rx.Device,LayoutElements,ARRAYSIZE(LayoutElements),
    ID3D10Blob_GetBufferPointer(ShaderBlob),ID3D10Blob_GetBufferSize(ShaderBlob),&rx.VertexShaderInputLayout);

  if(SUCCEEDED(D3DCompile(ShaderCode,strlen(ShaderCode),"rx::pixel-shader",0,0,"MainPS","ps_5_0",
    CompilationFlags,0,&ShaderBlob,&MessageBlob)))
  { ID3D11Device_CreatePixelShader(rx.Device,ID3D10Blob_GetBufferPointer(ShaderBlob),
      ID3D10Blob_GetBufferSize(ShaderBlob),NULL,&rx.PixelShader);
  } else
  { OutputDebugStringA((char*)ID3D10Blob_GetBufferPointer(MessageBlob));
    OutputDebugStringA("\r\n");
  }

  D3D11_SAMPLER_DESC SamplerInfo;
  ZeroMemory(&SamplerInfo,sizeof(SamplerInfo));
  SamplerInfo.AddressU=D3D11_TEXTURE_ADDRESS_WRAP;
  SamplerInfo.AddressV=D3D11_TEXTURE_ADDRESS_WRAP;
  SamplerInfo.AddressW=D3D11_TEXTURE_ADDRESS_WRAP;

  SamplerInfo.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  ID3D11Device_CreateSamplerState(rx.Device,&SamplerInfo,&rx.linear_sampler.Sampler);

  SamplerInfo.Filter=D3D11_FILTER_MIN_MAG_MIP_POINT;
  ID3D11Device_CreateSamplerState(rx.Device,&SamplerInfo,&rx.point_sampler.Sampler);
}
#endif
