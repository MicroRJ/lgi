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
rxsampler_apply(
  rxsampler_t sampler, int slot)
{
	/* XXXX */
  if(rxshader_typeof_vertex(rx.shader))
  {
    ID3D11DeviceContext_VSSetSamplers(rx.Context,slot,1,(ID3D11SamplerState**)&sampler.unknown);
  } else
  if(rxshader_typeof_pixel(rx.shader))
  {
    ID3D11DeviceContext_PSSetSamplers(rx.Context,slot,1,(ID3D11SamplerState**)&sampler.unknown);
  } else
  if(rxshader_typeof_compute(rx.shader))
  {
    ID3D11DeviceContext_CSSetSamplers(rx.Context,slot,1,(ID3D11SamplerState**)&sampler.unknown);
  }
}