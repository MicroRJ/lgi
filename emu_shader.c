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

Emu_shader_t
Emu_shader_create_bytecode(
  int flags, char const *label, size_t hlsl_length, void *hlsl_memory)
{
  Emu_shader_config_t config;
  ZeroMemory(&config,sizeof(config));

  config.flags = flags;
  config.label = label;
  config.source.bytecode.length = hlsl_length;
  config.source.bytecode.memory = hlsl_memory;

  Emu_shader_t shader;
  Emu_shader_init(&shader,&config);

  return shader;
}

Emu_shader_t
Emu_shader_load(
  int flags, char const *entry, char const *fpath)
{
  Emu_shader_config_t config;
  ZeroMemory(&config,sizeof(config));
  config.flags = flags;
  config.label = ccfnames(fpath);

  ccu32_t length = 0;
  void *handle = ccopenfile(fpath,"r");
  void *memory = ccpullfile(handle,0,&length);

  config.source.compile.memory = memory;
  config.source.compile.length = length;

  config.source.compile.entry = entry;

  Emu_shader_t shader;
  Emu_shader_init(&shader,&config);

  return shader;
}

void
Emu_shader_init(
  Emu_shader_t *shader, Emu_shader_config_t *config)
{
  ZeroMemory(shader,sizeof(shader));

  void *handle_to_close = NULL;
  void *memory_to_close = NULL;

  ID3DBlob *bytecode_blob_d3d = NULL;
  ID3DBlob *messages_blob_d3d = NULL;
  ID3DBlob *sig_blob_d3d = NULL;
  ID3D11ShaderReflection *reflect_d3d = NULL;
  ID3D11DeviceChild *shader_d3d = NULL;

  if(config->source.bytecode.memory == NULL)
  {
    if(config->source.compile.model == NULL)
    {
      /* todo: get this from the device */
      if(config->flags & EMU_kVERTEX_SHADER_BIT)
        config->source.compile.model = "vs_5_0";
      else
      if(config->flags & EMU_kPIXEL_SHADER_BIT)
        config->source.compile.model = "ps_5_0";
      else
      if(config->flags & EMU_kCOMPUTE_SHADER_BIT)
        config->source.compile.model = "cs_5_0";
    }
    if(config->source.compile.entry == NULL)
    {
      /* todo: get this from preferences */
      if(config->flags & EMU_kVERTEX_SHADER_BIT)
        config->source.compile.entry = "MainVS";
      else
      if(config->flags & EMU_kPIXEL_SHADER_BIT)
        config->source.compile.entry = "MainPS";
      else
      if(config->flags & EMU_kCOMPUTE_SHADER_BIT)
        config->source.compile.entry = "MainCS";
    }

    ccassert(config->source.compile.entry != 0);
    ccassert(config->source.compile.model != 0);
    ccassert(config->source.compile.memory != NULL);
    ccassert(config->source.compile.length != 0   );

    if(config->source.compile.debug_label == NULL) {
      config->source.compile.debug_label = config->label;
    }

    if(FAILED(
        D3DCompile(
          config->source.compile.memory, config->source.compile.length,
          config->source.compile.debug_label,0,0,
          config->source.compile.entry, config->source.compile.model,
          /* remove 'RX_SHADER_COMPILATION_FLAGS', should get from the config instead #todo */
          RX_SHADER_COMPILATION_FLAGS, 0, &bytecode_blob_d3d,&messages_blob_d3d)))
    {
      /* todo: better logging */
      cctraceerr("\n>>> shader compilation error\n%s\n>>> end",
        (char*)(messages_blob_d3d->lpVtbl->GetBufferPointer(messages_blob_d3d)));

      goto leave;
    }

    if(messages_blob_d3d != NULL) {
      cctraceerr("\n>>> shader compilation warning\n%s\n>>> end",
        (char*)(messages_blob_d3d->lpVtbl->GetBufferPointer(messages_blob_d3d)));
    }

    config->source.bytecode.memory=bytecode_blob_d3d->lpVtbl->GetBufferPointer(bytecode_blob_d3d);
    config->source.bytecode.length=bytecode_blob_d3d->lpVtbl->   GetBufferSize(bytecode_blob_d3d);
  }

  if(config->flags & EMU_kPIXEL_SHADER_BIT) {
    if(config->force_create_layout != TRUE) {
      cctracelog("'config.force_create_layout = FALSE': skipping input layout creation for pixel shaders");
      goto skip_create_input_layout;
    }
  }

  if(config->donot_create_layout != FALSE) {
    cctracelog("'config.donot_create_layout = TRUE': skipping input layout creation");
    goto skip_create_input_layout;
  }

  if(config->layout.d3d11.layout == NULL)
  {
    if(config->layout.attr_count == 0)
    {
      /* ensure signature blob is present otherwise early out, in my experience
        even with the /Qstrip_reflect /Qstrip_priv you still get the data you need,
        either way I think is good to let the user know that they could try
        and remove those flags in case it fails */
      if(FAILED(
          D3DGetBlobPart(
            config->source.bytecode.memory,
            config->source.bytecode.length,
            D3D_BLOB_INPUT_SIGNATURE_BLOB,0,&sig_blob_d3d)))
      {
        cctraceerr(
          "'microdev.rj@gmail.com': failed to acquire input signature blob, this likely means the reflection "
          "data isn't present in the bytecode, try removing the /Qstrip_reflect /Qstrip_priv compilation flags "
          "if you have them though this isn't expected to solve the issue");
        goto leave;
      }

      if(FAILED(
          D3DReflect(
            config->source.bytecode.memory,
            config->source.bytecode.length,
              &IID_ID3D11ShaderReflection,(void**)&reflect_d3d)))
      {
        cctraceerr(
          "'microdev.rj@gmail.com': failed to acquire reflection interface, this likely means the reflection "
          "data isn't present in the bytecode, try removing the /Qstrip_reflect /Qstrip_priv compilation flags "
          "if you have them though this isn't expected to solve the issue");
        goto leave;
      }

      D3D11_SHADER_DESC shader_info_d3d;
      reflect_d3d->lpVtbl->GetDesc(reflect_d3d,&shader_info_d3d);

      config->layout.attr_count = shader_info_d3d.InputParameters;

      int index;
      for( index = 0; index < config->layout.attr_count; index += 1)
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

        D3D11_INPUT_ELEMENT_DESC *elem_config_d3d=&config->layout.attr_array[index];
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
        ID3D11Device_CreateInputLayout(rx.d3d11.dev,
          config->layout.attr_array,config->layout.attr_count,
            config->source.bytecode.memory,config->source.bytecode.length,
              &config->layout.d3d11.layout))) {
      cctraceerr("failed to create input layout");
      goto leave;
    }
  }

skip_create_input_layout:
  if(config->flags & EMU_kVERTEX_SHADER_BIT)
  { if(FAILED(
        ID3D11Device_CreateVertexShader(rx.d3d11.dev,
          config->source.bytecode.memory,config->source.bytecode.length,
            NULL,(ID3D11VertexShader**)(&shader_d3d)))) {
      cctraceerr("failed to create vertex shader");
      goto leave;
    }
  } else
  if(config->flags & EMU_kPIXEL_SHADER_BIT)
  { if(FAILED(
        ID3D11Device_CreatePixelShader(rx.d3d11.dev,
          config->source.bytecode.memory,config->source.bytecode.length,
            NULL,(ID3D11PixelShader**)(&shader_d3d)))) {
      cctraceerr("failed to create pixel shader");
      goto leave;
    }
  } else
  if(config->flags & EMU_kCOMPUTE_SHADER_BIT)
  { if(FAILED(
        ID3D11Device_CreateComputeShader(rx.d3d11.dev,
          config->source.bytecode.memory,config->source.bytecode.length,
            NULL,(ID3D11ComputeShader**)(&shader_d3d)))) {
      cctraceerr("failed to create compute shader");
      goto leave;
    }
  } else
  {
    ccbreak();
  }

leave:
  shader->label = config->label;
  shader->flags = config->flags;
  shader->d3d11.unknown = shader_d3d;
  shader->d3d11.layout  = config->layout.d3d11.layout;

  if(sig_blob_d3d != NULL) {
    sig_blob_d3d->lpVtbl->Release(sig_blob_d3d);
  }

  if(reflect_d3d != NULL) {
    reflect_d3d->lpVtbl->Release(reflect_d3d);
  }

  if(bytecode_blob_d3d != NULL) {
    bytecode_blob_d3d->lpVtbl->Release(bytecode_blob_d3d);
  }

  if(messages_blob_d3d != NULL) {
    messages_blob_d3d->lpVtbl->Release(messages_blob_d3d);
  }

  ccfree(memory_to_close);
  ccclosefile(handle_to_close);
}



