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

/* We're going for simplicty here, this should be enough for most simple
 things, you can re-define most of these */
typedef int rxindex_t;

typedef union rxvertex_t rxvertex_t;
typedef union rxvertex_t
{ struct
	{ rxvec4_t  xyzw;
		rxvec2_t  uv;
		rxcolor_t rgba;
	};
	struct
	{ float x,y,z,w;
	  float u,v;
		float r,g,b,a;
	};
} rxvertex_t;

typedef struct rxvertex_layout_t rxvertex_layout_t;
typedef struct rxvertex_layout_t
{
  rxunknown_t unknown;
} rxvertex_layout_t;

typedef struct rxshader_t rxshader_t;
typedef struct rxshader_t
{
  rxunknown_t unknown;
} rxshader_t;

typedef struct rxshader_builtin_uniform_t rxshader_builtin_uniform_t;
typedef struct rxshader_builtin_uniform_t
{
  rxmatrix_t      matrix;

  float     screen_xsize;
  float     screen_ysize;
  float    mouse_xcursor;
  float    mouse_ycursor;
  double   total_seconds;
  double   delta_seconds;
  int       shadow_tally;
  int       candle_tally;
  int            padding[2];
} rxshader_builtin_uniform_t;

void
rxshader_apply(
  rxshader_t shader);

rxblobber_t
rxcompile_shader_bytecode(
  unsigned int   bytecode_length,
          void * bytecode_memory,
    const char *  entry,
    const char *  model,
    const char * master);

rxblobber_t
rxcompile_shader(
  const char   *entry,
  const char   *model,
  const char  *master);

rxshader_t
rxload_vertex_shader(
  const char *   entry,
  const char *  master);

rxshader_t
rxload_pixel_shader(
  const char *   entry,
  const char *  master);
