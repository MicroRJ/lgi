#ifndef _RXSHADER_HEADER
#define _RXSHADER_HEADER

typedef struct rxblobber_t rxblobber_t;
typedef struct rxblobber_t
{
  ID3DBlob    *unknown;

  void    *memory;
  size_t   length;
} rxblobber_t;

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

typedef struct rxuniform_t rxuniform_t;
typedef struct rxuniform_t
{
  // todo!!!: rename
  rxmatrix_t e;

  float    screen_xsize;
  float    screen_ysize;
  float   mouse_xcursor;
  float   mouse_ycursor;
  double  total_seconds;
  double  delta_seconds;
  int      shadow_tally;
  int      candle_tally;
  int           padding[2];
} rxuniform_t;

typedef int rxindex_t;

// todo: migrate to xyz
typedef struct rxvertex_t rxvertex_t;
typedef struct rxvertex_t
{ float x, y;
  float u, v;
  unsigned char r,g,b,a;
} rxvertex_t;

ccfunc rxblobber_t
rxcompile_shader_bytecode(
  unsigned int   bytecode_length,
          void * bytecode_memory,
    const char *  entry,
    const char *  model,
    const char * master);

ccfunc rxblobber_t
rxcompile_shader(
  const char   *entry,
  const char   *model,
  const char  *master);

int
rxunknown_typeof_compute_shader(rxunknown_t unknown)
{
  ID3D11ComputeShader *I=NULL;
  if(SUCCEEDED(IUnknown_QueryInterface(unknown,&IID_ID3D11ComputeShader,&I)))
    ID3D11DeviceChild_Release(I);
  return I!=NULL;
}

int
rxunknown_typeof_vertex_shader(rxunknown_t unknown)
{
  ID3D11VertexShader *I=NULL;
  if(SUCCEEDED(IUnknown_QueryInterface(unknown,&IID_ID3D11VertexShader,&I)))
    ID3D11DeviceChild_Release(I);
  return I!=NULL;
}

int
rxunknown_typeof_pixel_shader(rxunknown_t unknown)
{
  ID3D11PixelShader *I=NULL;
  if(SUCCEEDED(IUnknown_QueryInterface(unknown,&IID_ID3D11PixelShader,&I)))
    ID3D11DeviceChild_Release(I);
  return I!=NULL;
}

int
rxshader_typeof_compute(rxshader_t shader)
{
  return rxunknown_typeof_compute_shader(shader.unknown);
}

int
rxshader_typeof_vertex(rxshader_t shader)
{
  return rxunknown_typeof_vertex_shader(shader.unknown);
}

int
rxshader_typeof_pixel(rxshader_t shader)
{
  return rxunknown_typeof_pixel_shader(shader.unknown);
}
#endif

#ifdef _RXSHADER_IMPLEMENTATION

void
rxdelete_blobbler(
  rxblobber_t bytecode)
{
  if(bytecode.unknown)
  {
    ID3D11DeviceChild_Release(bytecode.unknown);
  }
}

rxblobber_t
rxcompile_shader_bytecode(
  unsigned int   length,
          void * memory,
    const char *  entry,
    const char *  model,
    const char * master)
{
  rxblobber_t blobber=(rxblobber_t){0};

  if(length != 0 && memory != 0)
  {
    ID3DBlob *BytecodeBlob,*MessagesBlob;

    if(SUCCEEDED(
        D3DCompile(memory,length,master,0,0,entry,model,
          RX_SHADER_COMPILATION_FLAGS,0,&BytecodeBlob,&MessagesBlob)))
    {
      rxcontents_erroneous(master,ccfalse);

      blobber.unknown=BytecodeBlob;
      blobber. memory=BytecodeBlob->lpVtbl->GetBufferPointer(BytecodeBlob);
      blobber. length=BytecodeBlob->lpVtbl->   GetBufferSize(BytecodeBlob);
    } else
      rxcontents_erroneous(master,cctrue);
  }

  return blobber;
}

#endif