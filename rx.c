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
**                 github.com/MicroRJ/rx
**
*/

/*
** Libraries that you should use instead:
** sokol or tigr or raylib or virtually any other library that isn't this one.
**
** ** panic notes **
** - I DON'T SEE ANYTHING:
**   - Ensure that the pixel shader is not the problem first, make it return a solid color, like white,
**     if it does show, ensure the alpha channel is set.
**   - Ensure the render stack is configured properly, see note below to ensure how.
**   - Ensure your constant, vertex and index buffers contain valid data, you can check all this quite
**     trivially in the amazing 'Render Doc' for free!
**     - Select the 'Event Browser' window, (You can toggle windows under the 'Window' menu)
**         - First look at your draw calls, ensure you have a 'ClearRenderTarget()', 'Present()' and
**           at least a draw instruction, such as 'DrawIndexed(6)', if you don't it means you forgot
**           missed some code.
**         - For further inspection, select a draw event, such as 'DrawIndexed(..)'
**           - First, go to the 'Pipeline Stage' window, click the vertex and then the pixel shader stages to ensure
**             that all required resources are properly bound in their corresponding slots.
**              - You can click on the texture or resource to view its contents.
**           - Then, to verify that the per vertex data is proper, click on the 'Mesh Viewer' window,
**             you can see the projected inputs and outputs of the vertex shader, use this window to ensure
**             everything is properly set.
**           - If applicable, ensure the 'w' component of your vectors is properly set, if not sure, edit
**              the vertex shader and manually set to .5.
**   - If you're sure the problem is not the data nor the shaders, then it will most likely be a pipeline configuration
**     mistake, check the blend state first, then the rasterizer state.
**      - Ensure there's at least one blend state properly configured, if you're not sure disable blending
**        altogether to use d3d's default blend state.
**      - If clipping enabled, ensure you've got a clip set big enough for your scene, if not sure, disable
**        clipping altogether by not setting the rasterizer.
**      - Ensure your view-port is properly set!
**   - If you still don't see anything, try changing the clear color to something different, if you don't
**     see that color, then it could be a driver error, in which case, run Visual Studio's Graphic Debugger.
*/
// todo: bind different textures to the shader to avoid having to switch them multiple times per frame?
#ifndef _RX_H
#define _RX_H

#pragma comment(lib,        "Gdi32")
#pragma comment(lib,       "dxguid")
#pragma comment(lib,        "d3d11")
#pragma comment(lib,  "d3dcompiler")

#pragma warning(push)
/* these are some warnings generated in d3dcompiler.h */
#pragma warning(disable:4115)
#pragma warning(disable:4201)

# ifndef _RX_NO_WINDOWS
# define             NOMINMAX
# define  WIN32_LEAN_AND_MEAN
# define _NO_CRT_STDIO_INLINE
#include     <windows.h>
#include    <Windowsx.h>
#  endif

# define  CINTERFACE
# define  COBJMACROS
# define  D3D11_NO_HELPERS
#include <d3dcompiler.h>
#include   <dxgidebug.h>
#include        <dxgi.h>
#include       <d3d11.h>
#include     <dxgi1_3.h>

#pragma warning(pop)

#ifndef EMU_MALLOC
#define EMU_MALLOC(size,user) ((void)(user),ccmalloc(size))
# endif
#ifndef EMU_REALLOC
#define EMU_REALLOC(size,memory,user) ((void)(user),ccrealloc(size,memory))
# endif
#ifndef EMU_FREE
#define EMU_FREE(memory,user) ((void)(user),ccfree(memory))
# endif

#   ifdef _RX_STANDALONE
#	include <cc/cc.c>
#	define STB_IMAGE_IMPLEMENTATION
#	define STBI_MALLOC(size)          EMU_MALLOC(size,NULL)
#	define STBI_REALLOC(size,memory)  EMU_REALLOC(size,memory,NULL)
#	define STBI_FREE(memory)          EMU_FREE(memory,NULL)
#	include "stb_image.h"
#  define STB_IMAGE_WRITE_IMPLEMENTATION
#  define STBIW_MALLOC(size)         EMU_MALLOC(size,NULL)
#  define STBIW_REALLOC(size,memory) EMU_REALLOC(size,memory,NULL)
#  define STBIW_FREE(memory)         EMU_FREE(memory,NULL)
#	include "stb_image_write.h"
#	pragma warning(push)
#		pragma warning(disable:4100)
#		define STB_TRUETYPE_IMPLEMENTATION
#		define STBTT_malloc(size,u) EMU_MALLOC(size,NULL)
#		define STBTT_free(memory,u) EMU_FREE(memory,NULL)
#		include "stb_truetype.h"
#	pragma warning(pop)
#endif//_RX_STANDALONE

/* todo: this is to be embedded eventually */
#include  "rxps.hlsl"
#include  "rxvs.hlsl"
#include  "rxsdf.vs.hlsl"
#include  "rxtxt.ps.hlsl"
#include  "rxtxt_sdf.ps.hlsl"
#include  "rxsdf_cir.ps.hlsl"
#include  "rxsdf_box.ps.hlsl"


/* disabled warnings */
#pragma warning(push)
/* unreferenced stuff */
#pragma warning(disable:4100)
/* nameless structs and unions */
#pragma warning(disable:4201)

/* int to float float to int and truncation warnings */
#pragma warning(disable:4244)
#pragma warning(disable:4305)


/* delicacies of programming */
#ifndef RX_TLIT
#ifndef __cplusplus
#define RX_TLIT(T) (T)
#  else
#define RX_TLIT(T)
# endif//__cplusplus
# endif//RX_TLIT

#ifndef _RX_DEFAULT_WINDOW_SIZE_X
#define _RX_DEFAULT_WINDOW_SIZE_X CW_USEDEFAULT
# endif//_RX_DEFAULT_WINDOW_SIZE_X
#ifndef _RX_DEFAULT_WINDOW_SIZE_Y
#define _RX_DEFAULT_WINDOW_SIZE_Y CW_USEDEFAULT
# endif//_RX_DEFAULT_WINDOW_SIZE_Y
#ifndef _RX_MSAA
#define _RX_MSAA 1
# endif//_RX_MSAA
#ifndef _RX_REFRESH_RATE
#define _RX_REFRESH_RATE 60
# endif//_RX_REFRESH_RATE

#ifndef RX_SHADER_COMPILATION_FLAGS
# ifdef RX_DEBUGGABLE_SHADERS
#define RX_SHADER_COMPILATION_FLAGS\
/* */D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR|\
/* */D3DCOMPILE_DEBUG|\
/* */D3DCOMPILE_SKIP_OPTIMIZATION|\
/* */D3DCOMPILE_WARNINGS_ARE_ERRORS
#else
#define RX_SHADER_COMPILATION_FLAGS\
/* */D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR|\
/* */D3DCOMPILE_ENABLE_STRICTNESS|\
/* */D3DCOMPILE_OPTIMIZATION_LEVEL3
# endif//RX_DEBUGGABLE_SHADERS
# endif//RX_SHADER_COMPILATION_FLAGS

#include "rxm.cc"

typedef struct rxcolor8_t rxcolor8_t;
typedef struct rxcolor8_t
{ unsigned char r,g,b,a;
} rxcolor8_t;

typedef rxvec4_t rxcolor_t;

#ifndef RX_RGBA
#define RX_RGBA(R,G,B,A) RX_TLIT(rxcolor_t){R,G,B,A}
# endif
#ifndef RX_RGBA_UNORM
#define RX_RGBA_UNORM(R,G,B,A) RX_RGBA((R)/255.f,(G)/255.f,(B)/255.f,(A)/255.f)
# endif

/*
**
**  NOLI SE TANGERE
**
*/

/* This could be made into an enum instead - XXX - the one called rj */
#define RX_COLOR_WHITE         RX_RGBA_UNORM(0xFF, 0xFF, 0xFF, 0xFF)
#define RX_COLOR_BLACK         RX_RGBA_UNORM(0x00, 0x00, 0x00, 0xFF)
#define RX_COLOR_RED           RX_RGBA_UNORM(0xFF, 0x00, 0x00, 0xFF)
#define RX_COLOR_GREEN         RX_RGBA_UNORM(0x00, 0xFF, 0x00, 0xFF)
#define RX_COLOR_BLUE          RX_RGBA_UNORM(0x00, 0x00, 0xFF, 0xFF)
#define RX_COLOR_YELLOW        RX_RGBA_UNORM(0xFF, 0xFF, 0x00, 0xFF)
#define RX_COLOR_CYAN          RX_RGBA_UNORM(0x00, 0xFF, 0xFF, 0xFF)
#define RX_COLOR_MAGENTA       RX_RGBA_UNORM(0xFF, 0x00, 0xFF, 0xFF)
#define RX_COLOR_ORANGE        RX_RGBA_UNORM(0xFF, 0xA5, 0x00, 0xFF)
#define RX_COLOR_PURPLE        RX_RGBA_UNORM(0x80, 0x00, 0x80, 0xFF)
#define RX_COLOR_PINK          RX_RGBA_UNORM(0xFF, 0xC0, 0xCB, 0xFF)
#define RX_COLOR_LIME          RX_RGBA_UNORM(0x00, 0xFF, 0x00, 0xFF)
#define RX_COLOR_TEAL          RX_RGBA_UNORM(0x00, 0x80, 0x80, 0xFF)
#define RX_COLOR_SKY_BLUE      RX_RGBA_UNORM(0x87, 0xCE, 0xEB, 0xFF)
#define RX_COLOR_GOLD          RX_RGBA_UNORM(0xFF, 0xD7, 0x00, 0xFF)
#define RX_COLOR_INDIGO        RX_RGBA_UNORM(0x4B, 0x00, 0x82, 0xFF)
#define RX_COLOR_SILVER        RX_RGBA_UNORM(0xC0, 0xC0, 0xC0, 0xFF)
#define RX_COLOR_TURQUOISE     RX_RGBA_UNORM(0x40, 0xE0, 0xD0, 0xFF)
#define RX_COLOR_CORAL         RX_RGBA_UNORM(0xFF, 0x7F, 0x50, 0xFF)
#define RX_COLOR_ORCHID        RX_RGBA_UNORM(0xDA, 0x70, 0xD6, 0xFF)
#define RX_COLOR_LAVENDER      RX_RGBA_UNORM(0xE6, 0xE6, 0xFA, 0xFF)
#define RX_COLOR_MAROON        RX_RGBA_UNORM(0x80, 0x00, 0x00, 0xFF)
#define RX_COLOR_NAVY          RX_RGBA_UNORM(0x00, 0x00, 0x80, 0xFF)
#define RX_COLOR_OLIVE         RX_RGBA_UNORM(0x80, 0x80, 0x00, 0xFF)
#define RX_COLOR_SALMON        RX_RGBA_UNORM(0xFA, 0x80, 0x72, 0xFF)
#define RX_COLOR_AQUAMARINE    RX_RGBA_UNORM(0x7F, 0xFF, 0xD4, 0xFF)

typedef enum {
	EMU_ERROR_NONE = 0,
	EMU_ERROR_CREATE_TEXTURE,
	EMU_ERROR
} emu_error;

enum {
	EMU_FORMAT_R8_UNORM 		  = DXGI_FORMAT_R8_UNORM,
	EMU_FORMAT_R8G8B8A8_UNORM = DXGI_FORMAT_R8G8B8A8_UNORM
};

enum {
	rx_kNONE   = 0,

	rx_kKEY_F1,
	rx_kKEY_F2,
	rx_kKEY_F3,
	rx_kKEY_F4,
	rx_kKEY_F5,
	rx_kKEY_F6,
	rx_kKEY_F7,
	rx_kKEY_F8,
	rx_kKEY_F9,
	rx_kKEY_F10,
	rx_kKEY_F11,
	rx_kKEY_F12,

	rx_kRETURN,

	rx_kKEY_LEFT,
	rx_kKEY_RIGHT,
	rx_kKEY_UP,
	rx_kKEY_DOWN,

	rx_kLCTRL,
	rx_kRCTRL,
	rx_kLSFHT,
	rx_kRSFHT,
	rx_kBCKSPC,
	rx_kDELETE,
	rx_kHOME,
	rx_kEND,
	rx_kESCAPE,

	rx_kKEY_SPACE = ' ',
	rx_kKEY_A     = 'A',
	rx_kKEY_Z     = 'Z',
	rx_kKEY_0     = '0',
	rx_kKEY_9     = '9',
};

// note: is this is good name?
typedef struct rxborrowed_t rxborrowed_t;
typedef struct rxborrowed_t
{
	int length;
	int stride;

	union
	{ unsigned char *cursor;
		void *memory;
	};

/* this will be removed possibly if we ever switch to a handle based system */
	struct
	{
		ID3D11Resource * resource;
	} d3d11;
} rxborrowed_t;

typedef ID3D11DeviceChild *rxunknown_t;

/* these are stupid */
ccfunc ccinle void
rxunknown_delete(void *unknown);

ccfunc rxborrowed_t
rxunknown_borrow(rxunknown_t buffer);

/* section: GPU buffers */
typedef struct rxuniform_buffer_t rxuniform_buffer_t;
typedef struct rxuniform_buffer_t
{
	rxunknown_t unknown;
} rxuniform_buffer_t;

void
rxuniform_buffer_delete(
rxuniform_buffer_t buffer );

void
rxuniform_buffer_update(
rxuniform_buffer_t uniform, void *memory, size_t length );

typedef struct rxstruct_buffer_t rxstruct_buffer_t;
typedef struct rxstruct_buffer_t
{
	rxunknown_t unknown;
} rxstruct_buffer_t;

void
rxstruct_buffer_delete(
rxstruct_buffer_t);

typedef struct rxindex_buffer_t rxindex_buffer_t;
typedef struct rxindex_buffer_t
{
	rxunknown_t unknown;
} rxindex_buffer_t;

void
rxindex_buffer_delete(
rxindex_buffer_t);

typedef struct rxvertex_buffer_t rxvertex_buffer_t;
typedef struct rxvertex_buffer_t
{
	rxunknown_t unknown;
} rxvertex_buffer_t;

void
rxvertex_buffer_delete(
rxvertex_buffer_t);


/* section: GPU sampler */
typedef struct rxsampler_t
{
	struct
	{

		ID3D11SamplerState *state;
	} d3d11;
} rxsampler_t;

ccfunc void
rxsampler_apply(
int slot,
rxsampler_t sampler);

/* section: textures */
#include "emu_texture.h"
#include "emu_shader.h"
#include "rx.program.c"
#include "emu_imp.h"

/* this struct is typedef'd should you want to avoid allocating it globally,
for instance say you had your own global state and within that state you'd
like to have the rx object, in such case simply use the appropriate macro to
prevent this file from adding rx to the global scope. */
typedef struct rx_t rx_t;
typedef struct rx_t {
/* timing stuff */
	int   tick_count;
	ccclocktick_t   start_ticks;
	ccclocktick_t   frame_ticks;
	ccclocktick_t   total_ticks;
	ccclocktick_t   delta_ticks;
/* todo!: there are more correct and robust ways to store time long term, @TomForsyth */
	double          total_seconds;
	double          delta_seconds;

/* todo: constants, should be upper case */
	rxsampler_t          linear_sampler;
	rxsampler_t          point_sampler;
	rxsampler_t          anisotropic_sampler;

/* main stuff */
	struct {
		struct {
			ID3D11InfoQueue        *inf;
			ID3D11Device           *dev;
			ID3D11DeviceContext    *ctx;
		};
	} d3d11;

	Emu_imp_t imp;

/* current pipeline state, not recommended to modify directly,
use the appropriate functions instead which will flush if necessary and if specified
to do so */
	pipenv_t pip;

/* whether the current pipeline should be uploaded */
	int      upl;

/* window related structure, we only support one window for now but it'd be
trivial to extend this */
	struct
	{
		unsigned  off: 1;
		unsigned  vis: 1;

/* window dimensions */
		struct {
			int size_x;
			int size_y;
			int center_x;
			int center_y;
		};

/* native window objects */
		struct {
			HWND obj;
		} win32;

/* output media */
		struct {
			Emu_texture_t *tar;

			struct {
				IDXGISwapChain2 *swap_chain;
				void            *frame_await;
			} d3d11;
		} out;

/* input handling members, these get updated once every tick */
		struct {
			struct {
/* todo: store this better - rj */
				short    chr;

/* todo: store this better */
				char     key_lst[0x100];
				char     key    [0x100];

				unsigned is_ctrl: 1;
				unsigned is_menu: 1;
				unsigned is_shft: 1;
			} kbrd;
			struct {
				int xcursor;
				int ycursor;
				int yscroll;
				int xscroll;

				int  xclick;
				int  yclick;

				int btn_old;
				int btn_cur;
			} mice;
		} in;
	} wnd;

/* basic platform specific stuff */
	struct {
		HMODULE shcore_dll;
		HMODULE user32_dll;

		struct {
			HCURSOR arrow;
		} cursor;
	} win32;

} rx_t;

/* the source of all evil is here */
ccglobal rx_t rx;

/* section: basic system functions */

void
Emu_system_set_cursor(/*todo*/HCURSOR cursor)
{
	SetCursor(cursor);
}

#include "rx.program.cc"


/* section: windowing functions */
int
rxwndmsg_handler_win32(UINT,WPARAM,LPARAM);

LRESULT CALLBACK
rxwndmsg_callback_win32(HWND,UINT,WPARAM,LPARAM);

ccfunc void
Emu_window_poll();


/* todo: rename */
#ifndef       WAS_DOWN
#define       WAS_DOWN(x) ((rx.wnd.in.mice.btn_old & (1 << x)) != 0)
# endif
#ifndef        IS_DOWN
#define        IS_DOWN(x) ((rx.wnd.in.mice.btn_cur & (1 << x)) != 0)
# endif
#ifndef IS_CLICK_LEAVE
#define IS_CLICK_LEAVE(x) !IS_DOWN(x) &&  WAS_DOWN(x)
# endif
#ifndef IS_CLICK_ENTER
#define IS_CLICK_ENTER(x)  IS_DOWN(x) && !WAS_DOWN(x)
# endif

ccfunc ccinle int rlIsCtrlKey()
{ return rx.wnd.in.kbrd.is_ctrl; }

ccfunc ccinle int rxismenu()
{ return rx.wnd.in.kbrd.is_menu; }

ccfunc ccinle int rxisshft()
{ return rx.wnd.in.kbrd.is_shft; }

ccfunc ccinle int rxtstbtn(int x)
{ return IS_DOWN(x); }

int
rxtstkey(int x)
{
	return rx.wnd.in.kbrd.key[x] != 0;
}

int rxchr()
{
	return rx.wnd.in.kbrd.chr;
}

void rxunknown_delete(void *unknown)
{
	if(unknown != 0)
	{
		IUnknown_Release((rxunknown_t)(unknown));
	}
}

void rxvertex_buffer_delete(rxvertex_buffer_t buffer)
{
	rxunknown_delete(buffer.unknown);
}

void rxindex_buffer_delete(rxindex_buffer_t buffer)
{
	rxunknown_delete(buffer.unknown);
}

/* section: pipeline config */
void
rxpipset_program(rxShader vs, rxShader ps, int flush)
{
	if( rx.pip.d3d11.vs != vs.d3d11.vertex_shader )
	{
		if(flush) {
			Emu_imp_flush();
		}

		rx.pip.d3d11.vs = vs.d3d11.vertex_shader;
		rx.pip.d3d11.in = vs.d3d11.layout;
		rx.upl = TRUE;

		ccassert(rx.pip.d3d11.in != NULL);
	}

	if( rx.pip.d3d11.ps != ps.d3d11.pixel_shader )
	{
		if(flush) {
			Emu_imp_flush();
		}

		rx.pip.ps_ = ps;

		rx.pip.d3d11.ps = ps.d3d11.pixel_shader;
		rx.upl = TRUE;

		rx.pip.dual_source_blending =
		(ps.flags & EMU_kSOURCE_BLENDING_BIT) != 0;
	}
}

void
regset(
int reg, void *val, int flush)
{
	if(rx.pip.reg[reg].val != val)
	{
		if(flush) {
			Emu_imp_flush();
		}

		rx.pip.reg[reg].val  = val;
		rx.upl = TRUE;
	}
}

/* todo: this should take the id of the resource */
void
rxpipset_sampler(
int reg, rxsampler_t sampler, int flush)
{
	regset(reg,sampler.d3d11.state,flush);
}

void
rxpipset_texture(
int reg, Emu_texture_t *texture, int flush)
{
	regset(reg,texture->d3d11.shader_target,flush);

/* we should have a vtable type thing here? this way we can set these things?
or in the flush function instead? idk #todo */
	rx.imp.var.xysource.x = texture->size_x;
	rx.imp.var.xysource.y = texture->size_y;
}


/* todo: this should take the id of the resource */
void
rxpipset_varying(
int reg, rxuniform_buffer_t buffer, int flush)
{
	regset(reg,buffer.unknown,flush);
}

void
pipupl()
{
	if(rx.upl != TRUE)
	{
		return;
	}

	rx.upl = FALSE;

/* #todo */
	ID3D11DeviceContext_OMSetDepthStencilState(rx.d3d11.ctx,rx.pip.d3d11.ds,1);

	if(rx.pip.dual_source_blending != FALSE) {
		ID3D11DeviceContext_OMSetBlendState(rx.d3d11.ctx,
		rx.imp.d3d11.subpixel_dual_blending_blend_state,0x00,0xFFFFFFFu);
	} else {
		ID3D11DeviceContext_OMSetBlendState(rx.d3d11.ctx,
		rx.imp.d3d11.default_blend_state,0x00,0xFFFFFFFu);
	}


	ID3D11DeviceContext_VSSetShader(rx.d3d11.ctx,rx.pip.d3d11.vs,0x00,0);
	ID3D11DeviceContext_PSSetShader(rx.d3d11.ctx,rx.pip.d3d11.ps,0x00,0);
	ID3D11DeviceContext_CSSetShader(rx.d3d11.ctx,rx.pip.d3d11.cs,0x00,0);

	for(int i=0;i<2;i+=1)
	{ ID3D11DeviceContext_VSSetConstantBuffers(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_VS_BLC_0+i].d3d11.buffer);
		ID3D11DeviceContext_PSSetConstantBuffers(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_PS_BLC_0+i].d3d11.buffer);

		ID3D11DeviceContext_VSSetShaderResources(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_VS_TEX_0+i].d3d11.resource_view);
		ID3D11DeviceContext_PSSetShaderResources(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_PS_TEX_0+i].d3d11.resource_view);

		ID3D11DeviceContext_VSSetSamplers(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_VS_SAM_0+i].d3d11.sampler);
		ID3D11DeviceContext_PSSetSamplers(rx.d3d11.ctx,i,1,&rx.pip.reg[REG_PS_SAM_0+i].d3d11.sampler);
	}

/* pending: this a good placement? */
	ID3D11DeviceContext_IASetInputLayout(rx.d3d11.ctx,rx.pip.d3d11.in);

/* todo */
	ID3D11RenderTargetView *render_targets[2] = { 0 };

	if(rx.pip.out.color[0] != NULL) {
		render_targets[0] = rx.pip.out.color[0]->d3d11.color_target;
	}
	if(rx.pip.out.color[1] != NULL) {
		render_targets[1] = rx.pip.out.color[1]->d3d11.color_target;
	}

	ID3D11DeviceContext_OMSetRenderTargets(rx.d3d11.ctx,
	rx.pip.out.count,render_targets,rx.pip.out.depth->d3d11.depth_target);

	unsigned int Stride=sizeof(Emu_imp_vertex_t);
	unsigned int Offset=0;

	ID3D11DeviceContext_IASetVertexBuffers(rx.d3d11.ctx,0,1,
	(ID3D11Buffer**)&rx.imp.asm_vtx.unknown,&Stride,&Offset);
	ID3D11DeviceContext_IASetIndexBuffer(rx.d3d11.ctx,
	(ID3D11Buffer *) rx.imp.asm_idx.unknown,DXGI_FORMAT_R32_UINT,0);
/* get this from the right place */
// D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
	ID3D11DeviceContext_IASetPrimitiveTopology(rx.d3d11.ctx,
	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

#include "rx.buffer.cc"

/* todo: deprecated, should use the appropiate pipenv function */
void rxpipset_viewport(float w, float h)
{ D3D11_VIEWPORT viewport_d3d;
	viewport_d3d.TopLeftX=0;
	viewport_d3d.TopLeftY=0;
	viewport_d3d.   Width=w;
	viewport_d3d.  Height=h;
	viewport_d3d.MinDepth=0;
	viewport_d3d.MaxDepth=1;
	ID3D11DeviceContext_RSSetViewports(rx.d3d11.ctx,1,&viewport_d3d);
}


void rximp_clip(int x0, int y0, int x1, int y1)
{
	ccassert(x0 <= x1);
	ccassert(y0 <= y1);

/* todo: */
	x0 = rxclampi(x0,0,rx.wnd.size_x);
	y0 = rxclampi(y0,0,rx.wnd.size_y);
	x1 = rxclampi(x1,0,rx.wnd.size_x);
	y1 = rxclampi(y1,0,rx.wnd.size_y);

	D3D11_RECT rect_d3d;
	rect_d3d.left  = x0;
	rect_d3d.top   = rx.wnd.size_y - y1;
	rect_d3d.right = x1;
	rect_d3d.bottom= rx.wnd.size_y - y0;
	ID3D11DeviceContext_RSSetScissorRects(rx.d3d11.ctx,1,&rect_d3d);
}

#include "emu_texture.c"
#include "emu_shader.c"
#include "emu_imp.c"

ccfunc ccinle void
Emu_window_clear()
{
/* should be the window stuff here instead dude #todo */
	float clear_color[]={.0f,.0f,.0f,1.f};

	ID3D11DeviceContext_ClearDepthStencilView(rx.d3d11.ctx,
	rx.pip.out.depth->d3d11.depth_target,
	D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,1.0f,0);

	for(int i=0; i<rx.pip.out.count; i+=1)
	{
		ID3D11DeviceContext_ClearRenderTargetView(rx.d3d11.ctx,
		rx.pip.out.color[i]->d3d11.color_target,clear_color);
	}
}

#include "rx.win32.cc"

void rxtime()
{
	ccclocktick_t ticks=ccclocktick();

	rx.total_ticks=ticks-rx.start_ticks;
	rx.total_seconds=ccclocksecs(rx.total_ticks);

	rx.delta_ticks=ticks-rx.frame_ticks;
	rx.delta_seconds=ccclocksecs(rx.delta_ticks);

	rx.frame_ticks=ticks;
}

int rlTick()
{
	rx.tick_count += 1;

	Emu_window_poll();

/* todo */
	Emu_system_set_cursor(rx.win32.cursor.arrow);

	Emu_imp_flush();

/* this has to be formalized #todo */
	Emu_texture_copy(
	rx.wnd.out.tar,rx.pip.out.color[0]);

	IDXGISwapChain_Present(rx.wnd.out.d3d11.swap_chain,1u,0);

/* this does not serve any purpose in full screen mode */
	WaitForSingleObjectEx(rx.wnd.out.d3d11.frame_await,INFINITE,TRUE);

	Emu_window_show();

	Emu_window_clear();

	rxtime();

	return !rx.wnd.off;
}

Emu_texture_t *
Emu_window_create_color_target()
{
	Emu_texture_t *result = NULL;

	IDXGIDevice * device_dxgi = NULL;
	IDXGIAdapter * adapter_dxgi = NULL;
	IDXGIFactory2 * factory_dxgi = NULL;

	ID3D11Device_QueryInterface(rx.d3d11.dev,&IID_IDXGIDevice,(void **)&device_dxgi);
	IDXGIDevice_GetAdapter(device_dxgi,&adapter_dxgi);
	IDXGIAdapter_GetParent(adapter_dxgi,&IID_IDXGIFactory2,(void**)&factory_dxgi);

	// DXGI_USAGE_UNORDERED_ACCESS
	DXGI_SWAP_CHAIN_DESC1 sc_config_d3d;
	ZeroMemory(&sc_config_d3d,sizeof(sc_config_d3d));
	sc_config_d3d.Width=rx.wnd.size_x;
	sc_config_d3d.Height=rx.wnd.size_y;
	sc_config_d3d.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
	sc_config_d3d.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc_config_d3d.BufferCount=2;
	sc_config_d3d.SwapEffect=DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	sc_config_d3d.Flags=DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH|
	DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
	/* todo: allow the user to specify this */
	sc_config_d3d.SampleDesc.  Count=1;
	sc_config_d3d.SampleDesc.Quality=0;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC sc_fs_config;
	ZeroMemory(&sc_fs_config,sizeof(sc_fs_config));
	sc_fs_config.RefreshRate.Numerator=_RX_REFRESH_RATE;
	sc_fs_config.RefreshRate.Denominator=1;
	sc_fs_config.Windowed=TRUE;

	IDXGIFactory2_CreateSwapChainForHwnd(factory_dxgi,
	(IUnknown *)rx.d3d11.dev,rx.wnd.win32.obj,
	&sc_config_d3d,&sc_fs_config,NULL,
	(IDXGISwapChain1 **)&rx.wnd.out.d3d11.swap_chain);

	rx.wnd.out.d3d11.frame_await = IDXGISwapChain2_GetFrameLatencyWaitableObject(rx.wnd.out.d3d11.swap_chain);


	ID3D11Texture2D *texture_d3d;
	IDXGISwapChain_GetBuffer(rx.wnd.out.d3d11.swap_chain,0,&IID_ID3D11Texture2D,(void **)&texture_d3d);

	Emu_texture_config_t config;
	ZeroMemory(&config,sizeof(config));
	config.memtype = D3D11_USAGE_DEFAULT;
	config.useflag = D3D11_BIND_RENDER_TARGET;
	config.size_x = sc_config_d3d. Width;
	config.size_y = sc_config_d3d.Height;
	config.format = sc_config_d3d.Format;
	config.d3d11.texture_2d = texture_d3d;

	result = Emu_texture_create(&config);

	IDXGIFactory_Release(factory_dxgi);
	IDXGIAdapter_Release(adapter_dxgi);
	IDXGIDevice_Release(device_dxgi);

	return result;
}

void rlInitWithWindow(const wchar_t *window_title) {
	rxsystem_init();

	UINT DriverModeFlags =
#ifdef RX_DEBUG_DEVICE
/* note: comment this out to use intel's graphic analyzer utility (user RenderDoc instead) */
	D3D11_CREATE_DEVICE_DEBUG|
	D3D11_CREATE_DEVICE_SINGLETHREADED|
#endif
	D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	D3D_FEATURE_LEVEL DriverFeatureMenu[2][2]= {
		{D3D_FEATURE_LEVEL_11_1,D3D_FEATURE_LEVEL_11_0},
		{D3D_FEATURE_LEVEL_10_1,D3D_FEATURE_LEVEL_10_0},
	};

	D3D_FEATURE_LEVEL DriverSelectedFeatureLevel;
	if(SUCCEEDED(D3D11CreateDevice(NULL,D3D_DRIVER_TYPE_HARDWARE,0,DriverModeFlags,DriverFeatureMenu[0],
	ARRAYSIZE(DriverFeatureMenu[0]),D3D11_SDK_VERSION,&rx.d3d11.dev,&DriverSelectedFeatureLevel,&rx.d3d11.ctx))||
	SUCCEEDED(D3D11CreateDevice(NULL,D3D_DRIVER_TYPE_WARP,0,DriverModeFlags,DriverFeatureMenu[1],
	ARRAYSIZE(DriverFeatureMenu[1]),D3D11_SDK_VERSION,&rx.d3d11.dev,&DriverSelectedFeatureLevel,&rx.d3d11.ctx)))
	{ if((DriverModeFlags&D3D11_CREATE_DEVICE_DEBUG))
		{ if(SUCCEEDED(IProvideClassInfo_QueryInterface(rx.d3d11.dev,&IID_ID3D11InfoQueue,(void**)&rx.d3d11.inf)))
			{
				ID3D11InfoQueue_SetBreakOnSeverity(rx.d3d11.inf, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
				ID3D11InfoQueue_SetBreakOnSeverity(rx.d3d11.inf, D3D11_MESSAGE_SEVERITY_ERROR,      TRUE);
				ID3D11InfoQueue_SetBreakOnSeverity(rx.d3d11.inf, D3D11_MESSAGE_SEVERITY_WARNING,    TRUE);
			}
		}
	}


	rxwindow_init(window_title);
	Emu_window_poll();

// note: we can use the adapter to enumerate display devices,
// this might come useful to the user!

	rx.wnd.out.tar = Emu_window_create_color_target();

	rx.pip.out.count = 1;
	rx.pip.out.color[0] = Emu_texture_create_color_target(
	rx.wnd.out.tar->size_x,
	rx.wnd.out.tar->size_y, rx.wnd.out.tar->format,_RX_MSAA,0);

	rx.pip.out.depth = Emu_texture_create_depth_target(
	rx.wnd.out.tar->size_x,
	rx.wnd.out.tar->size_y, DXGI_FORMAT_D32_FLOAT);

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

	ID3D11Device_CreateDepthStencilState(rx.d3d11.dev,&stencil_config_d3d,
	&rx.pip.d3d11.ds);

	Emu_imp_init();

/* todo */
	D3D11_RASTERIZER_DESC raster_config_d3d;
	ZeroMemory(&raster_config_d3d,sizeof(raster_config_d3d));
	raster_config_d3d.             FillMode=D3D11_FILL_SOLID;
	raster_config_d3d.             CullMode=D3D11_CULL_NONE;
	raster_config_d3d.FrontCounterClockwise=FALSE;
	raster_config_d3d.            DepthBias=D3D11_DEFAULT_DEPTH_BIAS;
	raster_config_d3d.       DepthBiasClamp=D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
	raster_config_d3d. SlopeScaledDepthBias=D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	raster_config_d3d.      DepthClipEnable= FALSE;
	raster_config_d3d.        ScissorEnable= TRUE;
	raster_config_d3d.    MultisampleEnable= _RX_MSAA >= 2;
	raster_config_d3d.AntialiasedLineEnable= FALSE;
	ID3D11Device_CreateRasterizerState(rx.d3d11.dev,&raster_config_d3d,&rx.pip.d3d11.rastr_state);

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

	ID3D11Device_CreateBlendState(rx.d3d11.dev,
	&blender_config_d3d,&rx.imp.d3d11.default_blend_state);

	blender_config_d3d.RenderTarget[0].BlendEnable = TRUE;
	blender_config_d3d.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC1_COLOR;
	blender_config_d3d.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC1_COLOR;
	blender_config_d3d.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blender_config_d3d.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blender_config_d3d.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blender_config_d3d.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blender_config_d3d.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	ID3D11Device_CreateBlendState(rx.d3d11.dev,
	&blender_config_d3d,&rx.imp.d3d11.subpixel_dual_blending_blend_state);

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

	rximp_clip(0,0,rx.wnd.size_x,rx.wnd.size_y);

	rxpipset_viewport(rx.wnd.size_x,rx.wnd.size_y);

	ID3D11DeviceContext_RSSetState(rx.d3d11.ctx,rx.pip.d3d11.rastr_state);

	rx.start_ticks=ccclocktick();
	rx.frame_ticks=rx.start_ticks;
	rxtime();
}

#pragma warning(pop)
#endif