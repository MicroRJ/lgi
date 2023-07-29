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

/* XXX this could use a better name */
rxoffline_texture_t
rxtexture_load(
  const char *name)
{
  const char *ext = ccfileext(name);

  rxoffline_texture_t result;
  memset(&result,0,sizeof(result));

  /* XXX this is google type stuff */
  if(strcmp(ext,"png")&&
     strcmp(ext,"jpg")&&
     strcmp(ext,"bmp")) return result;

  /* XXX support other formats, use own memory */
  void *memory=stbi_load(name,&result.size_x,&result.size_y,0,4);

  result.format=rxRGBA8888;
  result.memory=memory;
  result.stride=result.size_x*4;
  return result;
}

rxtexture_t
rxtexture_upload(
  rxoffline_texture_t local)
{
  return rxtexture_create_ex(local.size_x,local.size_y,local.format,local.stride,local.memory);
}

rxtexture_t
rxtexture_upload_byname(
  const char *name)
{
  rxoffline_texture_t local=rxtexture_load(name);

  rxtexture_t texture;
  memset(&texture,0,sizeof(texture));

  if(local.memory != ccnull)
  {
    texture = rxtexture_upload(local);

    stbi_image_free(local.memory);
  }

  return texture;
}

void
rxtexture_delete(
  rxtexture_t texture)
{
  ID3D11ShaderResourceView *View;
  ID3D11Resource           *Resource;

  if(SUCCEEDED(
  		IUnknown_QueryInterface(texture.unknown,&IID_ID3D11ShaderResourceView,&View)))
  {
    ID3D11View_GetResource(View,&Resource);

    IUnknown_Release(Resource);
    IUnknown_Release(View);
  }
}

rxtexture_t
rxtexture_create_untyped(
          int              size_x,
          int              size_y,
  DXGI_FORMAT              format,
          int              stride,
          void           * memory,
  D3D11_USAGE               usage,
          int          bind_flags,
          int   host_access_flags,
          int        sample_count,
          int      sample_quality )
{
  ccassert((size_x >= 1 && size_x <= 16384) || cctraceerr("invalid size x %i", size_x));
  ccassert((size_y >= 1 && size_y <= 16384) || cctraceerr("invalid size y %i", size_y));

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

  ID3D11Texture2D *Texture=NULL;

  if(SUCCEEDED(
      ID3D11Device_CreateTexture2D(rx.Device,&TextureI,memory?&SubresourceI:NULL,&Texture)))
  {
    rxarticle_t *article=rxarticle_create(Texture);

    if(bind_flags&D3D11_BIND_SHADER_RESOURCE)
    {
      D3D11_SHADER_RESOURCE_VIEW_DESC ViewI;
      ZeroMemory(&ViewI,sizeof(ViewI));
      ViewI.       Format=DXGI_FORMAT_UNKNOWN;
      ViewI.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
      ViewI.    Texture2D.MostDetailedMip=0;
      ViewI.    Texture2D.      MipLevels=1;

      ID3D11ShaderResourceView *View;
      if(SUCCEEDED(
          ID3D11Device_CreateShaderResourceView(rx.Device,
            (ID3D11Resource*)Texture,&ViewI,&View)))
      {
        rxarticle_attach(article,rxlinkage_kSHADER_RESOURCE_VIEW,View);
      }
    }

    if(bind_flags&D3D11_BIND_RENDER_TARGET)
    {
      D3D11_RENDER_TARGET_VIEW_DESC ViewI;
      ZeroMemory(&ViewI,sizeof(ViewI));
      ViewI.Format       =DXGI_FORMAT_UNKNOWN;
      ViewI.ViewDimension=D3D11_RTV_DIMENSION_TEXTURE2D;

      ID3D11RenderTargetView *View;
      if(SUCCEEDED(
          ID3D11Device_CreateRenderTargetView(rx.Device,
            (ID3D11Resource*)Texture,&ViewI,&View)))
      {
        rxarticle_attach(article,rxlinkage_kRENDER_TARGET_VIEW,View);
      }
    }
  }

  rxtexture_t the_result=RX_TLIT(rxtexture_t){(rxunknown_t)(Texture)};
  the_result. size_x=size_x;
  the_result. size_y=size_y;
  the_result. format=format;
  return the_result;
}

rxtexture_t
rxtexture_create_ex(
  int   size_x,
  int   size_y,
  int   format,
  int   stride,
  void *memory)
{
  return
    rxtexture_create_untyped(size_x,size_y,format,stride,memory,
      D3D11_USAGE_DYNAMIC,D3D11_BIND_SHADER_RESOURCE,D3D11_CPU_ACCESS_WRITE,1,0);
}

rxtexture_t
rxtexture_create(int size_x, int size_y, int format)
{
  return rxtexture_create_ex(size_x,size_y,format,0,0);
}

rxborrowed_t
rxtexture_borrow(rxtexture_t texture)
{
  rxborrowed_t result;
  memset(&result,0,sizeof(result));

  ID3D11Resource *Resource;
  if(FAILED(IUnknown_QueryInterface(texture.unknown,&IID_ID3D11Resource,&Resource)))
    goto error;

  D3D11_MAPPED_SUBRESOURCE MappedAccess;
  if(FAILED(ID3D11DeviceContext_Map(rx.Context,Resource,0,D3D11_MAP_WRITE_DISCARD,0,&MappedAccess)))
    goto error;

  ID3D11DeviceChild_Release(Resource);

  result.resource=Resource;
  result.  stride=MappedAccess.RowPitch;
  result.  memory=MappedAccess.pData;

  ccassert(result.stride != 0);
  ccassert(result.memory != 0);

error:
  return result;
}

void
rxtexture_apply(
  rxtexture_t texture, int slot)
{
  /* Please remove this - XXXX - the one called rj  */
  rxarticle_t *tangible=cctblgetP(rx.instance_table,texture.unknown);
  ccassert(ccerrnon());

  ID3D11ShaderResourceView *View;
  View = (ID3D11ShaderResourceView *)tangible->linkage[rxlinkage_kSHADER_RESOURCE_VIEW];

	if(rxshader_typeof_vertex(rx.shader))
	{
	  ID3D11DeviceContext_VSSetShaderResources(rx.Context,slot,1,&View);
	} else
	if(rxshader_typeof_pixel(rx.shader))
	{
	  ID3D11DeviceContext_PSSetShaderResources(rx.Context,slot,1,&View);
	} else
	if(rxshader_typeof_compute(rx.shader))
	{
	  ID3D11DeviceContext_CSSetShaderResources(rx.Context,slot,1,&View);
	}
}