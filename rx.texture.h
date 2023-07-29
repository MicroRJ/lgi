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

typedef struct rxoffline_texture_t rxoffline_texture_t;
typedef struct rxoffline_texture_t
{  int   size_x, size_y;
   int   format;
   int   stride;
  void * memory;
} rxoffline_texture_t;

typedef struct rxtexture_config_t
{
          int             size_x;
          int             size_y;
  DXGI_FORMAT             format;
          int             stride;
         void            *memory;
  D3D11_USAGE              usage;
          int         bind_flags;
          int  host_access_flags;
          int       sample_count;
          int     sample_quality;
} rxtexture_config_t;

ccfunc rxtexture_config_t
rxtexture_config_init(
          int             size_x,
          int             size_y,
  DXGI_FORMAT             format,
          int             stride,
          void           *memory,
  D3D11_USAGE              usage,
          int         bind_flags,
          int  host_access_flags,
          int       sample_count,
          int     sample_quality );

typedef struct rxtexture_t rxtexture_t;
typedef struct rxtexture_t
{
	// { rxunknown_t the_resource;
	// 	 rxunknown_t the_view;
	// }
	rxunknown_t unknown;
        int   size_x, size_y;
        int   format;
} rxtexture_t;

ccfunc int
rxtexure_init(
	rxtexture_t *texture, rxtexture_config_t *config);


typedef union rxrender_target_t rxrender_target_t;
typedef union rxrender_target_t
{ rxtexture_t texture;
  struct {
	rxunknown_t unknown;
        int   size_x, size_y;
        int   format; };
} rxrender_target_t;

rxoffline_texture_t
rxtexture_load(
  const char *name);




ccfunc rxtexture_t
rxtexture_create_untyped(
          int             size_x,
          int             size_y,
  DXGI_FORMAT             format,
          int             stride,
          void           *memory,
  D3D11_USAGE              usage,
          int         bind_flags,
          int  host_access_flags,
          int       sample_count,
          int     sample_quality );

ccfunc rxtexture_t
rxtexture_create_ex(
          int  size_x,
          int  size_y,
  DXGI_FORMAT  format,
          int  stride,
        void  *memory);

ccfunc rxtexture_t
rxtexture_create(
  int  size_x,
  int  size_y,
  int  format);

ccfunc void
rxtexture_apply(
  rxtexture_t texture,
          int    slot);

ccfunc void
rxtexture_bind(
	rxtexture_t texture);

ccfunc rxborrowed_t
rxtexture_borrow(
  rxtexture_t texture);

ccfunc rxtexture_t
rxtexture_upload(
  rxoffline_texture_t texture);

ccfunc rxtexture_t
rxtexture_upload_byname(
  const char *name);

void
rxrender_target_copy(
  rxrender_target_t l, rxrender_target_t r);

void
rxrender_target_apply(
  rxrender_target_t target, float *clear_color);
