@CALL fxc /nologo /T ps_5_0 /E MainPS /O3 /WX /Fh rxps.hlsl /Vn rx_ps_shader_bytecode /Qstrip_reflect /Qstrip_debug /Qstrip_priv rx.hlsl
@CALL fxc /nologo /T vs_5_0 /E MainVS /O3 /WX /Fh rxvs.hlsl /Vn rx_vs_shader_bytecode /Qstrip_reflect /Qstrip_debug /Qstrip_priv rx.hlsl
