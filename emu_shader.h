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

enum
{ EMU_kINVALID            = 0 << 0,
  EMU_kPIXEL_SHADER_BIT   = 1 << 1,
  EMU_kVERTEX_SHADER_BIT  = 1 << 2,
  EMU_kCOMPUTE_SHADER_BIT = 1 << 3,
  /* in my rather narrow experience with d3d-xx this
    'dual source blending' feature seems to be done quite differently
    on other API's, in d3d11 the pixel shader must output two values
    and yet exactly one render target must be bound, otherwise you
    get an error, to account for this indifference, this flag can be
    specified if need be, though it needs a better name #pending */
  EMU_kSOURCE_BLENDING_BIT = 1 << 4,
};

typedef struct
{
  /* optional */
  char const  *debug_label;
  /* optional */
  char const  *model;
  /* optional */
  char const  *entry;

  struct
  { size_t     length;
    void      *memory; };

} Emu_shader_compile_config_t;

typedef struct
{
  /* pass in the bytecode directly */
  struct
  { size_t length;

    union
    { void *memory;
      char *string; };
  } bytecode;

  Emu_shader_compile_config_t compile;

} Emu_shader_source_config_t;

typedef struct
{

  /* #todo */
  D3D11_INPUT_ELEMENT_DESC attr_array[0x20];
                       int attr_count;

  struct
  {

    ID3D11InputLayout *layout;
  } d3d11;
} Emu_shader_layout_config_t;

typedef struct
{
  int flags;

  char const *label;

  /* Emu_shader_t expected_color_attachments :: for pixel shaders only,
      further distinction could be made across shaders for extra type safety but that's just a
      little too overkill given that this module is not meant to be the forefront of rendering
      API as a whole.

    * d3d11 specific: if the dual source blending bit is specified the pipeline must
    use exactly one render target and the pixel shader must express exactly two
    color outputs, the init functions takes this into account and ensures (if possible)
    through reflection that only one color attachment is indeed expected */
  int expected_color_attachments;

  /* probably rename this #todo */
  Emu_shader_source_config_t source;
  Emu_shader_layout_config_t layout;

  /* by default layouts are not created for pixel shaders,
    force it with this flag */
  unsigned force_create_layout: 1;
  unsigned donot_create_layout: 1;
} Emu_shader_config_t;

typedef struct
{
  char const *label;

  int flags;
  int expected_color_attachments;

  struct
  { union
    { ID3D11DeviceChild    *unknown;
      ID3D11VertexShader   *vertex_shader;
      ID3D11PixelShader    *pixel_shader;
      ID3D11ComputeShader  *compute_shader; };

    ID3D11InputLayout *layout;
  } d3d11;
} Emu_shader_t;

/* beware that this function may not handle padding very well should
 you choose to let the API create the input layout for you automatically using
 the reflection API, ideally you should stick to aligned units,
 try to pack things into float4's if nothing else works. #important */
void
Emu_shader_init(
  Emu_shader_t *shader, Emu_shader_config_t *config);

Emu_shader_t
Emu_shader_create_bytecode(
  int flags, char const *label, size_t length, void *memory);

Emu_shader_t
Emu_shader_load(
  int flags, char const *entry, char const *fpath);
