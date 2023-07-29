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
rxshader_apply(
	rxshader_t shader )
{
  int should_reload=ccfalse;

  // todo!!:
  if(rxunknown_typeof_vertex_shader(shader.unknown) &&
      (shader.unknown != rx.vertex_shader.unknown || should_reload))
  {
    rx.shader=rx.vertex_shader=shader;

    ID3D11DeviceContext_VSSetShader(rx.Context,(ID3D11VertexShader*)shader.unknown,0x00,0);

    rxarticle_t *tangible=cctblgetP(rx.instance_table,shader.unknown);
    ccassert(ccerrnon());

    ID3D11InputLayout *Layout=(ID3D11InputLayout *)tangible->linkage[rxlinkage_kVERTEX_LAYOUT];

    ID3D11DeviceContext_IASetInputLayout(rx.Context,Layout);
    goto leave;
  }

  if(rxunknown_typeof_pixel_shader(shader.unknown) &&
      (shader.unknown != rx.pixel_shader.unknown || should_reload))
  {
    rx.shader=rx.pixel_shader=shader;

    ID3D11DeviceContext_PSSetShader(rx.Context,(ID3D11PixelShader*)shader.unknown,0x00,0);
    goto leave;
  }

leave:;
}