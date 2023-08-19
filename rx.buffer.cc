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

void
rxreturn(
	rxborrowed_t borrowed )
{
  ID3D11DeviceContext_Unmap(rx.d3d11.ctx,borrowed.d3d11.resource,0);
}

rxborrowed_t
rxunknown_borrow(
	rxunknown_t buffer )
{
	rxborrowed_t result;
	ZeroMemory(&result,sizeof(result));

  // note: ensures this is a valid buffer!
  ID3D11Resource *Resource;
  if(SUCCEEDED(IUnknown_QueryInterface(buffer,&IID_ID3D11Buffer,&Resource)))
  {
    D3D11_MAPPED_SUBRESOURCE MappedAccess;
    ID3D11DeviceContext_Map(rx.d3d11.ctx,Resource,0,D3D11_MAP_WRITE_DISCARD,0,&MappedAccess);

    result.d3d11.resource = Resource;
    result.        length = MappedAccess.RowPitch;
    result.        memory = MappedAccess.pData;

    ccassert(result.length != 0);
    ccassert(result.memory != 0);
  }

  return result;
}

rxborrowed_t
rxuniform_buffer_borrow(
	rxuniform_buffer_t buffer )
{
	/* do the type checking thing here - XXX - the one called rj */
  return rxunknown_borrow(buffer.unknown);
}

void
rxuniform_buffer_delete(
  rxuniform_buffer_t buffer)
{
  if(buffer.unknown != 0)
  {
    ID3D11Buffer *the_buffer;
    if(SUCCEEDED(IUnknown_QueryInterface(buffer.unknown,&IID_ID3D11Buffer,&the_buffer)))
    {
      IUnknown_Release(the_buffer);

      IUnknown_Release(buffer.unknown);
    }
  }
}

rxuniform_buffer_t
rxuniform_buffer_create(
  unsigned int length, void *memory)
{

  D3D11_BUFFER_DESC the_buffer_info;
  the_buffer_info.              Usage = D3D11_USAGE_DYNAMIC;
  the_buffer_info.     CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  the_buffer_info.          MiscFlags = 0;
  the_buffer_info.StructureByteStride = 0;
  the_buffer_info.          BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  the_buffer_info.          ByteWidth = (UINT)((length+15)/16*16);

  ID3D11Buffer *the_buffer;
  ID3D11Device_CreateBuffer(rx.d3d11.dev,&the_buffer_info,NULL,&the_buffer);

  return RX_TLIT(rxuniform_buffer_t){(rxunknown_t)(the_buffer)};
}

rxindex_buffer_t
rxcreate_index_buffer(
	size_t index_size, size_t index_count)
{
  D3D11_BUFFER_DESC the_buffer_info;
  the_buffer_info.              Usage = D3D11_USAGE_DYNAMIC;
  the_buffer_info.     CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  the_buffer_info.          MiscFlags = 0;
  the_buffer_info.StructureByteStride = 0;
  the_buffer_info.          BindFlags = D3D11_BIND_INDEX_BUFFER;
  the_buffer_info.          ByteWidth = (UINT)(index_size * index_count);

  ID3D11Buffer *the_buffer;
  ID3D11Device_CreateBuffer(rx.d3d11.dev,&the_buffer_info,NULL,&the_buffer);

  return RX_TLIT(rxindex_buffer_t){(rxunknown_t)(the_buffer)};
}

rxvertex_buffer_t
rxcreate_vertex_buffer(
	size_t vertex_size, size_t vertex_count)
{
  D3D11_BUFFER_DESC the_buffer_info;
  the_buffer_info.              Usage = D3D11_USAGE_DYNAMIC;
  the_buffer_info.     CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  the_buffer_info.          MiscFlags = 0;
  the_buffer_info.StructureByteStride = 0;
  the_buffer_info.          BindFlags = D3D11_BIND_VERTEX_BUFFER;
  the_buffer_info.          ByteWidth = (UINT)(vertex_size * vertex_count);

  ID3D11Buffer *the_buffer;
  ID3D11Device_CreateBuffer(rx.d3d11.dev,&the_buffer_info,NULL,&the_buffer);

  return RX_TLIT(rxvertex_buffer_t){(rxunknown_t)(the_buffer)};
}

rxstruct_buffer_t
rxcreate_struct_buffer(
	size_t struct_size, size_t struct_count)
{
  D3D11_BUFFER_DESC the_buffer_info;
  the_buffer_info.              Usage=D3D11_USAGE_DYNAMIC;
  the_buffer_info.     CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
  the_buffer_info.          MiscFlags=D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
  the_buffer_info.StructureByteStride=(UINT)(struct_size);
  the_buffer_info.          BindFlags=D3D11_BIND_SHADER_RESOURCE;
  the_buffer_info.          ByteWidth=(UINT)(struct_size * struct_count);

  ID3D11Buffer *the_buffer;
  ID3D11Device_CreateBuffer(rx.d3d11.dev,&the_buffer_info,NULL,&the_buffer);

  D3D11_SHADER_RESOURCE_VIEW_DESC D;
  D.             Format=DXGI_FORMAT_UNKNOWN;
  D.      ViewDimension=D3D11_SRV_DIMENSION_BUFFER;
  D.Buffer.FirstElement=0;
  D.Buffer. NumElements=(UINT)(struct_count);

  ID3D11ShaderResourceView *View;
  ID3D11Device_CreateShaderResourceView(rx.d3d11.dev,(ID3D11Resource*)the_buffer,&D,&View);

  rxstruct_buffer_t result;
  result.unknown=(rxunknown_t)View;

  return result;
}

void
rxuniform_buffer_update(
	rxuniform_buffer_t uniform, void *memory, size_t length)
{
  rxborrowed_t b = rxuniform_buffer_borrow(uniform);
  memcpy(b.memory,memory,length);
  rxreturn(b);
}

rxborrowed_t
rxborrow_vertex_buffer(
	rxvertex_buffer_t buffer)
{
  return rxunknown_borrow(buffer.unknown);
}

rxborrowed_t
rxborrow_index_buffer(
	rxindex_buffer_t buffer)
{
  return rxunknown_borrow(buffer.unknown);
}

rxborrowed_t
rxborrow_struct_buffer(
	rxstruct_buffer_t buffer)
{
	/* this a leak - XXX - */
  ID3D11Resource *the_resource;
  ID3D11View_GetResource((ID3D11View*)buffer.unknown,&the_resource);

  return rxunknown_borrow((rxunknown_t)the_resource);
}