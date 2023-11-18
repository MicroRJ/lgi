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

void
rxIMP_init() {
	rx.imp.varying = rxGPU_makeUniformBuffer(sizeof(rx.imp.pip.var),NULL);

	rx.imp.sha_vtx = rxGPU_makeShaderFromBytecode(rxGPU_kVERTEX_SHADER_BIT, "default_vs", sizeof(rx_vs_shader_bytecode),(void*)rx_vs_shader_bytecode);
	rx.imp.sha_pxl = rxGPU_makeShaderFromBytecode(rxGPU_kPIXEL_SHADER_BIT, "default_ps", sizeof(rx_ps_shader_bytecode),(void*)rx_ps_shader_bytecode);
	rx.imp.sha_pxl_txt = rxGPU_makeShaderFromBytecode(rxGPU_kPIXEL_SHADER_BIT|rxGPU_kDUAL_SOURCE_BLENDING_BIT, "ps_txt", sizeof(rx_ps_txt_sb),(void*)rx_ps_txt_sb);
	rx.imp.sha_pxl_txt_sdf = rxGPU_makeShaderFromBytecode(rxGPU_kPIXEL_SHADER_BIT, "ps_txt_sdf", sizeof(rx_ps_txt_sdf_sb),(void*)rx_ps_txt_sdf_sb);
	rx.imp.sha_vtx_sdf = rxGPU_makeShaderFromBytecode(rxGPU_kVERTEX_SHADER_BIT, "vs_sdf", sizeof(rx_vs_sdf_sb),(void*)rx_vs_sdf_sb);
	rx.imp.sha_pxl_sdf_cir = rxGPU_makeShaderFromBytecode(rxGPU_kPIXEL_SHADER_BIT, "ps_sdf_cir", sizeof(rx_ps_sdf_cir_sb),(void*)rx_ps_sdf_cir_sb);
	rx.imp.sha_pxl_sdf_box = rxGPU_makeShaderFromBytecode(rxGPU_kPIXEL_SHADER_BIT, "ps_sdf_box", sizeof(rx_ps_sdf_box_sb),(void*)rx_ps_sdf_box_sb);

	rx.imp.asm_idx = rxGPU_makeIndexBuffer(sizeof(rxIMP_Index), rxIMP_INDEX_BUFFER_SIZE);
	rx.imp.asm_vtx = rxGPU_makeVertexBuffer(sizeof(rxIMP_Vertex), rxIMP_VERTEX_BUFFER_SIZE);

	rx.imp.white_texture = rxGPU_create_texture(512,512,EMU_FORMAT_R8G8B8A8_UNORM,0,NULL);

	int stride;
	unsigned char *memory = rxGPU_borrow_texture(rx.imp.white_texture,&stride);
	memset(memory,0xff,stride*rx.imp.white_texture->size_y);
	rxGPU_return_texture(rx.imp.white_texture);

	/* TODO: */
	rx.imp.pip.out.count = 1;
	rx.imp.pip.out.color[0] = rxGPU_create_color_target(rx.wnd.out.tar->size_x,rx.wnd.out.tar->size_y,rx.wnd.out.tar->format,_RX_MSAA,0);
	rx.imp.pip.out.depth    = rxGPU_create_depth_target(rx.wnd.out.tar->size_x,rx.wnd.out.tar->size_y,DXGI_FORMAT_D32_FLOAT);

	D3D11_DEPTH_STENCIL_DESC stencil_config_d3d;
	stencil_config_d3d.     DepthEnable=FALSE;
	stencil_config_d3d.   StencilEnable=FALSE;
	stencil_config_d3d.  DepthWriteMask=D3D11_DEPTH_WRITE_MASK_ALL;
	stencil_config_d3d.       DepthFunc=D3D11_COMPARISON_LESS;
	stencil_config_d3d. StencilReadMask=D3D11_DEFAULT_STENCIL_READ_MASK;
	stencil_config_d3d.StencilWriteMask=D3D11_DEFAULT_STENCIL_WRITE_MASK;
	stencil_config_d3d.FrontFace.      StencilFailOp=D3D11_STENCIL_OP_KEEP;
	stencil_config_d3d.FrontFace. StencilDepthFailOp=D3D11_STENCIL_OP_DECR;
	stencil_config_d3d.FrontFace.      StencilPassOp=D3D11_STENCIL_OP_KEEP;
	stencil_config_d3d.FrontFace.        StencilFunc=D3D11_COMPARISON_ALWAYS;
	stencil_config_d3d. BackFace.      StencilFailOp=D3D11_STENCIL_OP_KEEP;
	stencil_config_d3d. BackFace. StencilDepthFailOp=D3D11_STENCIL_OP_DECR;
	stencil_config_d3d. BackFace.      StencilPassOp=D3D11_STENCIL_OP_KEEP;
	stencil_config_d3d. BackFace.        StencilFunc=D3D11_COMPARISON_ALWAYS;
	ID3D11Device_CreateDepthStencilState(rx.d3d11.dev,&stencil_config_d3d,&rx.imp.pip.d3d11.ds);

	D3D11_BLEND_DESC blender_config_d3d;
	ZeroMemory(&blender_config_d3d,sizeof(blender_config_d3d));
	blender_config_d3d.RenderTarget[0].          BlendEnable=TRUE;
	blender_config_d3d.RenderTarget[0].             SrcBlend=D3D11_BLEND_SRC_ALPHA;
	blender_config_d3d.RenderTarget[0].            DestBlend=D3D11_BLEND_INV_SRC_ALPHA;
	blender_config_d3d.RenderTarget[0].              BlendOp=D3D11_BLEND_OP_ADD;
	blender_config_d3d.RenderTarget[0].        SrcBlendAlpha=D3D11_BLEND_ZERO;
	blender_config_d3d.RenderTarget[0].       DestBlendAlpha=D3D11_BLEND_ZERO;
	blender_config_d3d.RenderTarget[0].         BlendOpAlpha=D3D11_BLEND_OP_ADD;
	blender_config_d3d.RenderTarget[0].RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;
	ID3D11Device_CreateBlendState(rx.d3d11.dev
	, &blender_config_d3d,&rx.imp.d3d11.default_blend_state);

	blender_config_d3d.RenderTarget[0].BlendEnable = TRUE;
	blender_config_d3d.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC1_COLOR;
	blender_config_d3d.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC1_COLOR;
	blender_config_d3d.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blender_config_d3d.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blender_config_d3d.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blender_config_d3d.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blender_config_d3d.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	ID3D11Device_CreateBlendState(rx.d3d11.dev
	, &blender_config_d3d,&rx.imp.d3d11.subpixel_dual_blending_blend_state);

	/* create some default samplers */
	D3D11_SAMPLER_DESC SamplerInfo;
	ZeroMemory(&SamplerInfo,sizeof(SamplerInfo));
	SamplerInfo.AddressU=D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerInfo.AddressV=D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerInfo.AddressW=D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerInfo.MaxAnisotropy  = 0;
	SamplerInfo.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SamplerInfo.MinLOD         = 0;
	SamplerInfo.MaxLOD         = D3D11_FLOAT32_MAX;
	SamplerInfo.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	ID3D11Device_CreateSamplerState(rx.d3d11.dev,&SamplerInfo,&rx.linear_sampler.d3d11.state);
	SamplerInfo.Filter=D3D11_FILTER_MIN_MAG_MIP_POINT;
	ID3D11Device_CreateSamplerState(rx.d3d11.dev,&SamplerInfo,&rx.point_sampler.d3d11.state);
	SamplerInfo.Filter=D3D11_FILTER_ANISOTROPIC;
	ID3D11Device_CreateSamplerState(rx.d3d11.dev,&SamplerInfo,&rx.anisotropic_sampler.d3d11.state);


	D3D11_RASTERIZER_DESC2 raster_config_d3d;
	ZeroMemory(&raster_config_d3d,sizeof(raster_config_d3d));
	raster_config_d3d.             FillMode=D3D11_FILL_SOLID;
	raster_config_d3d.             CullMode=D3D11_CULL_NONE;
	raster_config_d3d.FrontCounterClockwise=FALSE;
	raster_config_d3d.            DepthBias=D3D11_DEFAULT_DEPTH_BIAS;
	raster_config_d3d.       DepthBiasClamp=D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
	raster_config_d3d. SlopeScaledDepthBias=D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	raster_config_d3d.      DepthClipEnable= FALSE;
	raster_config_d3d.        ScissorEnable= TRUE;
	raster_config_d3d.    MultisampleEnable=_RX_MSAA >= 2;
	raster_config_d3d.AntialiasedLineEnable=FALSE;
	raster_config_d3d.ConservativeRaster=D3D11_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	ID3D11Device3 *lpDevice3;
	HRESULT error = IUnknown_QueryInterface(rx.d3d11.dev,&IID_ID3D11Device3,(void**)&lpDevice3);
	rx_assert(SUCCEEDED(error));

	ID3D11RasterizerState2 *rasterizer_d3d;
	error = lpDevice3->lpVtbl->CreateRasterizerState2(lpDevice3,&raster_config_d3d,&rasterizer_d3d);
	rx_assert(SUCCEEDED(error));

	error = IUnknown_QueryInterface(rasterizer_d3d,&IID_ID3D11RasterizerState,(void**)&rx.imp.pip.d3d11.rastr_state);
	rx_assert(SUCCEEDED(error));

	ID3D11DeviceContext_RSSetState(rx.d3d11.ctx,rx.imp.pip.d3d11.rastr_state);

	IUnknown_Release(lpDevice3);
	IUnknown_Release(rasterizer_d3d);
}

void
rxIMP_setShaders(rxGPU_Shader vs, rxGPU_Shader ps, int flush) {
	if (rx.imp.pip.d3d11.vs != vs.d3d11.vertex_shader) {
		if (flush) {
			rxIMP_flush();
		}
		rx.imp.pip.d3d11.vs = vs.d3d11.vertex_shader;
		rx.imp.pip.d3d11.in = vs.d3d11.layout;
		rx.imp.pip.changed = TRUE;
		rx_assert(rx.imp.pip.d3d11.in != NULL);
	}
	if (rx.imp.pip.d3d11.ps != ps.d3d11.pixel_shader) {
		if (flush) {
			rxIMP_flush();
		}
		rx.imp.pip.ps_ = ps;
		rx.imp.pip.d3d11.ps = ps.d3d11.pixel_shader;
		rx.imp.pip.changed = TRUE;
		rx.imp.pip.dual_source_blending = (ps.flags & rxGPU_kDUAL_SOURCE_BLENDING_BIT) != 0;
	}
}

void
rxIMP_setRegister(int reg, rxGPU_Handle handle, int flush) {
	if (rx.imp.pip.reg[reg].handle != handle) {
		if (flush) {
			rxIMP_flush();
		}
		rx.imp.pip.reg[reg].handle = handle;
		rx.imp.pip.changed = TRUE;
	}
}

/* todo: this should take the id of the resource */
void
rxIMP_setSampler(int reg, rxGPU_Sampler sampler, int flush) {
	rxIMP_setRegister(reg,(rxGPU_Handle)sampler.d3d11.state,flush);
}

void
rxIMP_setTexture(int reg, rxGPU_Texture *texture, int flush) {
	rxIMP_setRegister(reg,(rxGPU_Handle)texture->d3d11.shader_target,flush);
	rx.imp.pip.var.xysource.x = texture->size_x;
	rx.imp.pip.var.xysource.y = texture->size_y;
}

void
rxIMP_setVarying(int reg, rxGPU_Uniform_Buffer buffer, int flush) {
	rxIMP_setRegister(reg,(rxGPU_Handle)buffer.lpBuffer->d3d11.buffer,flush);
}

void
rxIMP_applyMode(int mode, int flush) {
	rxIMP_setVarying(rxPIPREG_kVS_BLC_0,rx.imp.varying,flush);
	rxIMP_setVarying(rxPIPREG_kPS_BLC_0,rx.imp.varying,flush);
	if (rx.imp.mode != mode) {
		rxIMP_flush();
		switch (mode) {
			case rxIMP_MODE_LCD_TEXT: {
				rxIMP_setShaders(rx.imp.sha_vtx,rx.imp.sha_pxl_txt,flush);
				rxIMP_setSampler(rxPIPREG_kPS_SAM_0,rx.point_sampler,flush);
			} break;
			case rxIMP_MODE_SDF_TEXT: {
				rxIMP_setShaders(rx.imp.sha_vtx,rx.imp.sha_pxl_txt_sdf,flush);
				rxIMP_setSampler(rxPIPREG_kPS_SAM_0,rx.linear_sampler,flush);
			} break;
			case rxIMP_MODE_SDF_RECT: {
				rxIMP_setShaders(rx.imp.sha_vtx_sdf,rx.imp.sha_pxl_sdf_box,flush);
			} break;
			case rxIMP_MODE_SDF_CIRCLE: {
				rxIMP_setShaders(rx.imp.sha_vtx_sdf,rx.imp.sha_pxl_sdf_cir,flush);
			} break;
			case rxIMP_MODE_QUAD: {
				rxIMP_setShaders(rx.imp.sha_vtx,rx.imp.sha_pxl,flush);
				rxIMP_setTexture(rxPIPREG_kPS_TEX_0,rx.imp.white_texture,flush);
				rxIMP_setSampler(rxPIPREG_kPS_SAM_0,rx.point_sampler,flush);
				rx.imp.world_matrix = rxmatrix_identity();
				rx.imp. view_matrix = rxmatrix_identity();
				rx.imp.view_matrix.m[0][0]=  2. / rx.wnd.size_x;
				rx.imp.view_matrix.m[1][1]=  2. / rx.wnd.size_y;
				rx.imp.view_matrix.m[2][2]= .5;
				rx.imp.view_matrix.m[3][0]=- 1.;
				rx.imp.view_matrix.m[3][1]=- 1.;
			} break;
			case rxIMP_MODE_2D: {
				rxIMP_setShaders(rx.imp.sha_vtx,rx.imp.sha_pxl,flush);

				rx.imp.world_matrix = rxmatrix_identity();
				rx.imp. view_matrix = rxmatrix_identity();
				rx.imp.view_matrix.m[0][0]=  2. / rx.wnd.size_x;
				rx.imp.view_matrix.m[1][1]=  2. / rx.wnd.size_y;
				rx.imp.view_matrix.m[2][2]= .5;
				rx.imp.view_matrix.m[3][0]=- 1.;
				rx.imp.view_matrix.m[3][1]=- 1.;
			} break;
			case rxIMP_MODE_3D: {
				// rx_assert(FALSE /* not implemented */);
				rxIMP_setShaders(rx.imp.sha_vtx,rx.imp.sha_pxl,flush);
				rx.imp.world_matrix = rxmatrix_identity();
				rx.imp. view_matrix = rxmatrix_identity();
				rx.imp. view_matrix = rxmatrix_projection((double)(rx.wnd.size_y)/(double)(rx.wnd.size_x),90,0.01,1000);
			} break;
		}
	}
}

void
rxIMP_apply_pipeline() {

	/* #todo */
	ID3D11DeviceContext_OMSetDepthStencilState(rx.d3d11.ctx,rx.imp.pip.d3d11.ds,1);

	if (rx.imp.pip.dual_source_blending != FALSE) {
		ID3D11DeviceContext_OMSetBlendState(rx.d3d11.ctx,rx.imp.d3d11.subpixel_dual_blending_blend_state,0x00,0xFFFFFFFu);
	} else {
		ID3D11DeviceContext_OMSetBlendState(rx.d3d11.ctx,rx.imp.d3d11.default_blend_state,0x00,0xFFFFFFFu);
	}

	ID3D11DeviceContext_VSSetShader(rx.d3d11.ctx,rx.imp.pip.d3d11.vs,0x00,0);
	ID3D11DeviceContext_PSSetShader(rx.d3d11.ctx,rx.imp.pip.d3d11.ps,0x00,0);

	for(int i=0;i<2;i+=1) {
		ID3D11DeviceContext_VSSetConstantBuffers(rx.d3d11.ctx,i,1,&rx.imp.pip.reg[rxPIPREG_kVS_BLC_0+i].d3d11.buffer);
		ID3D11DeviceContext_PSSetConstantBuffers(rx.d3d11.ctx,i,1,&rx.imp.pip.reg[rxPIPREG_kPS_BLC_0+i].d3d11.buffer);
		ID3D11DeviceContext_VSSetShaderResources(rx.d3d11.ctx,i,1,&rx.imp.pip.reg[rxPIPREG_kVS_TEX_0+i].d3d11.resource_view);
		ID3D11DeviceContext_PSSetShaderResources(rx.d3d11.ctx,i,1,&rx.imp.pip.reg[rxPIPREG_kPS_TEX_0+i].d3d11.resource_view);
		ID3D11DeviceContext_VSSetSamplers(rx.d3d11.ctx,i,1,&rx.imp.pip.reg[rxPIPREG_kVS_SAM_0+i].d3d11.sampler);
		ID3D11DeviceContext_PSSetSamplers(rx.d3d11.ctx,i,1,&rx.imp.pip.reg[rxPIPREG_kPS_SAM_0+i].d3d11.sampler);
	}

	ID3D11DeviceContext_IASetInputLayout(rx.d3d11.ctx,rx.imp.pip.d3d11.in);

	ID3D11RenderTargetView *render_targets[2] = { 0 };
	if(rx.imp.pip.out.color[0] != NULL) {
		render_targets[0] = rx.imp.pip.out.color[0]->d3d11.color_target;
	}
	if(rx.imp.pip.out.color[1] != NULL) {
		render_targets[1] = rx.imp.pip.out.color[1]->d3d11.color_target;
	}

	ID3D11DeviceContext_OMSetRenderTargets(rx.d3d11.ctx,rx.imp.pip.out.count,render_targets,rx.imp.pip.out.depth->d3d11.depth_target);

	unsigned int Stride=sizeof(rxIMP_Vertex);
	unsigned int Offset=0;

	ID3D11DeviceContext_IASetVertexBuffers(rx.d3d11.ctx,0,1,&rx.imp.asm_vtx.lpBuffer->d3d11.buffer,&Stride,&Offset);

	ID3D11DeviceContext_IASetIndexBuffer(rx.d3d11.ctx,rx.imp.asm_idx.lpBuffer->d3d11.buffer,DXGI_FORMAT_R32_UINT,0);

	/* get this from the right place */
	// D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
	ID3D11DeviceContext_IASetPrimitiveTopology(rx.d3d11.ctx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void
rxIMP_flush() {

	if(rx.imp.index_tally != 0) {
		/* return resources */
		rxGPU_returnBuffer(rx.imp.asm_vtx.lpBuffer);
		rxGPU_returnBuffer(rx.imp.asm_idx.lpBuffer);
		rx.imp.vertex_array=rxNull;
		rx.imp. index_array=rxNull;

		/* rx won't update the actual environment contents, you need to do that yourself */

		rxmatrix_t matrix = rxmatrix_multiply(rx.imp.world_matrix,rx.imp.view_matrix);
		rx.imp.pip.var.matrix = matrix;
		rx.imp.pip.var.xyscreen.x = (float)(rx.wnd.size_x);
		rx.imp.pip.var.xyscreen.y = (float)(rx.wnd.size_y);
		rx.imp.pip.var.xycursor.x = (float)(rx.wnd.in.mice.xcursor) / rx.wnd.size_x;
		rx.imp.pip.var.xycursor.y = (float)(rx.wnd.in.mice.ycursor) / rx.wnd.size_y;
		rx.imp.pip.var.total_seconds = rx.total_seconds;
		rx.imp.pip.var.delta_seconds = rx.delta_seconds;
		rxGPU_updateUniformBuffer(rx.imp.varying,&rx.imp.pip.var,sizeof(rx.imp.pip.var));

		/* upload pipeline changes if necessary */
		if (rx.imp.pip.changed != FALSE) {
			rxIMP_apply_pipeline();
			rx.imp.pip.changed = FALSE;
		}

		ID3D11DeviceContext_DrawIndexed(rx.d3d11.ctx,rx.imp.index_tally,0,0);
	}

	rx.imp.index_tally  = 0;
	rx.imp.vertex_tally = 0;
}


/* YOU MUST CALL THIS FUNCTION, CAN'T YOU SEE
IT IS FUNDAMENTAL */
rxAPI inline void
Emu_imp_end()
{

}

void
Emu_imp_begin(int index_count, int vertex_count)
{
	if (rx.imp.index_tally + index_count > rxIMP_INDEX_BUFFER_SIZE) {
		rxIMP_flush();
	}

	if (rx.imp.vertex_tally + vertex_count > rxIMP_VERTEX_BUFFER_SIZE) {
		rxIMP_flush();
	}

	if (rx.imp.vertex_array == NULL) {
		rx.imp.vertex_array = rxGPU_borrowVertexBuffer(rx.imp.asm_vtx,rxNull);
		rx.imp.index_array = rxGPU_borrowIndexBuffer(rx.imp.asm_idx,rxNull);
		rx.imp.vertex_tally = 0;
		rx.imp.index_tally = 0;

		rx.imp.index_offset = 0;
	}

	rx_assert(rx.imp.vertex_array != 0);
	rx_assert(rx.imp. index_array != 0);

/* #pending is this something that we want to do? */
	rx.imp.attr.xyzw.x = .0;
	rx.imp.attr.xyzw.y = .0;
	rx.imp.attr.xyzw.z = .5;
	rx.imp.attr.xyzw.w =  1;
	rx.imp.attr.rgba.r = .0;
	rx.imp.attr.rgba.g = .0;
	rx.imp.attr.rgba.b = .0;
	rx.imp.attr.rgba.a = .0;
	rx.imp.attr.  uv.x = .0;
	rx.imp.attr.  uv.y = .0;

	rx.imp.index_offset = rx.imp.vertex_tally;
}


rxAPI inline rxIMP_Vertex
rxvtx_xy(float x, float y)
{
	rx.imp.attr.xyzw.x =  x;
	rx.imp.attr.xyzw.y =  y;
	return rx.imp.attr;
}

rxAPI inline rxIMP_Vertex
rxvtx_xyuv(float x, float y, float u, float v)
{
	rx.imp.attr.xyzw.x =  x;
	rx.imp.attr.xyzw.y =  y;
	rx.imp.attr.xyzw.z = .5;
	rx.imp.attr.xyzw.w =  1;
	rx.imp.attr.  uv.x =  u;
	rx.imp.attr.  uv.y =  v;
	return rx.imp.attr;
}

rxAPI inline rxIMP_Vertex
rxvtx_xyuv_col(float x, float y, float u, float v, rlColor rgba)
{
	rx.imp.attr.xyzw.x =  x;
	rx.imp.attr.xyzw.y =  y;
	rx.imp.attr.xyzw.z = .5;
	rx.imp.attr.xyzw.w =  1;
	rx.imp.attr.  uv.x =  u;
	rx.imp.attr.  uv.y =  v;
	rx.imp.attr.rgba   = rgba;
	return rx.imp.attr;
}

rxAPI inline void
rxaddnidx(int num, ...)
{
	rx_assert(rx.imp.index_tally + num < rxIMP_INDEX_BUFFER_SIZE);

	va_list vli;
	va_start(vli,num);

	for(int i=0;i<num;i+=1)
	{
		rx.imp.index_array[
		rx.imp.index_tally] = rx.imp.index_offset + va_arg(vli,rxIMP_Index);
		rx.imp.index_tally += 1;
	}

	va_end(vli);
}

rxAPI inline void
rxaddnvtx(int num, ...)
{
	rx_assert(rx.imp.vertex_tally + num < rxIMP_VERTEX_BUFFER_SIZE);

	va_list vli;
	va_start(vli,num);

	for(int i=0;i<num;i+=1)
	{
		rx.imp.vertex_array[
		rx.imp.vertex_tally] = va_arg(vli,rxIMP_Vertex);
		rx.imp.vertex_tally += 1;
	}

	va_end(vli);
}


void
Emu_imp_circle_sdf(
rxvec2_t center, rxvec2_t radius, rlColor color, float roundness, float softness )
{
	if (softness == 0) {
		softness = 1.;
	}
	int x0,y0,x1,y1;
	x0 = (int) (center.x - (1. + radius.x + softness));
	y0 = (int) (center.y - (1. + radius.y + softness));
	x1 = (int) (center.x + (1. + radius.x + softness));
	y1 = (int) (center.y + (1. + radius.y + softness));
	// __debugbreak();
	// TODO:
	// this apply mode is silly AF
	rxIMP_applyMode(rxIMP_MODE_2D,FALSE);
	rxIMP_applyMode(rxIMP_MODE_SDF_CIRCLE,TRUE);

	Emu_imp_begin(6,4);

/* set shared attributes */
	rx.imp.attr.rect.xyxy   = rxvec4_xyzw(center.x,center.y,radius.x,radius.y);
	rx.imp.attr.rect.rgba   = color;
	rx.imp.attr.rect.flag.x = roundness;
	rx.imp.attr.rect.flag.w = softness;
	rxaddnidx(6, 0,1,2, 0,2,3);
	rxaddnvtx(4,
	rxvtx_xy(x0,y0),rxvtx_xy(x0,y1),
	rxvtx_xy(x1,y1),rxvtx_xy(x1,y0));

	Emu_imp_end();
}

void
Emu_imp_rect_sdf(
rxvec2_t center, rxvec2_t radius, rlColor color, float roundness, float softness )
{
	int x0,y0,x1,y1;
	x0 = (int) (center.x - (1. + radius.x + softness));
	y0 = (int) (center.y - (1. + radius.y + softness));
	x1 = (int) (center.x + (1. + radius.x + softness));
	y1 = (int) (center.y + (1. + radius.y + softness));

	// TODO:
	// this apply mode is silly AF
	rxIMP_applyMode(rxIMP_MODE_2D,FALSE);
	rxIMP_applyMode(rxIMP_MODE_SDF_RECT,TRUE);

	Emu_imp_begin(6,4); {
		rx.imp.attr.rect.xyxy   = rxvec4_xyzw(center.x,center.y,radius.x,radius.y);
		rx.imp.attr.rect.rgba   = color;
		rx.imp.attr.rect.flag.x = roundness;
		rx.imp.attr.rect.flag.w = softness;
		rxaddnidx(6, 0,1,2, 0,2,3);
		rxaddnvtx(4,
		rxvtx_xy(x0,y0),rxvtx_xy(x0,y1),
		rxvtx_xy(x1,y1),rxvtx_xy(x1,y0));
	}
	Emu_imp_end();
}

void
Emu_imp_rect_uv(
rlColor color, rxGPU_Texture *texture,
rxGPU_Sampler sampler, float x, float y, float w, float h)
{
	rxvec2_t xy0 = (rxvec2_t){x+0,y+0};
	rxvec2_t xy1 = (rxvec2_t){x+w,y+h};

	rxvec2_t uv0 = (rxvec2_t){0,0};
	rxvec2_t uv1 = (rxvec2_t){1,1};

	rxIMP_applyMode(rxIMP_MODE_QUAD,TRUE);

	rxIMP_setTexture(rxPIPREG_kPS_TEX_0,texture,TRUE);
	rxIMP_setSampler(rxPIPREG_kPS_SAM_0,sampler,TRUE);

	Emu_imp_begin(6,4);
	rx.imp.attr.rgba = color;
	rxaddnvtx(4,
	rxvtx_xyuv(xy0.x,xy0.y, uv0.x,uv1.y),
	rxvtx_xyuv(xy0.x,xy1.y, uv0.x,uv0.y),
	rxvtx_xyuv(xy1.x,xy1.y, uv1.x,uv0.y),
	rxvtx_xyuv(xy1.x,xy0.y, uv1.x,uv1.y));
	rxaddnidx(6, 0,1,2, 0,2,3);
	Emu_imp_end();
}

void
Emu_imp_rect(rlColor color, float x, float y, float w, float h)
{
	rxvec2_t xy0 = (rxvec2_t){x+0,y+0};
	rxvec2_t xy1 = (rxvec2_t){x+w,y+h};

	rxvec2_t uv0 = (rxvec2_t){0,0};
	rxvec2_t uv1 = (rxvec2_t){1,1};


	rxIMP_applyMode(rxIMP_MODE_QUAD,TRUE);
	Emu_imp_begin(6,4);
	rx.imp.attr.rgba = color;
	rxaddnvtx(4,
	rxvtx_xyuv(xy0.x,xy0.y, uv0.x,uv1.y),
	rxvtx_xyuv(xy0.x,xy1.y, uv0.x,uv0.y),
	rxvtx_xyuv(xy1.x,xy1.y, uv1.x,uv0.y),
	rxvtx_xyuv(xy1.x,xy0.y, uv1.x,uv1.y));
	rxaddnidx(6, 0,1,2, 0,2,3);
	Emu_imp_end();
}

void
Emu_imp_line(
rlColor color, float thickness, float x0, float y0, float x1, float y1)
{
	float xdist=x1-x0;
	float ydist=y1-y0;
	float length=sqrtf(xdist*xdist + ydist*ydist);
	float xnormal=.5f * thickness * -ydist/length;
	float ynormal=.5f * thickness * +xdist/length;

	rxIMP_applyMode(rxIMP_MODE_QUAD,TRUE);
	Emu_imp_begin(6,4);
	rx.imp.attr.rgba = color;
	rxaddnvtx(4,
	rxvtx_xyuv(x0-xnormal,y0-ynormal,0,1),
	rxvtx_xyuv(x0+xnormal,y0+ynormal,0,0),
	rxvtx_xyuv(x1+xnormal,y1+ynormal,1,0),
	rxvtx_xyuv(x1-xnormal,y1-ynormal,1,1));
	rxaddnidx(6, 0,1,2, 0,2,3);
	Emu_imp_end();
}

/* todo: this has to be re-visited for sub-pixel rendering */
void
Emu_imp_outline(
rlColor color, float x, float y, float w, float h)
{
	Emu_imp_rect(color,x-.5,y+h-.5,w+.5,1.);
	Emu_imp_rect(color,x-.5,y+0-.5,w+.5,1.);
	Emu_imp_rect(color,x+0-.5,y-.5,1.,h+.5);
	Emu_imp_rect(color,x+w-.5,y-.5,1.,h+.5);
}
