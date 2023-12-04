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

#ifndef lgi_GPU_HEADER
#define lgi_GPU_HEADER


typedef ID3D11DeviceChild *lgi_GPU_Handle;

void
lgi_GPU__closeHandle(lgi_GPU_Handle handle) {
	if (handle != 0) {
		IUnknown_Release(handle);
	}
}

typedef struct lgi_Sampler lgi_Sampler;
typedef struct lgi_Texture lgi_Texture;
typedef struct rxGPU_TEXTURE rxGPU_TEXTURE;
typedef struct lgi_Buffer lgi_Buffer;
typedef struct lgi_Vertex_Buffer lgi_Vertex_Buffer;
typedef struct lgi_Index_Buffer lgi_Index_Buffer;
typedef struct rxGPU_Uniform_Buffer rxGPU_Uniform_Buffer;
typedef struct rxGPU_Structured_Buffer rxGPU_Structured_Buffer;
typedef struct rxGPU_SHADER_COMPILE rxGPU_SHADER_COMPILE;
typedef struct rxGPU_SHADER_SOURCE rxGPU_SHADER_SOURCE;
typedef struct rxGPU_SHADER_LAYOUT rxGPU_SHADER_LAYOUT;
typedef struct lgi_Shader_Config lgi_Shader_Config;
typedef struct lgi_Shader lgi_Shader;

lgi_API void lgi_Buffer__returnMemory(lgi_Buffer *xx);
lgi_API void lgi_Buffer__updateContents(lgi_Buffer *xx, void *source, size_t length);
lgi_API void lgi_GPU__deleteVertexBuffer(lgi_Vertex_Buffer xx);
lgi_API void lgi_GPU__deleteIndexBuffer(lgi_Index_Buffer xx);

lgi_API rxGPU_TEXTURE rxGPU_make_texture_config(int size_x, int size_y, DXGI_FORMAT format, int stride, void  *memory, int samples, int quality, D3D11_USAGE memtype, int useflag, int memflag);
lgi_API lgi_Error rxGPU_init_texture(lgi_Texture *texture, rxGPU_TEXTURE *config);
lgi_API void rxGPU_close_texture(lgi_Texture *texture);
lgi_API void rxGPU_delete_texture(lgi_Texture *texture);
lgi_API lgi_Texture *rxGPU_create_texture_ex(rxGPU_TEXTURE *config);
lgi_API lgi_Texture *lgi_GPU__createTextureSimply(int size_x, int size_y, DXGI_FORMAT format, int stride, void *memory);
lgi_API void *rxGPU_borrow_texture(lgi_Texture *texture, int *stride);
lgi_API void rxGPU_return_texture(lgi_Texture *texture);
lgi_API void lgi_Texture__updateContents(lgi_Texture *texture, lgi_Bitmap image);
lgi_API lgi_Texture *rxGPU_create_depth_target(int size_x, int size_y, int format);
lgi_API lgi_Texture *rxGPU_create_color_target(int size_x, int size_y, int format, int samples, int quality);

/* [[BEWARE]]:
	May not handle padding very well should you choose to let the API create the input layout for you
	automatically using the reflection API.
	Try to pack things into bigger units if you run into alignment issues, or try
	re-ordering the structure. */
lgi_API void lgi_GPU__createShader(lgi_Shader *shader, lgi_Shader_Config *config);
lgi_API lgi_Shader lgi_compileShader(int flags, char const *label, size_t length, void *memory);


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

typedef struct lgi_Buffer {
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
} lgi_Buffer;

// TODO: there's no need for this!
typedef struct lgi_Vertex_Buffer {

	lgi_Buffer *lpBuffer;
} lgi_Vertex_Buffer;

// TODO: there's no need for this!
typedef struct lgi_Index_Buffer {

	lgi_Buffer *lpBuffer;
} lgi_Index_Buffer;

// TODO: there's no need for this!
typedef struct rxGPU_Uniform_Buffer {

	lgi_Buffer *lpBuffer;
} rxGPU_Uniform_Buffer;

// TODO: there's no need for this!
typedef struct rxGPU_Structured_Buffer {

	lgi_Buffer *buffer;
} rxGPU_Structured_Buffer;

// TODO: we can associate this with a texture and forget about it!
typedef struct lgi_Sampler {
	struct {
		ID3D11SamplerState *state;
	} d3d11;
} lgi_Sampler;

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
typedef struct lgi_Texture {
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

	char const *debug_label;
} lgi_Texture;

// TODO: can we get rid of this!
typedef struct lgi_Bitmap {
	int size_x, size_y;
	int format;
	int stride;
	void * memory;
} lgi_Bitmap;


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

typedef struct lgi_Shader_Config {
	int flags;
	char const *label;
	int expected_color_attachments;
	rxGPU_SHADER_SOURCE source;
	rxGPU_SHADER_LAYOUT layout;
	unsigned force_create_layout: 1;
	unsigned donot_create_layout: 1;
} lgi_Shader_Config;

typedef struct lgi_Shader {
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
} lgi_Shader;

#endif