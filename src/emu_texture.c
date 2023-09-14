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

ccfunc ccinle void *
Emu_texture_borrow(Emu_texture_t *texture, int *stride)
{
  D3D11_MAPPED_SUBRESOURCE memory_d3d;
  ID3D11DeviceContext_Map(
    rx.d3d11.ctx,texture->d3d11.resource,0,D3D11_MAP_WRITE_DISCARD,0,&memory_d3d);

  texture->mapped.stride = memory_d3d.RowPitch;
  texture->mapped.memory = memory_d3d.pData;

	if(stride != NULL) {
		*stride = memory_d3d.RowPitch;
	}

	return memory_d3d.pData;
}

ccfunc ccinle void
Emu_texture_return(Emu_texture_t *texture)
{
  ID3D11DeviceContext_Unmap(rx.d3d11.ctx,texture->d3d11.resource,0);
}

ccfunc ccinle Emu_texture_config_t
Emu_texture_config_init(
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
  Emu_texture_config_t config;
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

  config.d3d11.shader_target = NULL;
  config.d3d11.color_target = NULL;
  config.d3d11.depth_target = NULL;
  config.d3d11.resource = NULL;
  return config;
}

ccfunc emu_error
Emu_texture_init(
  Emu_texture_t *texture, Emu_texture_config_t *config)
{
 	emu_error error = EMU_ERROR_NONE;

  if(config->d3d11.texture_2d == NULL) {
	  /* check this properly based on d3dxx's version spec #todo */
	  ccassert((config->size_x >= 1 &&
	            config->size_x <= 16384)
	    || cctraceerr("invalid size x %i", config->size_x));
	  ccassert((config->size_y >= 1 &&
	            config->size_y <= 16384)
	    || cctraceerr("invalid size y %i", config->size_y));

	  D3D11_TEXTURE2D_DESC config_d3d;
	  config_d3d.Width=config->size_x;
	  config_d3d.Height=config->size_y;
	  config_d3d.MipLevels=1;
	  config_d3d.ArraySize=1;
	  config_d3d.Format=config->format;
	  config_d3d.SampleDesc.Count=1;
	  config_d3d.SampleDesc.Quality=0;
	  config_d3d.Usage=config->memtype;
	  config_d3d.BindFlags=config->useflag;
	  config_d3d.CPUAccessFlags=config->memflag;
	  config_d3d.MiscFlags=0;

	  if(config->memory != NULL) {

		  D3D11_SUBRESOURCE_DATA data_config_d3d;
		  ZeroMemory(&data_config_d3d,sizeof(data_config_d3d));
		  data_config_d3d.    pSysMem=config->memory;
		  data_config_d3d.SysMemPitch=config->stride;

			ID3D11Device_CreateTexture2D(rx.d3d11.dev,
				&config_d3d,&data_config_d3d,&config->d3d11.texture_2d);
	  } else {

	  	ID3D11Device_CreateTexture2D(rx.d3d11.dev,
				&config_d3d,NULL,&config->d3d11.texture_2d);
	  }

	  if(config->d3d11.texture_2d == NULL) {

	  	error = EMU_ERROR_CREATE_TEXTURE;
	  	goto leave;
	  }
  }

  if(config->d3d11.depth_target == NULL) {
  	if((config->useflag & D3D11_BIND_DEPTH_STENCIL) != 0) {

	  	ID3D11Device_CreateDepthStencilView(rx.d3d11.dev,
	    	config->d3d11.resource,NULL,&config->d3d11.depth_target);

	  	if(config->d3d11.depth_target == NULL) {
    		error = EMU_ERROR_CREATE_TEXTURE;
	    	goto leave;
	    }
		}
	}

  if(config->d3d11.color_target == NULL) {
  	if((config->useflag & D3D11_BIND_RENDER_TARGET) != 0) {

  		D3D11_RENDER_TARGET_VIEW_DESC view_config_d3d;
	    ZeroMemory(&view_config_d3d,sizeof(view_config_d3d));
	    view_config_d3d.Format       =DXGI_FORMAT_UNKNOWN;
	    view_config_d3d.ViewDimension=D3D11_RTV_DIMENSION_TEXTURE2D;

	    ID3D11Device_CreateRenderTargetView(rx.d3d11.dev,
        config->d3d11.resource,&view_config_d3d,&config->d3d11.color_target);

	    if(config->d3d11.color_target == NULL) {
    		error = EMU_ERROR_CREATE_TEXTURE;
	    	goto leave;
	    }
  	}
  }

  if(config->d3d11.shader_target == NULL) {
    if((config->useflag & D3D11_BIND_SHADER_RESOURCE) != 0) {

      D3D11_SHADER_RESOURCE_VIEW_DESC texture_view_config_d3d;
      ZeroMemory(&texture_view_config_d3d,sizeof(texture_view_config_d3d));
      texture_view_config_d3d.       Format=DXGI_FORMAT_UNKNOWN;
      texture_view_config_d3d.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
      texture_view_config_d3d.    Texture2D.MostDetailedMip=0;
      texture_view_config_d3d.    Texture2D.      MipLevels=1;

      ID3D11Device_CreateShaderResourceView(rx.d3d11.dev,
        config->d3d11.resource,&texture_view_config_d3d,&config->d3d11.shader_target);

      if(config->d3d11.shader_target == NULL) {
    		error = EMU_ERROR_CREATE_TEXTURE;
	    	goto leave;
	    }
    }
  }

leave:
  texture->size_x = config->size_x;
  texture->size_y = config->size_y;
  texture->format = config->format;
  texture->samples = config->samples;
  texture->quality = config->quality;
  texture->d3d11.resource = config->d3d11.resource;
  texture->d3d11.shader_target = config->d3d11.shader_target;
  texture->d3d11.color_target = config->d3d11.color_target;
  texture->d3d11.depth_target = config->d3d11.depth_target;

  return error;
}

void
Emu_texture_delete( Emu_texture_t *texture )
{
  rxunknown_delete(texture->d3d11.shader_target);
  rxunknown_delete(texture->d3d11.resource);

  EMU_FREE(texture,NULL);
}

ccfunc ccinle Emu_texture_t *
Emu_texture_create( Emu_texture_config_t *config )
{
	Emu_texture_t *result = EMU_MALLOC(sizeof(Emu_texture_t),NULL);

	emu_error error = Emu_texture_init(result,config);

	if (error != EMU_ERROR_NONE) {

		Emu_texture_delete(result);

		result = NULL;
	}

	return result;
}

ccfunc ccinle Emu_texture_t *
Emu_texture_create_depth_target(
  int size_x, int size_y, int format)
{
	Emu_texture_config_t config =
		Emu_texture_config_init(size_x,size_y,format,0,NULL,1,0,
      D3D11_USAGE_DEFAULT,D3D11_BIND_DEPTH_STENCIL,0);

  return Emu_texture_create( &config );
}

ccfunc ccinle Emu_texture_t *
Emu_texture_create_color_target(
  int size_x, int size_y, int format, int samples, int quality)
{
	Emu_texture_config_t config =
		Emu_texture_config_init(size_x,size_y,format,0,0,samples,quality,
      D3D11_USAGE_DEFAULT,D3D11_BIND_RENDER_TARGET,0);

  return Emu_texture_create( &config );
}

ccfunc ccinle Emu_texture_t *
Emu_texture_create_simple(
  int size_x, int size_y, int format, int stride, void *memory)
{
	Emu_texture_config_t config =
		Emu_texture_config_init(size_x,size_y,format,stride,memory,1,0,
      D3D11_USAGE_DYNAMIC,D3D11_BIND_SHADER_RESOURCE,D3D11_CPU_ACCESS_WRITE);

  return Emu_texture_create( &config );
}

Emu_texture_memory_t
Emu_texture_load(
  const char *name)
{
  const char *ext = ccfileext(name);

  Emu_texture_memory_t result;
  memset(&result,0,sizeof(result));

  /* XXX this is google type stuff */
  if(strcmp(ext,"png")&&
     strcmp(ext,"jpg")&&
     strcmp(ext,"bmp")) return result;

  /* XXX support other formats, use own memory */
  void *memory=stbi_load(name,&result.size_x,&result.size_y,0,4);

  result.format=EMU_FORMAT_R8G8B8A8_UNORM;
  result.memory=memory;
  result.stride=result.size_x*4;
  return result;
}

Emu_texture_t *
Emu_texture_memory_upload(Emu_texture_memory_t local)
{
  Emu_texture_t *result =
  	Emu_texture_create_simple(
  		local.size_x,local.size_y,
  		local.format,
  		local.stride,local.memory);

	return result;
}

ccfunc ccinle void
Emu_texture_copy(
  Emu_texture_t *dst,
  Emu_texture_t *src )
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

Emu_texture_memory_t
Emu_texture_memory_create( int size_x, int size_y, int format )
{
	int bpp = 0;
	if (format == EMU_FORMAT_R8_UNORM) {
		bpp = 1;
	} else
	if (format == EMU_FORMAT_R8G8B8A8_UNORM) {
		bpp = 4;
	}

	ccassert(bpp != 0);

	Emu_texture_memory_t result;
	result.size_x = size_x;
	result.size_y = size_y;
	result.format = format;
	result.stride = size_x * bpp;
	result.memory = EMU_MALLOC(size_y * size_x * bpp, NULL);

	return result;
}
