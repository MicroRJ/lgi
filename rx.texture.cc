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

#if 0
enum
{ EMU_HOST_READ_BIT         = 1 << 0,
	EMU_HOST_WRITE_BIT        = 1 << 1,
	EMU_GPU_READ_BIT          = 1 << 2,
	EMU_GPU_WRITE_BIT         = 1 << 3,

	EMU_CPU_READ_BIT          = EMU_HOST_READ_BIT,
	EMU_CPU_WRITE_BIT         = EMU_HOST_WRITE_BIT,

	EMU_COLOR_ATTACHMENT_BIT  = 1 << 4,
	EMU_DEPTH_ATTACHMENT_BIT  = 1 << 5,

	EMU_USAGE_DEFAULT_BIT     = EMU_GPU_READ_BIT|EMU_GPU_WRITE_BIT,
	EMU_USAGE_DYNAMIC_BIT     = EMU_CPU_WRITE_BIT|EMU_GPU_READ_BIT,
	EMU_USAGE_IMMUTABLE_BIT   = EMU_GPU_READ_BIT,
	EMU_USAGE_STAGING_BIT     = EMU_GPU_WRITE_BIT|EMU_CPU_READ_BIT,
};
#endif

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
          int    memflag )
{
  rxtexture_config_t config;
  config.size_x     = size_x;
  config.size_y     = size_y;
  config.format     = format;
  config.stride     = stride;
  config.memory     = memory;
  config.samples    = samples;
  config.quality    = quality;
  config.memtype    = memtype;
  config.useflag    = useflag;
  config.memflag    = memflag;

  config.d3d11.view     = NULL;
  config.d3d11.resource = NULL;
  return config;
}

ccfunc int
rxtexture_init(
  rxtexture_t         *texture,
  rxtexture_config_t  *config )
{
  ZeroMemory(texture,sizeof(*texture));

  /* todo: check this properly based on d3d11's version spec */
  ccassert((config->size_x >= 1 &&
            config->size_x <= 16384)
    || cctraceerr("invalid size x %i", config->size_x));
  ccassert((config->size_y >= 1 &&
            config->size_y <= 16384)
    || cctraceerr("invalid size y %i", config->size_y));

  D3D11_TEXTURE2D_DESC texture_config_d3d;
  texture_config_d3d.                 Width=config->size_x;
  texture_config_d3d.                Height=config->size_y;
  texture_config_d3d.             MipLevels=1;
  texture_config_d3d.             ArraySize=1;
  texture_config_d3d.                Format=config->format;
  texture_config_d3d.SampleDesc.      Count=1;
  texture_config_d3d.SampleDesc.    Quality=0;
  texture_config_d3d.                 Usage=config->memtype;
  texture_config_d3d.             BindFlags=config->useflag;
  texture_config_d3d.        CPUAccessFlags=config->memflag;
  texture_config_d3d.             MiscFlags=0;

  D3D11_SUBRESOURCE_DATA texture_upload_config_d3d;
  ZeroMemory(&texture_upload_config_d3d,sizeof(texture_upload_config_d3d));
  texture_upload_config_d3d.    pSysMem=config->memory;
  texture_upload_config_d3d.SysMemPitch=config->stride;

  ID3D11Device_CreateTexture2D(rx.d3d11.dev,
    &texture_config_d3d,
    config->memory ? &texture_upload_config_d3d : NULL,
    &config->d3d11.texture_2d);

  ccassert(config->d3d11.texture_2d != NULL);

  if(config->d3d11.view == NULL)
  {
    if((config->useflag & D3D11_BIND_SHADER_RESOURCE) != 0)
    {
      D3D11_SHADER_RESOURCE_VIEW_DESC texture_view_config_d3d;
      ZeroMemory(&texture_view_config_d3d,sizeof(texture_view_config_d3d));
      texture_view_config_d3d.       Format=DXGI_FORMAT_UNKNOWN;
      texture_view_config_d3d.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
      texture_view_config_d3d.    Texture2D.MostDetailedMip=0;
      texture_view_config_d3d.    Texture2D.      MipLevels=1;

      ID3D11Device_CreateShaderResourceView(rx.d3d11.dev,
        config->d3d11.resource,&texture_view_config_d3d,&config->d3d11.view);

      ccassert(config->d3d11.view != NULL);
    }
  }

  texture->size_x         = config->size_x;
  texture->size_y         = config->size_y;
  texture->format         = config->format;
  texture->samples        = config->samples;
  texture->quality        = config->quality;
  texture->d3d11.view     = config->d3d11.view;
  texture->d3d11.resource = config->d3d11.resource;
  return cctrue;
}

ccfunc ccinle rxtexture_t
rxtexture_create_exex(
          int    size_x,
          int    size_y,
  DXGI_FORMAT    format,
          int    stride,
          void * memory,
  /* todo: these could all be merged into a single flag */
  D3D11_USAGE    memtype,
          int    useflag,
          int    memflag )
{

  rxtexture_config_t config =
    rxtexture_config_init(
      size_x,size_y,format,stride,memory,1,0,memtype,useflag,memflag);

  rxtexture_t texture;
  rxtexture_init(&texture,&config);

  return texture;
}

ccfunc ccinle rxtexture_t
rxtexture_create_ex(
    int   size_x,
    int   size_y,
    int   format,
    int   stride,
    void *memory )
{
  return
    rxtexture_create_exex(size_x,size_y,format,stride,memory,
      D3D11_USAGE_DYNAMIC,D3D11_BIND_SHADER_RESOURCE,D3D11_CPU_ACCESS_WRITE);
}

ccfunc ccinle rxtexture_t
rxtexture_create_empty()
{
	rxtexture_t texture;
	memset(&texture,0,sizeof(texture));
	return texture;
}

rxtexture_t
rxtexture_create(int size_x, int size_y, int format)
{
  return rxtexture_create_ex(size_x,size_y,format,0,0);
}


rxtexture_memory_t
rxtexture_load(
  const char *name)
{
  const char *ext = ccfileext(name);

  rxtexture_memory_t result;
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
rxtexture_memory_upload(
  rxtexture_memory_t local)
{
  return rxtexture_create_ex(local.size_x,local.size_y,local.format,local.stride,local.memory);
}

rxtexture_t
rxtexture_upload(
  const char *name)
{
  rxtexture_memory_t local=rxtexture_load(name);

  rxtexture_t texture;
  memset(&texture,0,sizeof(texture));

  if(local.memory != ccnull)
  {
    texture = rxtexture_memory_upload(local);

    stbi_image_free(local.memory);
  }

  return texture;
}

void
rxtexture_delete(
  rxtexture_t texture)
{
  rxunknown_delete(texture.d3d11.view);
  rxunknown_delete(texture.d3d11.resource);
}


/* todo: perhaps we should have a specific type for this */
rxborrowed_t
rxtexture_borrow(rxtexture_t texture)
{
  rxborrowed_t result;
  memset(&result,0,sizeof(result));

  D3D11_MAPPED_SUBRESOURCE memory_d3d;
  if(FAILED(
      ID3D11DeviceContext_Map(
        rx.d3d11.ctx,
        texture.d3d11.resource,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &memory_d3d)))
  {
    goto error;
  }

  result.        stride = memory_d3d.RowPitch;
  result.        memory = memory_d3d.pData;
  result.d3d11.resource = texture.d3d11.resource;

  ccassert(result.stride != 0);
  ccassert(result.memory != 0);

error:
  return result;
}


rxdrawing_texture_t
rxdrawing_texture_create(
  int size_x, int size_y, int format, int samples, int quality)
{
  rxdrawing_texture_t surface;
  ZeroMemory(&surface,sizeof(surface));

  rxtexture_config_t texture_config =
    rxtexture_config_init(size_x,size_y,format,0,NULL,samples,quality,
      D3D11_USAGE_DEFAULT,D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET,0);

  rxtexture_t texture;
  if(rxtexture_init(&texture,&texture_config))
  {
    D3D11_RENDER_TARGET_VIEW_DESC render_target_config_d3d;
    ZeroMemory(&render_target_config_d3d,sizeof(render_target_config_d3d));
    render_target_config_d3d.Format       =DXGI_FORMAT_UNKNOWN;
    render_target_config_d3d.ViewDimension=D3D11_RTV_DIMENSION_TEXTURE2D;

    ID3D11RenderTargetView *render_target_view_d3d;
    if(SUCCEEDED(
        ID3D11Device_CreateRenderTargetView(rx.d3d11.dev,
          texture_config.d3d11.resource,&render_target_config_d3d,&render_target_view_d3d)))
    {
      surface.texture    = texture;
      surface.d3d11.view = render_target_view_d3d;
    }
  }

  return surface;
}

ccfunc ccinle void
rxtexture_copy(
  rxtexture_t *dst,
  rxtexture_t *src )
{
  if( dst->d3d11.resource !=
      src->d3d11.resource )
  {
  	if(src->samples <= 1)
  	{
	    ID3D11DeviceContext_CopyResource(rx.d3d11.ctx,
	      dst->d3d11.resource,
	      src->d3d11.resource);
  	} else
  	{
		  ID3D11DeviceContext_ResolveSubresource(rx.d3d11.ctx,
		  	dst->d3d11.resource, 0,
		  	src->d3d11.resource, 0, dst->format);
  	}
  }
}
