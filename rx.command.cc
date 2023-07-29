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

/* Perhaps all command thingies should be rxc_*xxx* instead to make it clear */

const char *
rxcomtag(
  const char *label)
{
  const char *the_label = label ? label : rx.command_name;

  rx.command_name = label;

  return the_label;
}

rxcom_t *
rxcomadd(
  rx_k type )
{
  if(rx.command_tally >= RX_COMMAND_BUFFER_SIZE)
  {
    cctracewar("'%i': command limit reached", rx.command_tally);

    rx.command_tally = RX_COMMAND_BUFFER_SIZE - 1;
  }

  rxcom_t *the_command;

  the_command = &rx.command_array[
                 rx.command_tally];

  rx.command_tally += 1;

  the_command-> kind = type;
  the_command->label = rxcomtag(0);

  return the_command;
}

void rxenable_ztesting(int enable)
{
  if(enable)
  rxcomtag("ztest::on" ); else
  rxcomtag("ztest::off");

  rxcom_t *com = rxcomadd(rx_kENABLE_DEPTH_TESTING);
  com->enable = enable;
}

void rx3d()
{
  rxcomtag("mode::3d");
  rxcomadd(rx_kMODE3D);

#ifdef _RX_ENABLE_DEPTH_STENCIL
  rxenable_ztesting(TRUE);
#endif
}

void rx2d()
{
  rxcomtag("mode::2d");
  rxcomadd(rx_kMODE2D);

#ifdef _RX_ENABLE_DEPTH_STENCIL
  rxenable_ztesting(FALSE);
#endif
}

void rxclip(int x0, int y0, int x1, int y1)
{
  ccassert(x0 <= x1);
  ccassert(y0 <= y1);

  /* this has to be revised - XXX - the one called rj */
  x0 = rxclampi(x0,0,rx.size_x);
  y0 = rxclampi(y0,0,rx.size_y);
  x1 = rxclampi(x1,0,rx.size_x);
  y1 = rxclampi(y1,0,rx.size_y);

  rxcom_t *c = rxcomadd(rx_kCLIP);
  c->x0 = x0; c->y0 = y0;
  c->x1 = x1; c->y1 = y1;
}

void rxsampler_bind_ex(rxsampler_t sampler, int offset)
{
  rxcom_t *com = rxcomadd(rx_kSAMPLER);
  com->sampler = sampler;
  com-> offset = offset;
}

void rxsampler_bind(rxsampler_t sampler)
{
  rxsampler_bind_ex(sampler,0);
}

void rxtexture_bind_ex(rxtexture_t texture, int offset)
{
  rxcom_t *com = rxcomadd(rx_kTEXTURE);
  com->texture = texture;
  com-> offset = offset;
}

void rxtexture_bind(rxtexture_t texture)
{
  rxtexture_bind_ex(texture,0);
}

int rxcomexc(rxcom_t *com, int index_offset)
{
  int result = 0;

  switch(com->kind)
  {
    case rx_kENABLE_DEPTH_TESTING:
    {
      if(com->enable)
        rx.the_stencil_state = rx.the_stencil_state_on;
      else
        rx.the_stencil_state = rx.the_stencil_state_off;

      ID3D11DeviceContext_OMSetDepthStencilState(rx.Context,rx.the_stencil_state,1);
    } break;
    case rx_kMODE2D:
    { rx.world_matrix = rxmatrix_identity();
      rx. view_matrix = rxmatrix_identity();
      rx.view_matrix.m[0][0]=  2. / rx.size_x;
      rx.view_matrix.m[1][1]=  2. / rx.size_y;
      rx.view_matrix.m[2][2]= .5;
      rx.view_matrix.m[3][0]=- 1.;
      rx.view_matrix.m[3][1]=- 1.;
    } break;
    /* This is temporary, ideally we'd use something like a camera or whatnot - XXX - the one called rj */
    case rx_kMODE3D:
    { rx.world_matrix = rxmatrix_identity();
      rx. view_matrix = rxmatrix_identity();
      rx. view_matrix = rxmatrix_projection(
      	(double)(rx.size_y)/(double)(rx.size_x),90,0.01,1000);
    } break;
    case rx_kCLIP:
    { /* this sort of caculation should be done when the command is added
       and not at execution time? - XXX - the one called rj */
      D3D11_RECT the_clip_rect;
      the_clip_rect.left  = com->x0;
      the_clip_rect.top   = rx.size_y - com->y1;
      the_clip_rect.right = com->x1;
      the_clip_rect.bottom= rx.size_y - com->y0;
      ID3D11DeviceContext_RSSetScissorRects(rx.Context,1,&the_clip_rect);
    } break;
    case rx_kPULLTARGET:
    {
      rx.target=rx.target_stack[--rx.target_index];
    } break;
    case rx_kPUSHTARGET:
    {
      rx.target_stack[rx.target_index++]=rx.target;
      rx.target=com->target;

      float color[]={0,0,0,0};
      rxrender_target_apply(com->target,color);

    } break;
    // case rx_kCLEAR:
    // { float color[]={com->r,com->g,com->b,com->a};
    //   ID3D11DeviceContext_ClearRenderTargetView(rx.Context,
    //     (ID3D11RenderTargetView*)rx.target.unknown,color);
    // } break;
    case rx_kCUSTOM:
    {
      ccassert(com->custom!=0);
      com->custom();
    } break;
    // todo: this is to be revised!
    case rx_kMATRIX:
    {
      rx.world_matrix = com->matrix;
    } break;
    case rx_kPUSHSHADER:
    {
      ccassert(com->shader.unknown!=0);

      ccassert(rx.shader_index < ccCarrlenL(rx.shader_stack));

      rx.shader_stack[rx.shader_index++]=rx.shader;
      rxshader_apply(com->shader);
    } break;
    case rx_kPULLSHADER:
    { ccassert(rx.shader_index > 0);

      rxshader_t shader=rx.shader_stack[--rx.shader_index];
      rxshader_apply(shader);
    } break;
    case rx_kUNIFORM:
    {
#if 0
      ccassert(com->unknown!=0);
      ccassert(rx.LastShaderSetByUser !=0 ||
        cctraceerr("'no shader': expected a previous command to have been a shader"));
      if(SUCCEEDED(IUnknown_QueryInterface(com->unknown,&IID_ID3D11Buffer,&rx.LastBufferSetByUser)))
      {
        ID3D11DeviceChild *DummyShader;
        if(SUCCEEDED(IUnknown_QueryInterface(rx.LastShaderSetByUser,&IID_ID3D11VertexShader,&DummyShader)))
          ID3D11DeviceContext_VSSetConstantBuffers(rx.Context,0,1,(ID3D11Buffer**)&rx.LastBufferSetByUser);
        else
        if(SUCCEEDED(IUnknown_QueryInterface(rx.LastShaderSetByUser,&IID_ID3D11PixelShader,&DummyShader)))
          ID3D11DeviceContext_PSSetConstantBuffers(rx.Context,0,1,(ID3D11Buffer**)&rx.LastBufferSetByUser);
        else
          cctracewar("'rxqueue_shader_command()': expected vertex or pixel shader",0);
      } else
          cctraceerr("'rxqueue_shader_command()': invalid interface, expected buffer",0);
#endif
    } break;
    case rx_kSAMPLER:
    {

      rxsampler_apply(com->sampler,com->offset);

    } break;
    case rx_kTEXTURE:
    {
      rxtexture_apply(com->texture,com->offset);
    } break;
    case rx_kINDEXED:
    {
      rxmatrix_t matrix = rxmatrix_multiply(rx.world_matrix,rx.view_matrix);

      rxshader_builtin_uniform_t t;
      t.       matrix=matrix;
      t. screen_xsize=(float)(rx.size_x);
      t. screen_ysize=(float)(rx.size_y);
      t.mouse_xcursor=(float)(rx.xcursor) / rx.size_x; // todo!!:
      t.mouse_ycursor=(float)(rx.ycursor) / rx.size_y; // todo!!:
      t.total_seconds=rx.total_seconds;
      t.delta_seconds=rx.delta_seconds;
      t. shadow_tally=rx.shadow_tally;
      t. candle_tally=rx.candle_tally;

      rxuniform_buffer_update(rx.uniform_buffer,&t,sizeof(t));

      ID3D11DeviceContext_DrawIndexed(rx.Context,com->length,index_offset,com->offset);
      result += com->length;

    } break;
  }

  return result;
}
