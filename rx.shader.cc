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
rxshader_init(
  rxshader_t *shader, shader_config_t *config)
{
  ZeroMemory(shader,sizeof(shader));

  void *handle_to_close = NULL;
  void *memory_to_close = NULL;

  ID3DBlob *bytecode_blob_d3d = NULL;
  ID3DBlob *messages_blob_d3d = NULL;

  if(config->hlsl.bytecode.memory == NULL)
  {
    if(config->hlsl.source.model == NULL)
    {
      /* todo: get this from the device */
      if(config->type==RX_OBJECT_TYPE_kVERTEX_SHADER)
        config->hlsl.source.model = "vs_5_0";
      else
      if(config->type==RX_OBJECT_TYPE_kPIXEL_SHADER)
        config->hlsl.source.model = "ps_5_0";
      else
      if(config->type==RX_OBJECT_TYPE_kCOMPUTE_SHADER)
        config->hlsl.source.model = "cs_5_0";
    }
    if(config->hlsl.source.entry == NULL)
    {
      /* todo: get this from preferences */
      if(config->type==RX_OBJECT_TYPE_kVERTEX_SHADER)
        config->hlsl.source.entry = "MainVS";
      else
      if(config->type==RX_OBJECT_TYPE_kPIXEL_SHADER)
        config->hlsl.source.entry = "MainPS";
      else
      if(config->type==RX_OBJECT_TYPE_kCOMPUTE_SHADER)
        config->hlsl.source.entry = "MainCS";
    }

    ccassert(config->hlsl.source.entry != 0);
    ccassert(config->hlsl.source.model != 0);


    if(config->hlsl.source.memory == NULL)
    {
      ccu32_t length = 0;
      handle_to_close = ccopenfile(config->hlsl.source.fpath,"r");
      memory_to_close = ccpullfile(handle_to_close,0,&length);

      config->hlsl.source.memory = memory_to_close;
      config->hlsl.source.length = length;
    }


    if(FAILED(
        D3DCompile(
          config->hlsl.source.memory,
          config->hlsl.source.length,
          config->hlsl.source.label,0,0,
          config->hlsl.source.entry,
          config->hlsl.source.model,
          RX_SHADER_COMPILATION_FLAGS,0,
            &bytecode_blob_d3d,
            &messages_blob_d3d)))
    {
      /* todo: better logging */
      cctraceerr("\n>>> shader compile error\n%s\n>>> end",
        (char*)(messages_blob_d3d->lpVtbl->GetBufferPointer(messages_blob_d3d)));

      goto leave;
    }

    if(messages_blob_d3d != NULL)
    {
      cctraceerr("\n>>> shader compile warning\n%s\n>>> end",
        (char*)(messages_blob_d3d->lpVtbl->GetBufferPointer(messages_blob_d3d)));
    }

    config->hlsl.bytecode.memory=bytecode_blob_d3d->lpVtbl->GetBufferPointer(bytecode_blob_d3d);
    config->hlsl.bytecode.length=bytecode_blob_d3d->lpVtbl->   GetBufferSize(bytecode_blob_d3d);
  }

  ID3D11DeviceChild *shader_d3d = NULL;

  if(config->type == RX_OBJECT_TYPE_kVERTEX_SHADER)
  { if(FAILED(ID3D11Device_CreateVertexShader(rx.d3d11.dev,
        config->hlsl.bytecode.memory,
        config->hlsl.bytecode.length,NULL,(ID3D11VertexShader**)&shader_d3d))) goto leave;
  } else
  if(config->type == RX_OBJECT_TYPE_kPIXEL_SHADER)
  { if(FAILED(ID3D11Device_CreatePixelShader(rx.d3d11.dev,
        config->hlsl.bytecode.memory,
        config->hlsl.bytecode.length,NULL,(ID3D11PixelShader**)&shader_d3d))) goto leave;
  } else
  if(config->type == RX_OBJECT_TYPE_kCOMPUTE_SHADER)
  { if(FAILED(ID3D11Device_CreateComputeShader(rx.d3d11.dev,
        config->hlsl.bytecode.memory,
        config->hlsl.bytecode.length,NULL,(ID3D11ComputeShader**)&shader_d3d))) goto leave;
  } else
  {
    ccbreak();
  }

  if(config->d3d11.layout == NULL)
  {
    if(config->d3d11.layout_length == 0)
    {
      /* ensure signature blob is present otherwise early out */
      ID3DBlob *sig_blob_d3d = NULL;
      if(FAILED(
          D3DGetBlobPart(
            config->hlsl.bytecode.memory,
            config->hlsl.bytecode.length,
            D3D_BLOB_INPUT_SIGNATURE_BLOB,0,&sig_blob_d3d))) goto leave;

      ID3D11ShaderReflection *reflect_d3d = NULL;
      if(FAILED(
          D3DReflect(
            config->hlsl.bytecode.memory,
            config->hlsl.bytecode.length,
              &IID_ID3D11ShaderReflection,(void**)&reflect_d3d))) goto leave;

      D3D11_SHADER_DESC shader_info_d3d;
      reflect_d3d->lpVtbl->GetDesc(reflect_d3d,&shader_info_d3d);

      config->d3d11.layout_length = shader_info_d3d.InputParameters;

      int index;
      for( index = 0;
           index <  config->d3d11.layout_length;
           index += 1)
      {
        D3D11_SIGNATURE_PARAMETER_DESC param_info_d3d;
        reflect_d3d->lpVtbl->GetInputParameterDesc(reflect_d3d,index,&param_info_d3d);

        DXGI_FORMAT Format = 0;
        /* This is something that we have to revisit, for instance, say I wanted to
         support RGBA color as 4 chars instead of four floats for greater memory efficiency,
         there's not a straight forward way to know the mapping of the vertex layout we're
         using and map it to the layout of the input signature
         - XXX - the one called rj */
        if(param_info_d3d.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
        {
          switch(param_info_d3d.Mask)
          { case 0b1000:
            case 0b0100:
            case 0b0010:
            case 0b0001: Format=DXGI_FORMAT_R32_FLOAT;          break;
            case 0b1100:
            case 0b0011: Format=DXGI_FORMAT_R32G32_FLOAT;       break;
            case 0b0111: Format=0;                              break;
            case 0b1111: Format=DXGI_FORMAT_R32G32B32A32_FLOAT; break;
            default:
              ccassert(!"not implemented");
          }
        } else
          ccassert(!"not implemented");

        D3D11_INPUT_ELEMENT_DESC *elem_config_d3d=&config->d3d11.layout_config[index];
        elem_config_d3d->              Format=Format;
        elem_config_d3d->        SemanticName=param_info_d3d.SemanticName;
        elem_config_d3d->       SemanticIndex=param_info_d3d.SemanticIndex;
        elem_config_d3d->   AlignedByteOffset=D3D11_APPEND_ALIGNED_ELEMENT;
        elem_config_d3d->      InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA;
        elem_config_d3d->          InputSlot =0;
        elem_config_d3d->InstanceDataStepRate=0;
      }
    }

    if(FAILED(
        ID3D11Device_CreateInputLayout(
          rx.d3d11.dev,
              config->d3d11.layout_config,
              config->d3d11.layout_length,
                config->hlsl.bytecode.memory,
                config->hlsl.bytecode.length,  &config->d3d11.layout))) goto leave;
  }

  shader->d3d11.layout  = config->d3d11.layout;
  shader->d3d11.unknown = shader_d3d;

leave:

  if(bytecode_blob_d3d != NULL)
  {
    bytecode_blob_d3d->lpVtbl->Release(bytecode_blob_d3d);
  }

  if(messages_blob_d3d != NULL)
  {
    messages_blob_d3d->lpVtbl->Release(messages_blob_d3d);
  }

  ccfree(memory_to_close);
  ccclosefile(handle_to_close);
}

rxshader_t
rxshader_create(int type, size_t hlsl_length, void *hlsl_memory)
{
  shader_config_t config;
  ZeroMemory(&config,sizeof(config));

  config.type                 = type;
  config.hlsl.bytecode.length = hlsl_length;
  config.hlsl.bytecode.memory = hlsl_memory;

  rxshader_t shader;
  rxshader_init(&shader,&config);

  return shader;
}


rxshader_t
rxshader_load(
  int type, char const *entry, char const *fpath)
{
  shader_config_t config;
  ZeroMemory(&config,sizeof(config));

  config.type              = type;
  config.hlsl.source.fpath = fpath;
  config.hlsl.source.entry = entry;
  config.hlsl.source.label = fpath;

  rxshader_t shader;
  rxshader_init(&shader,&config);

  return shader;
}


