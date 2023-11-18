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

#ifndef _rxGPU_H
#define _rxGPU_H


typedef ID3D11DeviceChild *rxGPU_Handle;

void
rxGPU_close_handle(rxGPU_Handle handle) {
	if (handle != 0) {
		IUnknown_Release(handle);
	}
}

typedef struct rxGPU_Sampler rxGPU_Sampler;
typedef struct rxGPU_Texture rxGPU_Texture;
typedef struct rxGPU_TEXTURE rxGPU_TEXTURE;
typedef struct rxGPU_Buffer rxGPU_Buffer;
typedef struct rxGPU_Vertex_Buffer rxGPU_Vertex_Buffer;
typedef struct rxGPU_Index_Buffer rxGPU_Index_Buffer;
typedef struct rxGPU_Uniform_Buffer rxGPU_Uniform_Buffer;
typedef struct rxGPU_Structured_Buffer rxGPU_Structured_Buffer;
typedef struct rxGPU_SHADER_COMPILE rxGPU_SHADER_COMPILE;
typedef struct rxGPU_SHADER_SOURCE rxGPU_SHADER_SOURCE;
typedef struct rxGPU_SHADER_LAYOUT rxGPU_SHADER_LAYOUT;
typedef struct rxGPU_SHADER rxGPU_SHADER;
typedef struct rxGPU_Shader rxGPU_Shader;

rxAPI void rxGPU_returnBuffer(rxGPU_Buffer *lpBuffer);
rxAPI void rxGPU_updateUniformBuffer(rxGPU_Uniform_Buffer buffer, void *source, size_t length);
rxAPI void rxGPU_deleteVertexBuffer(rxGPU_Vertex_Buffer buffer);
rxAPI void rxGPU_deleteIndexBuffer(rxGPU_Index_Buffer buffer);

rxAPI rxGPU_TEXTURE rxGPU_make_texture_config(int size_x, int size_y, DXGI_FORMAT format, int stride, void  *memory, int samples, int quality, D3D11_USAGE memtype, int useflag, int memflag);
rxAPI rxError rxGPU_init_texture(rxGPU_Texture *texture, rxGPU_TEXTURE *config);
rxAPI void rxGPU_close_texture(rxGPU_Texture *texture);
rxAPI void rxGPU_delete_texture(rxGPU_Texture *texture);
rxAPI rxGPU_Texture *rxGPU_create_texture_ex(rxGPU_TEXTURE *config);
rxAPI rxGPU_Texture *rxGPU_create_texture(int size_x, int size_y, DXGI_FORMAT format, int stride, void *memory);
rxAPI void *rxGPU_borrow_texture(rxGPU_Texture *texture, int *stride);
rxAPI void rxGPU_return_texture(rxGPU_Texture *texture);
rxAPI void rxGPU_update_texture(rxGPU_Texture *texture, rx_Image image);
rxAPI rxGPU_Texture *rxGPU_create_depth_target(int size_x, int size_y, int format);
rxAPI rxGPU_Texture *rxGPU_create_color_target(int size_x, int size_y, int format, int samples, int quality);

/* [[BEWARE]]:
	May not handle padding very well should you choose to let the API create the input layout for you
	automatically using the reflection API.
	Try to pack things into bigger units if you run into alignment issues, or try
	re-ordering the structure. */
rxAPI void rxGPU_initShader(rxGPU_Shader *shader, rxGPU_SHADER *config);
rxAPI rxGPU_Shader rxGPU_makeShaderFromBytecode(int flags, char const *label, size_t length, void *memory);


#if 0
enum {
	EMU_HOST_READ_BIT         = 1 << 0,
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

typedef struct rxGPU_Buffer {
	struct {
		int   stride;
		void *memory;
	} mapped;
	struct {
		union {
			ID3D11Buffer *buffer;
			ID3D11Resource *resource;
		};
		ID3D11ShaderResourceView *shader_target;
	} d3d11;
} rxGPU_Buffer;

typedef struct rxGPU_Vertex_Buffer {

	rxGPU_Buffer *lpBuffer;
} rxGPU_Vertex_Buffer;

typedef struct rxGPU_Index_Buffer {

	rxGPU_Buffer *lpBuffer;
} rxGPU_Index_Buffer;

typedef struct rxGPU_Uniform_Buffer {

	rxGPU_Buffer *lpBuffer;
} rxGPU_Uniform_Buffer;

typedef struct rxGPU_Structured_Buffer {

	rxGPU_Buffer *buffer;
} rxGPU_Structured_Buffer;

typedef struct rxGPU_Sampler {
	struct {
		ID3D11SamplerState *state;
	} d3d11;
} rxGPU_Sampler;

typedef struct rxGPU_TEXTURE {
	int    format;
	int    size_x;
	int    size_y;
  	/* multisampling, default should be 1,0 */
	int    samples;
	int    quality;

  	/* replace this with a single flag field instead #todo */
	struct {
  		/* GPU allocation flags */
		D3D11_USAGE memtype;
		int useflag;
    	/* CPU access flags */
		int memflag;
	};

	/* memory and stride are optional, if memory provided stride may not be 0 */
	int    stride;
	void  *memory;

 	/* optional in/out */
	struct {
		union {
			ID3D11Resource  *resource;
			ID3D11Texture2D *texture_2d;
		};

		ID3D11ShaderResourceView  *shader_target;
		ID3D11RenderTargetView     *color_target;
		ID3D11DepthStencilView     *depth_target;
	} d3d11;

} rxGPU_TEXTURE;

/* base texture object for all other textures */
typedef struct rxGPU_Texture {
	int format;
	int size_x;
	int size_y;
	int samples;
	int quality;

  /* [[TODO]]: have a mapper function? */
	struct {
		int   stride;
		void *memory;
	} mapped;

	struct {
		union {
			ID3D11Resource  *resource;
			ID3D11Texture2D *texture_2d;
		};
    	/* views are optional  */
		ID3D11ShaderResourceView *shader_target;
		ID3D11RenderTargetView   *color_target;
		ID3D11DepthStencilView   *depth_target;
	} d3d11;
} rxGPU_Texture;

typedef struct rx_Image {
	int size_x, size_y;
	int format;
	int stride;
	void * memory;
} rx_Image;


enum {
	rxGPU_kINVALID = 0 << 0,
	rxGPU_kPIXEL_SHADER_BIT = 1 << 1,
	rxGPU_kVERTEX_SHADER_BIT = 1 << 2,
	rxGPU_kCOMPUTE_SHADER_BIT = 1 << 3,
	rxGPU_kDUAL_SOURCE_BLENDING_BIT = 1 << 4,
};

typedef struct rxGPU_SHADER_COMPILE {
	char const *debug_label;
	char const *model;
	char const *entry;
	struct {
		size_t length;
		void *memory;
	};
} rxGPU_SHADER_COMPILE;

typedef struct rxGPU_SHADER_SOURCE {
	struct {
		size_t length;
		union {
			void *memory;
			char *string;
		};
	} bytecode;
	rxGPU_SHADER_COMPILE compile;
} rxGPU_SHADER_SOURCE;

typedef struct rxGPU_SHADER_LAYOUT {
	D3D11_INPUT_ELEMENT_DESC attr_array[0x20];
	int attr_count;
	struct {
		ID3D11InputLayout *layout;
	} d3d11;
} rxGPU_SHADER_LAYOUT;

typedef struct rxGPU_SHADER {
	int flags;
	char const *label;
	int expected_color_attachments;
	rxGPU_SHADER_SOURCE source;
	rxGPU_SHADER_LAYOUT layout;
	unsigned force_create_layout: 1;
	unsigned donot_create_layout: 1;
} rxGPU_SHADER;

typedef struct rxGPU_Shader {
	char const *label;
	int flags;
	int expected_color_attachments;
	struct {
		union {
			ID3D11DeviceChild *unknown;
			ID3D11VertexShader *vertex_shader;
			ID3D11PixelShader *pixel_shader;
			ID3D11ComputeShader *compute_shader;
		};
		ID3D11InputLayout *layout;
	} d3d11;
} rxGPU_Shader;

#endif