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

#ifndef _rxIMP_H
#define _rxIMP_H

#ifndef rxIMP_INDEX_BUFFER_SIZE
#define rxIMP_INDEX_BUFFER_SIZE  0x1000
# endif//rxIMP_INDEX_BUFFER_SIZE
#ifndef rxIMP_VERTEX_BUFFER_SIZE
#define rxIMP_VERTEX_BUFFER_SIZE 0x1000
# endif//rxIMP_VERTEX_BUFFER_SIZE
#ifndef rxIMP_ENABLE_DEPTH_TESTING
#define rxIMP_ENABLE_DEPTH_TESTING
# endif//rxIMP_ENABLE_DEPTH_TESTING

rxAPI void rxIMP_setShaders(rxGPU_Shader vs, rxGPU_Shader ps, int flush);
rxAPI void rxIMP_setRegister(int reg, void *res, int flush);
rxAPI void rxIMP_setSampler(int reg, rxGPU_Sampler sampler, int flush);
rxAPI void rxIMP_setTexture(int reg, rxGPU_Texture *texture, int flush);
rxAPI void rxIMP_setVarying(int reg, rxGPU_Uniform_Buffer buffer, int flush);
rxAPI void rxIMP_flush();
rxAPI void rxIMP_applyMode();

enum {
	rxIMP_MODE_2D            = 0,
	rxIMP_MODE_3D,
	rxIMP_MODE_QUAD,
	rxIMP_MODE_SDF_RECT,
	rxIMP_MODE_SDF_CIRCLE,
	rxIMP_MODE_SDF_TEXT,
	rxIMP_MODE_LCD_TEXT
};

typedef int rxIMP_Index;

typedef union {
	struct {
		rxvec4_t  xyzw;
		rxvec4_t  rgba;
		rxvec2_t  uv;
	};
	struct {
		float x,y,z,w;
		float r,g,b,a;
		float u,v;
	};
	/* for specialized shaders */
	struct {
		rxvec2_t xy;
		rxvec4_t xyxy;
		rxvec4_t rgba;
		rxvec4_t flag;
	} rect;
} rxIMP_Vertex;

typedef struct {
	unsigned       enable: 1;
	unsigned int   write_mask;
	struct {
		D3D11_BLEND    src;
		D3D11_BLEND    dst;
		D3D11_BLEND_OP op;
	} color;
	struct {
		D3D11_BLEND    src;
		D3D11_BLEND    dst;
		D3D11_BLEND_OP op;
	} alpha;
} EMU_color_blend_config_t;

typedef struct {
	EMU_color_blend_config_t blend_config;
	int format;
	struct {
		unsigned enabled: 1;
		int samples;
		int quality;
	} multisampling;
} EMU_color_config_t;

enum {
	rxPIPREG_kPS_TEX_0, rxPIPREG_kPS_TEX_1,
	rxPIPREG_kPS_SAM_0, rxPIPREG_kPS_SAM_1,
	rxPIPREG_kPS_BLC_0, rxPIPREG_kPS_BLC_1,

	rxPIPREG_kVS_TEX_0, rxPIPREG_kVS_TEX_1,
	rxPIPREG_kVS_SAM_0, rxPIPREG_kVS_SAM_1,
	rxPIPREG_kVS_BLC_0, rxPIPREG_kVS_BLC_1,
};

typedef union {
	rxGPU_Handle handle;
	union {
		ID3D11ShaderResourceView   *resource_view;
		ID3D11Buffer               *buffer;
		ID3D11SamplerState         *sampler;
	} d3d11;
} rxIMP_Register;

typedef struct {
	struct {
		rxmatrix_t matrix;
		rxvec2_t   xyscreen;
		/* this should be an array */
		rxvec2_t   xysource;
		rxvec2_t   xycursor;
		double     total_seconds;
		double     delta_seconds;
	} var;
	struct {
		ID3D11InputLayout *in;
		ID3D11DepthStencilState *ds;
		ID3D11VertexShader *vs;
		ID3D11PixelShader *ps;
		ID3D11RasterizerState *rastr_state;
		ID3D11BlendState *blend_state;
	} d3d11;
	rxGPU_Shader ps_;
	unsigned dual_source_blending: 1;
	struct {
		rxGPU_Texture *color[2];
		int count;
		rxGPU_Texture *depth;
	} out;
	rxIMP_Register reg[0x20];
	int changed;
} rxIMP_Pipeline;

typedef struct {

	int mode;

	rxmatrix_t view_matrix;
	rxmatrix_t world_matrix;



	rxGPU_Uniform_Buffer  varying;

	struct {
		ID3D11BlendState *default_blend_state;
		ID3D11BlendState *subpixel_dual_blending_blend_state;
	} d3d11;

	struct {
		rxGPU_Shader sha_vtx_sdf;
		rxGPU_Shader sha_pxl_sdf_cir;
		rxGPU_Shader sha_pxl_sdf_box;
		rxGPU_Shader sha_pxl_txt;
		rxGPU_Shader sha_pxl_txt_sdf;
		rxGPU_Shader sha_vtx;
		rxGPU_Shader sha_pxl;
	};

	rxGPU_Vertex_Buffer asm_vtx;
	rxGPU_Index_Buffer asm_idx;

	rxIMP_Vertex *vertex_array;
	int vertex_tally;

	rxIMP_Index *index_array;
	int index_tally;

	int index_offset;
	rxIMP_Vertex attr;
	rxGPU_Texture *white_texture;

	rxIMP_Pipeline pip;
} rxIMP_Context;

#endif