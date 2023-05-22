#ifndef _RXUNIFORM_BUFFER_HEADER
#define _RXUNIFORM_BUFFER_HEADER


#endif

#ifdef _RXUNIFORM_BUFFER_IMPLEMENTATION


rxborrowed_t
rxborrow_typeless_buffer(rxunknown_t buffer)
{
  rxborrowed_t result;
  result.resource=0;
  result.  length=0;
  result.  memory=0;

  // note: ensures this is a valid buffer!
  ID3D11Resource *Resource;
  if(SUCCEEDED(
      IUnknown_QueryInterface(buffer,&IID_ID3D11Buffer,&Resource)))
  {
    D3D11_MAPPED_SUBRESOURCE MappedAccess;
    ID3D11DeviceContext_Map(rx.Context,Resource,0,D3D11_MAP_WRITE_DISCARD,0,&MappedAccess);

    result.resource=Resource;
    result.  length=MappedAccess.RowPitch;
    result.  memory=MappedAccess.pData;

    ccassert(result.length != 0);
    ccassert(result.memory != 0);
  }

  return result;
}

void
rxdevice_bind_uniform_buffer(
  rxuniform_buffer_t buffer, int slot)
{
  ccassert(buffer.unknown != 0);

  // todo!!:
  ID3D11Buffer *Buffer;
  if(SUCCEEDED(ID3D11DeviceChild_QueryInterface(buffer.unknown,&IID_ID3D11Buffer,&Buffer)))
  { ID3D11DeviceChild_Release(Buffer);

    if(rxshader_typeof_vertex(rx.shader))
      ID3D11DeviceContext_VSSetConstantBuffers(rx.Context,slot,1,&Buffer);
    else
    if(rxshader_typeof_pixel(rx.shader))
      ID3D11DeviceContext_PSSetConstantBuffers(rx.Context,slot,1,&Buffer);
    else
    if(rxshader_typeof_compute(rx.shader))
      ID3D11DeviceContext_CSSetConstantBuffers(rx.Context,slot,1,&Buffer);
  }
}

void
rxdelete_uniform_buffer(
  rxuniform_buffer_t buffer)
{
  if(buffer.unknown != 0)
  {
    ID3D11Buffer *Buffer;
    if(SUCCEEDED(IUnknown_QueryInterface(buffer.unknown,&IID_ID3D11Buffer,&Buffer)))
    {
      IUnknown_Release(Buffer);

      IUnknown_Release(buffer.unknown);
    }
  }
}

rxuniform_buffer_t
rxcreate_uniform_buffer(
  unsigned int length, void *memory)
{

  D3D11_BUFFER_DESC BufferI;
  BufferI.              Usage=D3D11_USAGE_DYNAMIC;
  BufferI.     CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
  BufferI.          MiscFlags=0;
  BufferI.StructureByteStride=0;
  BufferI.          BindFlags=D3D11_BIND_CONSTANT_BUFFER;
  BufferI.          ByteWidth=(UINT)((length+15)/16*16);

  ID3D11Buffer *Buffer;
  ID3D11Device_CreateBuffer(rx.Device,&BufferI,NULL,&Buffer);

  return (rxuniform_buffer_t){(rxunknown_t)(Buffer)};
}

rxborrowed_t rxborrow_uniform_buffer(rxuniform_buffer_t buffer)
{
  return rxborrow_typeless_buffer(buffer.unknown);
}

void rxupdate_uniform_buffer(rxuniform_buffer_t uniform, void *memory, size_t length)
{
  rxborrowed_t b=rxborrow_uniform_buffer(uniform);
  memcpy(b.memory,memory,length);
  rxreturn(b);
}

#endif