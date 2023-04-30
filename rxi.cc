#ifndef _RXTEXTURE_HEADER
#define _RXTEXTURE_HEADER

typedef struct rxoffline_texture_t rxoffline_texture_t;
typedef struct rxoffline_texture_t
{  int   size_x, size_y;
   int   format;
   int   stride;
  void * memory;
} rxoffline_texture_t;

rxoffline_texture_t
rxsummon_texture(
  const char *name);

typedef struct rxtexture_t rxtexture_t;
typedef struct rxtexture_t
{ rxunknown_t unknown;
        int size_x, size_y;
        int format;
} rxtexture_t;

ccfunc ID3D11Texture2D *
rxdevice_allocate_texture_ex(
          int             size_x,
          int             size_y,
  DXGI_FORMAT             format,
          int             stride,
          void           *memory,
  D3D11_USAGE              usage,
          int         bind_flags,
          int  host_access_flags,
          int       sample_count,
          int     sample_quality );

ccfunc rxtexture_t
rxdevice_create_texture_ex(
          int  size_x,
          int  size_y,
  DXGI_FORMAT  format,
          int  stride,
        void  *memory);

ccfunc void
rxdevice_bind_texture(
  rxtexture_t texture,
          int    slot);

ccfunc rxtexture_t
rxcreate_texture(
  int  size_x,
  int  size_y,
  int  format);

ccfunc rxborrowed_t
rxborrow_texture(
  rxtexture_t texture);

ccfunc rxtexture_t
rxupload_texture(
  rxoffline_texture_t texture);

ccfunc rxtexture_t
rxupload_texture_ondisk(
  const char *name);

typedef struct rxsampler_t rxsampler_t;
typedef struct rxsampler_t
{
  ID3D11SamplerState *unknown;
} rxsampler_t;

ccfunc void
rxdevice_bind_sampler(
  rxsampler_t sampler,
          int    slot);

#endif

#ifdef _RXTEXTURE_IMPLEMENTATION

rxoffline_texture_t
rxsummon_texture(
  const char *name)
{
  // todo!!: support other formats
  // todo!!: the texture memory may not necessarily come from a file
  // todo!!: provide own memory
  rxoffline_texture_t t;
  t.format=rxRGBA8888;
  t.memory=stbi_load(name,&t.size_x,&t.size_y,0,4);
  t.stride=t.size_x*4;
  return t;
}

rxtexture_t
rxupload_texture(
  rxoffline_texture_t local)
{
  return rxdevice_create_texture_ex(local.size_x,local.size_y,local.format,local.stride,local.memory);
}

rxtexture_t
rxupload_texture_ondisk(
  const char *name)
{
  rxoffline_texture_t local=rxsummon_texture(name);

  rxtexture_t texture=rxupload_texture(local);

  stbi_image_free(local.memory);

  return texture;
}

void rxdevice_bind_texture(
  rxtexture_t texture, int slot)
{
  ccassert(texture.unknown != 0);

  // todo!!:
  ID3D11ShaderResourceView *View;
  if(SUCCEEDED(ID3D11DeviceChild_QueryInterface(texture.unknown,&IID_ID3D11ShaderResourceView,&View)))
  {
    ID3D11DeviceChild_Release(View);

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

void
rxdevice_bind_sampler(
  rxsampler_t sampler, int slot)
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

void
rxdelete_texture(
  rxtexture_t texture)
{
  ID3D11ShaderResourceView *View;
  ID3D11Resource           *Resource;

  if(SUCCEEDED(IUnknown_QueryInterface(texture.unknown,&IID_ID3D11ShaderResourceView,&View)))
  {
    ID3D11View_GetResource(View,&Resource);

    IUnknown_Release(Resource);
    IUnknown_Release(View);
  }
}

ID3D11Texture2D *
rxdevice_allocate_texture_ex(
          int             size_x,
          int             size_y,
  DXGI_FORMAT             format,
          int             stride,
          void           *memory,
  D3D11_USAGE              usage,
          int         bind_flags,
          int  host_access_flags,
          int       sample_count,
          int     sample_quality )
{
  ccassert(size_x >= 1 || cctraceerr("invalid size x"));
  ccassert(size_y >= 1 || cctraceerr("invalid size y"));

  D3D11_TEXTURE2D_DESC TextureI;
  TextureI.                 Width=size_x;
  TextureI.                Height=size_y;
  TextureI.             MipLevels=1;
  TextureI.             ArraySize=1;
  TextureI.                Format=format;
  TextureI.SampleDesc.      Count=sample_count;
  TextureI.SampleDesc.    Quality=sample_quality;
  TextureI.                 Usage=usage;
  TextureI.             BindFlags=bind_flags;
  TextureI.        CPUAccessFlags=host_access_flags;
  TextureI.             MiscFlags=0;

  D3D11_SUBRESOURCE_DATA SubresourceI;
  ZeroMemory(&SubresourceI,sizeof(SubresourceI));
  SubresourceI.    pSysMem=memory;
  SubresourceI.SysMemPitch=stride;

  ID3D11Texture2D *Texture2D;
  if(FAILED(ID3D11Device_CreateTexture2D(rx.Device,&TextureI,memory?&SubresourceI:NULL,&Texture2D)))
    Texture2D=NULL;

  return Texture2D;
}

rxtexture_t
rxdevice_create_texture_ex(int w, int h, int f, int s, void *m)
{
  ID3D11Texture2D *Texture2D;
  Texture2D=rxdevice_allocate_texture_ex(w,h,f,s,m,
    D3D11_USAGE_DYNAMIC,D3D11_BIND_SHADER_RESOURCE,D3D11_CPU_ACCESS_WRITE,1,0);

  D3D11_SHADER_RESOURCE_VIEW_DESC ViewI;
  ZeroMemory(&ViewI,sizeof(ViewI));
  ViewI.       Format=DXGI_FORMAT_UNKNOWN;
  ViewI.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
  ViewI.    Texture2D.MostDetailedMip=0;
  ViewI.    Texture2D.      MipLevels=1;

  ID3D11ShaderResourceView *ShaderResourceView;
  ID3D11Device_CreateShaderResourceView(rx.Device,
    (ID3D11Resource *)Texture2D,&ViewI,&ShaderResourceView);

  rxtexture_t result;
  result.unknown=(rxunknown_t)ShaderResourceView;
  result. size_x=w;
  result. size_y=h;
  result. format=f;

  return result;
}

rxtexture_t rxcreate_texture(int w, int h, int f)
{
  return rxdevice_create_texture_ex(w,h,f,0,0);
}

rxborrowed_t rxborrow_texture(rxtexture_t texture)
{
  rxborrowed_t result;
  result.resource=0;
  result.  stride=0;
  result.  memory=0;

  // todo: debugonly! ensures this is a valid shader resource view!
  ID3D11ShaderResourceView *View;
  ID3D11Resource           *Resource;

  if(FAILED(IUnknown_QueryInterface(texture.unknown,&IID_ID3D11ShaderResourceView,&View)))
    goto error;

  ID3D11ShaderResourceView_GetResource(View,&Resource);

  D3D11_MAPPED_SUBRESOURCE MappedAccess;
  if(FAILED(ID3D11DeviceContext_Map(rx.Context,Resource,0,D3D11_MAP_WRITE_DISCARD,0,&MappedAccess)))
    goto error;

  ID3D11DeviceChild_Release(Resource);
  ID3D11DeviceChild_Release(View);

  result.resource=Resource;
  result.  stride=MappedAccess.RowPitch;
  result.  memory=MappedAccess.pData;

  ccassert(result.stride != 0);
  ccassert(result.memory != 0);

error:
  return result;
}

#endif