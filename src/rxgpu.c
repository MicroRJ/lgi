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

rxAPI void
rxGPU_returnBuffer(rxGPU_Buffer *lpBuffer) {
	rx_assert(lpBuffer->mapped.memory != NULL);
	if (lpBuffer->mapped.memory != NULL) {
		ID3D11DeviceContext_Unmap(rx.d3d11.ctx,lpBuffer->d3d11.resource,0);
		lpBuffer->mapped.memory = NULL;
	}
}

void *
rxGPU_borrowBuffer(rxGPU_Buffer *lpBuffer, int *lpStride) {
	rx_assert(lpBuffer->mapped.memory == NULL);

	if (lpBuffer->mapped.memory == NULL) {

		ID3D11Resource *lpResource = lpBuffer->d3d11.resource;

#if defined(_DEBUG)
		{
			HRESULT error = IUnknown_QueryInterface(lpResource,&IID_ID3D11Resource,(void**)&lpResource);
			if FAILED(error) {
				__debugbreak();
			}
		}
#endif

		D3D11_MAPPED_SUBRESOURCE memory_d3d;
		HRESULT error = ID3D11DeviceContext_Map(rx.d3d11.ctx,lpResource,0,D3D11_MAP_WRITE_DISCARD,0,&memory_d3d);
		if SUCCEEDED(error) {
			lpBuffer->mapped.stride = memory_d3d.RowPitch;
			lpBuffer->mapped.memory = memory_d3d.pData;
		} else {
			lpBuffer->mapped.stride = 0;
			lpBuffer->mapped.memory = 0;
		}
	}
	if (lpStride != NULL) {
		*lpStride = lpBuffer->mapped.stride;
	}
	return lpBuffer->mapped.memory;
}

rxAPI void
rxGPU_updateUniformBuffer(rxGPU_Uniform_Buffer buffer, void *source, size_t length) {
	void *target = rxGPU_borrowBuffer(buffer.lpBuffer,NULL);
	CopyMemory(target,source,length);
	rxGPU_returnBuffer(buffer.lpBuffer);
}

rxAPI void
rxGPU_deleteVertexBuffer(rxGPU_Vertex_Buffer buffer) {
	rxGPU_close_handle((rxGPU_Handle)buffer.lpBuffer->d3d11.buffer);
	EMU_FREE(buffer.lpBuffer,NULL);
}

rxAPI void
rxGPU_deleteIndexBuffer(rxGPU_Index_Buffer buffer) {
	rxGPU_close_handle((rxGPU_Handle)buffer.lpBuffer->d3d11.buffer);
	EMU_FREE(buffer.lpBuffer,NULL);
}

void *
rxGPU_borrowVertexBuffer(rxGPU_Vertex_Buffer buffer, int *lpStride) {
	return rxGPU_borrowBuffer(buffer.lpBuffer,lpStride);
}

void *
rxGPU_borrowIndexBuffer(rxGPU_Index_Buffer buffer, int *lpStride) {
	return rxGPU_borrowBuffer(buffer.lpBuffer,lpStride);
}


rxGPU_Uniform_Buffer
rxGPU_makeUniformBuffer(unsigned int length, void *memory) {
	D3D11_BUFFER_DESC the_buffer_info;
	the_buffer_info.              Usage = D3D11_USAGE_DYNAMIC;
	the_buffer_info.     CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	the_buffer_info.          MiscFlags = 0;
	the_buffer_info.StructureByteStride = 0;
	the_buffer_info.          BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	the_buffer_info.          ByteWidth = (UINT)((length+15)/16*16);

	ID3D11Buffer *the_buffer;
	ID3D11Device_CreateBuffer(rx.d3d11.dev,&the_buffer_info,NULL,&the_buffer);

	rxGPU_Buffer *buffer = EMU_MALLOC(sizeof(rxGPU_Buffer),NULL);
	buffer->d3d11.buffer = the_buffer;
	buffer->d3d11.shader_target = rxNull;
	buffer->mapped.stride = 0;
	buffer->mapped.memory = 0;

	return RX_TLIT(rxGPU_Uniform_Buffer){buffer};
}

rxGPU_Index_Buffer
rxGPU_makeIndexBuffer(int index_size, int index_count) {

	D3D11_BUFFER_DESC the_buffer_info;
	the_buffer_info.              Usage = D3D11_USAGE_DYNAMIC;
	the_buffer_info.     CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	the_buffer_info.          MiscFlags = 0;
	the_buffer_info.StructureByteStride = 0;
	the_buffer_info.          BindFlags = D3D11_BIND_INDEX_BUFFER;
	the_buffer_info.          ByteWidth = (UINT)(index_size * index_count);

	ID3D11Buffer *the_buffer;
	ID3D11Device_CreateBuffer(rx.d3d11.dev,&the_buffer_info,NULL,&the_buffer);

	rxGPU_Buffer *buffer = EMU_MALLOC(sizeof(rxGPU_Buffer),NULL);
	buffer->d3d11.buffer = the_buffer;
	buffer->d3d11.shader_target = rxNull;
	buffer->mapped.stride = 0;
	buffer->mapped.memory = 0;

	return RX_TLIT(rxGPU_Index_Buffer){buffer};
}

rxGPU_Vertex_Buffer
rxGPU_makeVertexBuffer(int vertex_size, int vertex_count) {

	D3D11_BUFFER_DESC the_buffer_info;
	the_buffer_info.              Usage = D3D11_USAGE_DYNAMIC;
	the_buffer_info.     CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	the_buffer_info.          MiscFlags = 0;
	the_buffer_info.StructureByteStride = 0;
	the_buffer_info.          BindFlags = D3D11_BIND_VERTEX_BUFFER;
	the_buffer_info.          ByteWidth = (UINT)(vertex_size * vertex_count);

	ID3D11Buffer *the_buffer;
	ID3D11Device_CreateBuffer(rx.d3d11.dev,&the_buffer_info,NULL,&the_buffer);

	rxGPU_Buffer *buffer = EMU_MALLOC(sizeof(rxGPU_Buffer),NULL);
	buffer->d3d11.buffer = the_buffer;
	buffer->d3d11.shader_target = rxNull;
	buffer->mapped.stride = 0;
	buffer->mapped.memory = 0;

	return RX_TLIT(rxGPU_Vertex_Buffer){buffer};
}

rxGPU_Structured_Buffer
rxcreate_struct_buffer(int struct_size, int struct_count) {

	D3D11_BUFFER_DESC the_buffer_info;
	the_buffer_info.              Usage=D3D11_USAGE_DYNAMIC;
	the_buffer_info.     CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
	the_buffer_info.          MiscFlags=D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	the_buffer_info.StructureByteStride=(UINT)(struct_size);
	the_buffer_info.          BindFlags=D3D11_BIND_SHADER_RESOURCE;
	the_buffer_info.          ByteWidth=(UINT)(struct_size * struct_count);

	ID3D11Buffer *the_buffer;
	ID3D11Device_CreateBuffer(rx.d3d11.dev,&the_buffer_info,NULL,&the_buffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC D;
	D.             Format=DXGI_FORMAT_UNKNOWN;
	D.      ViewDimension=D3D11_SRV_DIMENSION_BUFFER;
	D.Buffer.FirstElement=0;
	D.Buffer. NumElements=(UINT)(struct_count);

	ID3D11ShaderResourceView *View;
	ID3D11Device_CreateShaderResourceView(rx.d3d11.dev,(ID3D11Resource*)the_buffer,&D,&View);

	rxGPU_Buffer *buffer = EMU_MALLOC(sizeof(rxGPU_Buffer),NULL);
	buffer->d3d11.buffer = the_buffer;
	buffer->d3d11.shader_target = View;
	buffer->mapped.stride = 0;
	buffer->mapped.memory = 0;

	return RX_TLIT(rxGPU_Structured_Buffer){buffer};
}

/* [[TEXTURE]] */
/* pass in the region to update #todo */
void
rxGPU_update_texture(rxGPU_Texture *texture, rx_Image image) {

	rx_assert(image.format == texture->format);

	int   stride;
	void *memory = rxGPU_borrow_texture(texture,&stride);

	memcpy(memory,image.memory,image.size_y*image.stride);

	rxGPU_return_texture(texture);
}

void
rxGPU_return_texture(rxGPU_Texture *lpTexture) {
	rx_assert(lpTexture->mapped.memory != NULL);
	if (lpTexture->mapped.memory != NULL) {
		ID3D11DeviceContext_Unmap(rx.d3d11.ctx,lpTexture->d3d11.resource,0);
		lpTexture->mapped.memory = NULL;
	}
}

void *
rxGPU_borrow_texture(rxGPU_Texture *texture, int *stride) {
	rx_assert(texture->mapped.memory == NULL);
	if (texture->mapped.memory == NULL) {
		D3D11_MAPPED_SUBRESOURCE memory_d3d;
		HRESULT error = ID3D11DeviceContext_Map(rx.d3d11.ctx,texture->d3d11.resource,0,D3D11_MAP_WRITE_DISCARD,0,&memory_d3d);
		if SUCCEEDED(error) {
			texture->mapped.stride = memory_d3d.RowPitch;
			texture->mapped.memory = memory_d3d.pData;
		} else {
			texture->mapped.stride = 0;
			texture->mapped.memory = 0;
		}
	}

	if (stride != NULL) {
		*stride = texture->mapped.stride;
	}
	return texture->mapped.memory;
}

rxGPU_TEXTURE
rxGPU_make_texture_config(int size_x, int size_y, DXGI_FORMAT format
, int stride, void  *memory, int samples, int quality
, D3D11_USAGE memtype, int useflag, int memflag) {

	rxGPU_TEXTURE config;
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

rxError
rxGPU_init_texture(rxGPU_Texture *texture, rxGPU_TEXTURE *config) {
	rxError error = rxError_kNONE;


	if(config->d3d11.texture_2d == NULL) {
	  /* check this properly based on d3dxx's version spec #todo */
		rx_assert((config->size_x >= 1 &&
		config->size_x <= 16384)
		|| rxLOG_error("invalid size x %i", config->size_x));
		rx_assert((config->size_y >= 1 &&
		config->size_y <= 16384)
		|| rxLOG_error("invalid size y %i", config->size_y));

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

			error = rxError_kCREATE_TEXTURE;
			goto L_leave;
		}
	}

	if(config->d3d11.depth_target == NULL) {
		if((config->useflag & D3D11_BIND_DEPTH_STENCIL) != 0) {

			ID3D11Device_CreateDepthStencilView(rx.d3d11.dev,
			config->d3d11.resource,NULL,&config->d3d11.depth_target);

			if(config->d3d11.depth_target == NULL) {
				error = rxError_kCREATE_TEXTURE;
				goto L_leave;
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
				error = rxError_kCREATE_TEXTURE;
				goto L_leave;
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
				error = rxError_kCREATE_TEXTURE;
				goto L_leave;
			}
		}
	}

	L_leave:
	texture->size_x = config->size_x;
	texture->size_y = config->size_y;
	texture->format = config->format;
	texture->samples = config->samples;
	texture->quality = config->quality;
	texture->d3d11.resource = config->d3d11.resource;
	texture->d3d11.shader_target = config->d3d11.shader_target;
	texture->d3d11.color_target = config->d3d11.color_target;
	texture->d3d11.depth_target = config->d3d11.depth_target;
	texture->mapped.memory = 0;
	texture->mapped.stride = 0;

	return error;
}

void
rxGPU_close_texture(rxGPU_Texture *texture) {
	rxGPU_close_handle((rxGPU_Handle) texture->d3d11.shader_target);
	rxGPU_close_handle((rxGPU_Handle) texture->d3d11.resource);
}

void
rxGPU_delete_texture(rxGPU_Texture *texture) {
	rxGPU_close_texture(texture);
	EMU_FREE(texture,NULL);
}

rxGPU_Texture *
rxGPU_create_texture_ex( rxGPU_TEXTURE *config ) {
	rxGPU_Texture *result = EMU_MALLOC(sizeof(rxGPU_Texture),NULL);
	rxError error = rxGPU_init_texture(result,config);
	if (error != rxError_kNONE) {
		rxGPU_delete_texture(result);
		EMU_FREE(result,NULL);
		result = NULL;
	}
	return result;
}

rxGPU_Texture *
rxGPU_create_depth_target(int size_x, int size_y, int format) {
	rxGPU_TEXTURE config = rxGPU_make_texture_config(size_x,size_y,format,0,NULL,1,0,D3D11_USAGE_DEFAULT,D3D11_BIND_DEPTH_STENCIL,0);
	return rxGPU_create_texture_ex(&config);
}

rxGPU_Texture *
rxGPU_create_color_target(int size_x, int size_y, int format, int samples, int quality) {
	rxGPU_TEXTURE config = rxGPU_make_texture_config(size_x,size_y,format,0,0,samples,quality,D3D11_USAGE_DEFAULT,D3D11_BIND_RENDER_TARGET,0);
	return rxGPU_create_texture_ex(&config);
}

void
rxGPU_copyTexture(rxGPU_Texture *dst, rxGPU_Texture *src) {
	if (dst->d3d11.resource != src->d3d11.resource) {
		if (src->samples <= 1) {
			ID3D11DeviceContext_CopyResource(rx.d3d11.ctx,dst->d3d11.resource,src->d3d11.resource);
		} else {
			ID3D11DeviceContext_ResolveSubresource(rx.d3d11.ctx,dst->d3d11.resource,0,src->d3d11.resource,0,dst->format);
		}
	}
}

rxGPU_Texture *
rxGPU_create_texture(int size_x, int size_y, int format, int stride, void *memory) {
	rxGPU_TEXTURE config = rxGPU_make_texture_config(size_x,size_y,format,stride,memory,1,0,D3D11_USAGE_DYNAMIC,D3D11_BIND_SHADER_RESOURCE,D3D11_CPU_ACCESS_WRITE);
	return rxGPU_create_texture_ex( &config );
}


/* [[SHADER]] */

rxGPU_Shader
rxGPU_makeShaderFromBytecode(int flags, char const *label, size_t hlsl_length, void *hlsl_memory) {
	rxGPU_SHADER config;
	ZeroMemory(&config,sizeof(config));
	config.flags = flags;
	config.label = label;
	config.source.bytecode.length = hlsl_length;
	config.source.bytecode.memory = hlsl_memory;
	rxGPU_Shader shader;
	rxGPU_initShader(&shader,&config);
	return shader;
}


char const *
rxGPU__getShaderEntryName(int shader_flags) {
	if(shader_flags & rxGPU_kVERTEX_SHADER_BIT) {
		return "MainVS";
	} else
	if(shader_flags & rxGPU_kPIXEL_SHADER_BIT) {
		return "MainPS";
	} else
	if(shader_flags & rxGPU_kCOMPUTE_SHADER_BIT) {
		return "MainCS";
	}
	return NULL;
}

char const *
rxGPU__getShaderModelName(int shader_flags) {
	/* [[TODO]]: get this from the device */
	if (shader_flags & rxGPU_kVERTEX_SHADER_BIT) {
		return "vs_5_0";
	} else
	if (shader_flags & rxGPU_kPIXEL_SHADER_BIT) {
		return "ps_5_0";
	} else
	if (shader_flags & rxGPU_kCOMPUTE_SHADER_BIT) {
		return "cs_5_0";
	}
	return NULL;
}

void
rxGPU_initShader(rxGPU_Shader *shader, rxGPU_SHADER *config) {
	ZeroMemory(shader,sizeof(shader));

	ID3DBlob *bytecode_blob_d3d = NULL;
	ID3DBlob *messages_blob_d3d = NULL;
	ID3DBlob *sig_blob_d3d = NULL;
	ID3D11ShaderReflection *reflect_d3d = NULL;
	ID3D11DeviceChild *shader_d3d = NULL;

	if (config->source.bytecode.memory == NULL) {

		if (config->source.compile.debug_label == NULL) {
			config->source.compile.debug_label = config->label;
		}

		char const *model = config->source.compile.model;
		if (model == NULL) {
			config->source.compile.model = model = rxGPU__getShaderModelName(config->flags);
		}
		char const *entry = config->source.compile.entry;
		if (entry == NULL) {
			config->source.compile.entry = entry = rxGPU__getShaderEntryName(config->flags);
		}

		void *memory = config->source.compile.memory;
		size_t length = config->source.compile.length;
		char const *debug_label = config->source.compile.debug_label;
		HRESULT error = D3DCompile(memory,length,debug_label,0,0,entry,model,RX_SHADER_COMPILATION_FLAGS,0,&bytecode_blob_d3d,&messages_blob_d3d);

		rx_assert(entry != 0);
		rx_assert(model != 0);
		rx_assert(memory != NULL);
		rx_assert(length != 0);

		if FAILED(error) {
			rxLOG_error("\n>>> shader compilation error\n%s\n>>> end",(char*)(messages_blob_d3d->lpVtbl->GetBufferPointer(messages_blob_d3d)));
			goto L_leave;
		}

		if(messages_blob_d3d != NULL) {
			rxLOG_error("\n>>> shader compilation warning\n%s\n>>> end",(char*)(messages_blob_d3d->lpVtbl->GetBufferPointer(messages_blob_d3d)));
		}

		config->source.bytecode.memory=bytecode_blob_d3d->lpVtbl->GetBufferPointer(bytecode_blob_d3d);
		config->source.bytecode.length=bytecode_blob_d3d->lpVtbl->   GetBufferSize(bytecode_blob_d3d);
	}

	void *memory = config->source.bytecode.memory;
	size_t length = config->source.bytecode.length;

	if(config->layout.d3d11.layout == NULL) {
		if (config->flags & rxGPU_kPIXEL_SHADER_BIT) {
			if (config->force_create_layout != TRUE) {
				rxLOG_trace("'config.force_create_layout = FALSE': skipping input layout creation for pixel shaders");
				goto L_create_shader;
			}
		}

		if (config->donot_create_layout != FALSE) {
			rxLOG_trace("'config.donot_create_layout = TRUE': skipping input layout creation");
			goto L_create_shader;
		}
		if(config->layout.attr_count == 0) {
	      /* ensure signature blob is present otherwise early out, in my experience
	        even with the /Qstrip_reflect /Qstrip_priv you still get the data you need,
	        either way I think is good to let the user know that they could try
	        and remove those flags in case it fails */

			HRESULT error = D3DGetBlobPart(memory,length,D3D_BLOB_INPUT_SIGNATURE_BLOB,0,&sig_blob_d3d);
			if FAILED(error) {
				rxLOG_error(
				"'microdev.rj@gmail.com': you've reached an unlikely point, the API has failed to acquire [[input signature blob]], this likely means the reflection "
				"data isn't present in the bytecode, try removing the /Qstrip_reflect /Qstrip_priv compilation flags "
				"if you have them; though this isn't expected to solve the issue");
				goto L_leave;
			}
			error = D3DReflect(memory,length,&IID_ID3D11ShaderReflection,(void**)&reflect_d3d);
			if FAILED(error) {
				rxLOG_error(
				"'microdev.rj@gmail.com': you've reached an unlikely point, the API has failed to acquire [[reflection interface]], this likely means the reflection "
				"data isn't present in the bytecode, try removing the /Qstrip_reflect /Qstrip_priv compilation flags "
				"if you have them; though this isn't expected to solve the issue");
				goto L_leave;
			}

			D3D11_SHADER_DESC shader_info_d3d;
			error = reflect_d3d->lpVtbl->GetDesc(reflect_d3d,&shader_info_d3d);
			rx_assert(SUCCEEDED(error));

			config->layout.attr_count = shader_info_d3d.InputParameters;

			int index;
			for (index = 0; index < config->layout.attr_count; index += 1)
			{
				D3D11_SIGNATURE_PARAMETER_DESC param_info_d3d;
				error = reflect_d3d->lpVtbl->GetInputParameterDesc(reflect_d3d,index,&param_info_d3d);
				rx_assert(SUCCEEDED(error));

				DXGI_FORMAT Format = 0;
	        	/* [[TODO]]:  */
				if(param_info_d3d.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
					switch(param_info_d3d.Mask) {
						case 0b1000:
						case 0b0100:
						case 0b0010:
						case 0b0001: Format=DXGI_FORMAT_R32_FLOAT;          break;
						case 0b1100:
						case 0b0011: Format=DXGI_FORMAT_R32G32_FLOAT;       break;
						case 0b0111: Format=0;                              break;
						case 0b1111: Format=DXGI_FORMAT_R32G32B32A32_FLOAT; break;
						default:
						rx_assert(!"not implemented");
					}
				} else {
					rx_assert(!"not implemented");
				}

				D3D11_INPUT_ELEMENT_DESC *elem_config_d3d=&config->layout.attr_array[index];
				elem_config_d3d->Format=Format;
				elem_config_d3d->SemanticName=param_info_d3d.SemanticName;
				elem_config_d3d->SemanticIndex=param_info_d3d.SemanticIndex;
				elem_config_d3d->AlignedByteOffset=D3D11_APPEND_ALIGNED_ELEMENT;
				elem_config_d3d->InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA;
				elem_config_d3d->InputSlot =0;
				elem_config_d3d->InstanceDataStepRate=0;
			}
		}

		int attr_count = config->layout.attr_count;
		D3D11_INPUT_ELEMENT_DESC *attr_array = config->layout.attr_array;
		HRESULT error = ID3D11Device_CreateInputLayout(rx.d3d11.dev,attr_array,attr_count,memory,length,&config->layout.d3d11.layout);
		if FAILED(error) {
			rxLOG_error("failed to create input layout");
			goto L_leave;
		}
	}

	L_create_shader:
	if (config->flags & rxGPU_kVERTEX_SHADER_BIT) {
		HRESULT error = ID3D11Device_CreateVertexShader(rx.d3d11.dev,memory,length,NULL,(ID3D11VertexShader**)(&shader_d3d));
		if FAILED(error) {
			rxLOG_error("failed to create vertex shader");
			goto L_leave;
		}
	} else
	if (config->flags & rxGPU_kPIXEL_SHADER_BIT) {
		HRESULT error = ID3D11Device_CreatePixelShader(rx.d3d11.dev,memory,length,NULL,(ID3D11PixelShader**)(&shader_d3d));
		if FAILED(error) {
			rxLOG_error("failed to create pixel shader");
			goto L_leave;
		}
	} else
	if (config->flags & rxGPU_kCOMPUTE_SHADER_BIT) {
		HRESULT error = ID3D11Device_CreateComputeShader(rx.d3d11.dev,memory,length,NULL,(ID3D11ComputeShader**)(&shader_d3d));
		if FAILED(error) {
			rxLOG_error("failed to create compute shader");
			goto L_leave;
		}
	} else {
		rx_assert(!"error");
	}

	L_leave:
	shader->label = config->label;
	shader->flags = config->flags;
	shader->d3d11.unknown = shader_d3d;
	shader->d3d11.layout  = config->layout.d3d11.layout;

	if(sig_blob_d3d != NULL) {
		sig_blob_d3d->lpVtbl->Release(sig_blob_d3d);
	}
	if(reflect_d3d != NULL) {
		reflect_d3d->lpVtbl->Release(reflect_d3d);
	}
	if(bytecode_blob_d3d != NULL) {
		bytecode_blob_d3d->lpVtbl->Release(bytecode_blob_d3d);
	}
	if(messages_blob_d3d != NULL) {
		messages_blob_d3d->lpVtbl->Release(messages_blob_d3d);
	}
}
