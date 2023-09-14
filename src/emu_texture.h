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

#if 0
enum
{ EMU_HOST_READ_BIT         = 1 << 0,
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

typedef struct
{
  int    format;
  int    size_x;
  int    size_y;

  /* multisampling, default should be 1,0 */
  int    samples;
  int    quality;

  /* replace this with a single flag field instead #todo */
  struct
  { /* GPU allocation flags */
    D3D11_USAGE memtype;
            int useflag;
    /* CPU access flags */
            int memflag; };

  /* memory and stride are optional, if memory provided stride may not be 0 */
  int    stride;
  void  *memory;

  /* optional in/out */
  struct
  { union
    { ID3D11Resource         *resource;
      ID3D11Texture2D        *texture_2d; };

    ID3D11ShaderResourceView  *shader_target;
    ID3D11RenderTargetView     *color_target;
    ID3D11DepthStencilView     *depth_target;
  } d3d11;
} Emu_texture_config_t;

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
          int    memflag );

/* base texture object for all other textures */
typedef struct
{
  int format;
  int size_x;
  int size_y;
  int samples;
  int quality;

  /* feeling this out, do not use directly just in case #pending #important */
  struct
  {
  	int   stride;
  	void *memory;
  } mapped;

  /* doing too much? #pending
  struct
  {
	  void    *memory;
	  int      stride;
	  unsigned dirty: 1;
  } hosted; */

  struct
  { union
    { ID3D11Resource  *resource;
      ID3D11Texture2D *texture_2d; };

    /* views are optional  */
    ID3D11ShaderResourceView *shader_target;
    ID3D11RenderTargetView   *color_target;
    ID3D11DepthStencilView   *depth_target;
  } d3d11;

} Emu_texture_t;

typedef struct
{  int   size_x, size_y;
   int   format;
   int   stride;
  void * memory;
} Emu_texture_memory_t;

ccfunc Emu_texture_memory_t
Emu_texture_load( const char *name );

ccfunc Emu_texture_t *
Emu_texture_memory_upload( Emu_texture_memory_t texture );

ccfunc Emu_texture_memory_t
Emu_texture_memory_create( int size_x, int size_y, int format );

ccfunc emu_error
Emu_texture_init(
  Emu_texture_t *texture, Emu_texture_config_t *config);

ccfunc ccinle Emu_texture_t *
Emu_texture_create(
  Emu_texture_config_t *config );

ccfunc ccinle Emu_texture_t *
Emu_texture_create_simple(
          int  size_x,
          int  size_y,
  DXGI_FORMAT  format,
          int  stride,
        void  *memory);

ccfunc ccinle Emu_texture_t *
Emu_texture_create_depth_target(
  int size_x, int size_y, int format);

ccfunc ccinle Emu_texture_t *
Emu_texture_create_color_target(
  int size_x, int size_y, int format, int samples, int quality);

ccfunc ccinle void *
Emu_texture_borrow(
	Emu_texture_t *texture, int *stride);

ccfunc ccinle void
Emu_texture_return(
	Emu_texture_t *texture);

/* pass in the region to update #todo */
ccfunc ccinle void
Emu_texture_update(
  Emu_texture_t *texture, Emu_texture_memory_t memory_ )
{
	ccassert(memory_.format == texture->format);

	int   stride;
	void *memory = Emu_texture_borrow(texture,&stride);

	memcpy(memory,memory_.memory,memory_.size_y*memory_.stride);

	Emu_texture_return(texture);
}