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

ccfunc void
Emu_imp_init()
{
	rx.imp.varying = rxuniform_buffer_create(sizeof(rx.imp.var),NULL);

	rx.imp.sha_vtx = rxShaderCreateFromBytecode(EMU_kVERTEX_SHADER_BIT, "default_vs",sizeof(rx_vs_shader_bytecode),(void*)rx_vs_shader_bytecode);
	rx.imp.sha_pxl = rxShaderCreateFromBytecode(EMU_kPIXEL_SHADER_BIT, "default_ps",sizeof(rx_ps_shader_bytecode),(void*)rx_ps_shader_bytecode);
	rx.imp.sha_pxl_txt = rxShaderCreateFromBytecode(EMU_kPIXEL_SHADER_BIT|EMU_kSOURCE_BLENDING_BIT, "ps_txt",sizeof(rx_ps_txt_sb),(void*)rx_ps_txt_sb);
	rx.imp.sha_pxl_txt_sdf = rxShaderCreateFromBytecode(EMU_kPIXEL_SHADER_BIT, "ps_txt_sdf",sizeof(rx_ps_txt_sdf_sb),(void*)rx_ps_txt_sdf_sb);
	rx.imp.sha_vtx_sdf = rxShaderCreateFromBytecode(EMU_kVERTEX_SHADER_BIT, "vs_sdf",sizeof(rx_vs_sdf_sb),(void*)rx_vs_sdf_sb);
	rx.imp.sha_pxl_sdf_cir = rxShaderCreateFromBytecode(EMU_kPIXEL_SHADER_BIT, "ps_sdf_cir",sizeof(rx_ps_sdf_cir_sb),(void*)rx_ps_sdf_cir_sb);
	rx.imp.sha_pxl_sdf_box = rxShaderCreateFromBytecode(EMU_kPIXEL_SHADER_BIT, "ps_sdf_box",sizeof(rx_ps_sdf_box_sb),(void*)rx_ps_sdf_box_sb);

	rx.imp.asm_idx = rxcreate_index_buffer(sizeof(Emu_imp_index_t), EMU_IMP_INDEX_BUFFER_SIZE);
	rx.imp.asm_vtx = rxcreate_vertex_buffer(sizeof(Emu_imp_vertex_t), EMU_IMP_VERTEX_BUFFER_SIZE);

	rx.imp.white_texture = rlGPU_makeTexture(512,512,EMU_FORMAT_R8G8B8A8_UNORM,0,NULL);

	int stride;
	unsigned char *memory = rlGPU_borrowTexture(rx.imp.white_texture,&stride);

	memset(memory,0xff,stride*rx.imp.white_texture->size_y);

	rlGPU_returnTexture(rx.imp.white_texture);
}

ccfunc void
Emu_imp_apply(int mode, int flush)
{
	rxpipset_varying(REG_VS_BLC_0,rx.imp.varying,flush);
	rxpipset_varying(REG_PS_BLC_0,rx.imp.varying,flush);

	if (rx.imp.mode != mode)
	{
		Emu_imp_flush();

		switch (mode)
		{
			case EMU_IMP_MODE_LCD_TEXT: {
				rxpipset_program(rx.imp.sha_vtx,rx.imp.sha_pxl_txt,flush);
				rxpipset_sampler(REG_PS_SAM_0,rx.point_sampler,flush);
			} break;
			case EMU_IMP_MODE_SDF_TEXT: {
				rxpipset_program(rx.imp.sha_vtx,rx.imp.sha_pxl_txt_sdf,flush);
				rxpipset_sampler(REG_PS_SAM_0,rx.linear_sampler,flush);
			} break;
			case EMU_IMP_MODE_SDF_RECT: {
				rxpipset_program(rx.imp.sha_vtx_sdf,rx.imp.sha_pxl_sdf_box,flush);
			} break;
			case EMU_IMP_MODE_SDF_CIRCLE: {
				rxpipset_program(rx.imp.sha_vtx_sdf,rx.imp.sha_pxl_sdf_cir,flush);
			} break;
			case EMU_IMP_MODE_QUAD: {
				rxpipset_program(rx.imp.sha_vtx,rx.imp.sha_pxl,flush);
				rxpipset_texture(REG_PS_TEX_0,rx.imp.white_texture,flush);
				rxpipset_sampler(REG_PS_SAM_0,rx.point_sampler,flush);

				rx.imp.world_matrix = rxmatrix_identity();
				rx.imp. view_matrix = rxmatrix_identity();
				rx.imp.view_matrix.m[0][0]=  2. / rx.wnd.size_x;
				rx.imp.view_matrix.m[1][1]=  2. / rx.wnd.size_y;
				rx.imp.view_matrix.m[2][2]= .5;
				rx.imp.view_matrix.m[3][0]=- 1.;
				rx.imp.view_matrix.m[3][1]=- 1.;
			} break;
			case EMU_IMP_MODE_2D: {
				rxpipset_program(rx.imp.sha_vtx,rx.imp.sha_pxl,flush);

				rx.imp.world_matrix = rxmatrix_identity();
				rx.imp. view_matrix = rxmatrix_identity();
				rx.imp.view_matrix.m[0][0]=  2. / rx.wnd.size_x;
				rx.imp.view_matrix.m[1][1]=  2. / rx.wnd.size_y;
				rx.imp.view_matrix.m[2][2]= .5;
				rx.imp.view_matrix.m[3][0]=- 1.;
				rx.imp.view_matrix.m[3][1]=- 1.;
			} break;
			case EMU_IMP_MODE_3D: {
				ccassert(FALSE /* not implemented */);
				rxpipset_program(rx.imp.sha_vtx,rx.imp.sha_pxl,flush);

				rx.imp.world_matrix = rxmatrix_identity();
				rx.imp. view_matrix = rxmatrix_identity();
				rx.imp. view_matrix = rxmatrix_projection(
				(double)(rx.wnd.size_y)/(double)(rx.wnd.size_x),90,0.01,1000);
			} break;
		}
	}
}

ccfunc void
Emu_imp_flush()
{
	if(rx.imp.index_tally != 0)
	{
/* return immediate mode resources */
		rxreturn(rx.imp.vertex_buffer_writeonly);
		rxreturn(rx.imp. index_buffer_writeonly);
		rx.imp.vertex_array=ccnull;
		rx.imp. index_array=ccnull;

/* emu won't update the actual environment contents, you need to do that yourself */

		rxmatrix_t matrix = rxmatrix_multiply(rx.imp.world_matrix,rx.imp.view_matrix);
		rx.imp.var.matrix=matrix;
		rx.imp.var.xyscreen.x=(float)(rx.wnd.size_x);
		rx.imp.var.xyscreen.y=(float)(rx.wnd.size_y);
		// todo!!:
 		// todo!!:
		rx.imp.var.xycursor.x=(float)(rx.wnd.in.mice.xcursor) / rx.wnd.size_x;
		rx.imp.var.xycursor.y=(float)(rx.wnd.in.mice.ycursor) / rx.wnd.size_y;
		rx.imp.var.total_seconds=rx.total_seconds;
		rx.imp.var.delta_seconds=rx.delta_seconds;
		rxuniform_buffer_update(rx.imp.varying,&rx.imp.var,sizeof(rx.imp.var));

/* upload pipeline changes if necessary */
		pipupl();

		ID3D11DeviceContext_DrawIndexed(rx.d3d11.ctx,rx.imp.index_tally,0,0);
	}

	rx.imp.index_tally = 0;
	rx.imp.vertex_tally = 0;
}


/* YOU MUST CALL THIS FUNCTION, CAN'T YOU SEE
IT IS FUNDAMENTAL */
ccfunc ccinle void
Emu_imp_end()
{

}

ccfunc void
Emu_imp_begin(int index_count, int vertex_count)
{
	if (rx.imp.index_tally + index_count > EMU_IMP_INDEX_BUFFER_SIZE) {
		Emu_imp_flush();
	}

	if (rx.imp.vertex_tally + vertex_count > EMU_IMP_VERTEX_BUFFER_SIZE) {
		Emu_imp_flush();
	}

	if (rx.imp.vertex_array == NULL) {

		rx.imp.vertex_buffer_writeonly = rxborrow_vertex_buffer(rx.imp.asm_vtx);
		rx.imp. index_buffer_writeonly =  rxborrow_index_buffer(rx.imp.asm_idx);
		rx.imp. vertex_array=rx.imp.vertex_buffer_writeonly.memory;
		rx.imp.  index_array=rx.imp. index_buffer_writeonly.memory;
		rx.imp. vertex_tally=0;
		rx.imp.  index_tally=0;

		rx.imp.index_offset =0;
	}

	ccassert(rx.imp.vertex_array != 0);
	ccassert(rx.imp. index_array != 0);

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


ccfunc ccinle Emu_imp_vertex_t
rxvtx_xy(float x, float y)
{
	rx.imp.attr.xyzw.x =  x;
	rx.imp.attr.xyzw.y =  y;
	return rx.imp.attr;
}

ccfunc ccinle Emu_imp_vertex_t
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

ccfunc ccinle Emu_imp_vertex_t
rxvtx_xyuv_col(float x, float y, float u, float v, rxcolor_t rgba)
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

ccfunc ccinle void
rxaddnidx(int num, ...)
{
	ccassert(rx.imp.index_tally + num < EMU_IMP_INDEX_BUFFER_SIZE);

	va_list vli;
	va_start(vli,num);

	for(int i=0;i<num;i+=1)
	{
		rx.imp.index_array[
		rx.imp.index_tally] = rx.imp.index_offset + va_arg(vli,Emu_imp_index_t);
		rx.imp.index_tally += 1;
	}

	va_end(vli);
}

ccfunc ccinle void
rxaddnvtx(int num, ...)
{
	ccassert(rx.imp.vertex_tally + num < EMU_IMP_VERTEX_BUFFER_SIZE);

	va_list vli;
	va_start(vli,num);

	for(int i=0;i<num;i+=1)
	{
		rx.imp.vertex_array[
		rx.imp.vertex_tally] = va_arg(vli,Emu_imp_vertex_t);
		rx.imp.vertex_tally += 1;
	}

	va_end(vli);
}


void
Emu_imp_circle_sdf(
rxvec2_t center, rxvec2_t radius, rxcolor_t color, float roundness, float softness )
{
	int x0,y0,x1,y1;
	x0 = (int) (center.x - (1. + radius.x + softness));
	y0 = (int) (center.y - (1. + radius.y + softness));
	x1 = (int) (center.x + (1. + radius.x + softness));
	y1 = (int) (center.y + (1. + radius.y + softness));

	Emu_imp_apply(EMU_IMP_MODE_SDF_CIRCLE,TRUE);

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
rxvec2_t center, rxvec2_t radius, rxcolor_t color, float roundness, float softness )
{
	int x0,y0,x1,y1;
	x0 = (int) (center.x - (1. + radius.x + softness));
	y0 = (int) (center.y - (1. + radius.y + softness));
	x1 = (int) (center.x + (1. + radius.x + softness));
	y1 = (int) (center.y + (1. + radius.y + softness));

	Emu_imp_apply(EMU_IMP_MODE_SDF_RECT,TRUE);

	Emu_imp_begin(6,4); {
		/* set shared attributes */
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
rxcolor_t color, rlTexture *texture,
rxsampler_t sampler, float x, float y, float w, float h)
{
	rxvec2_t xy0 = (rxvec2_t){x+0,y+0};
	rxvec2_t xy1 = (rxvec2_t){x+w,y+h};

	rxvec2_t uv0 = (rxvec2_t){0,0};
	rxvec2_t uv1 = (rxvec2_t){1,1};

	Emu_imp_apply(EMU_IMP_MODE_QUAD,TRUE);

	rxpipset_texture(REG_PS_TEX_0,texture,TRUE);
	rxpipset_sampler(REG_PS_SAM_0,sampler,TRUE);

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
Emu_imp_rect(rxcolor_t color, float x, float y, float w, float h)
{
	rxvec2_t xy0 = (rxvec2_t){x+0,y+0};
	rxvec2_t xy1 = (rxvec2_t){x+w,y+h};

	rxvec2_t uv0 = (rxvec2_t){0,0};
	rxvec2_t uv1 = (rxvec2_t){1,1};


	Emu_imp_apply(EMU_IMP_MODE_QUAD,TRUE);
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

ccfunc void
Emu_imp_line(
rxcolor_t color, float thickness, float x0, float y0, float x1, float y1)
{
	float xdist=x1-x0;
	float ydist=y1-y0;
	float length=sqrtf(xdist*xdist + ydist*ydist);
	float xnormal=.5f * thickness * -ydist/length;
	float ynormal=.5f * thickness * +xdist/length;

	Emu_imp_apply(EMU_IMP_MODE_QUAD,TRUE);
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
rxcolor_t color, float x, float y, float w, float h)
{
	Emu_imp_rect(color,x-.5,y+h-.5,w+.5,1.);
	Emu_imp_rect(color,x-.5,y+0-.5,w+.5,1.);
	Emu_imp_rect(color,x+0-.5,y-.5,1.,h+.5);
	Emu_imp_rect(color,x+w-.5,y-.5,1.,h+.5);
}
