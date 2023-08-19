@SETLOCAL
@SET myOptions=/Qstrip_reflect /Qstrip_debug /Qstrip_priv rx.hlsl
@CALL fxc /nologo /T vs_5_0 /E MainVS /O3 /WX /Fh rxvs.hlsl /Vn rx_vs_shader_bytecode %myOptions%
@CALL fxc /nologo /T ps_5_0 /E MainPS /O3 /WX /Fh rxps.hlsl /Vn rx_ps_shader_bytecode %myOptions%

@CALL fxc /nologo /T vs_5_0 /E MainVS_SDF       /O3 /WX /Fh rxsdf.vs.hlsl     /Vn rx_vs_sdf_sb     %myOptions%
@CALL fxc /nologo /T ps_5_0 /E MainPS_CircleSDF /O3 /WX /Fh rxsdf_cir.ps.hlsl /Vn rx_ps_sdf_cir_sb %myOptions%
@CALL fxc /nologo /T ps_5_0 /E MainPS_BoxSDF    /O3 /WX /Fh rxsdf_box.ps.hlsl /Vn rx_ps_sdf_box_sb %myOptions%

@CALL fxc /nologo /T ps_5_0 /E MainPS_Text  /O3 /WX /Fh rxtxt.ps.hlsl /Vn rx_ps_txt_sb %myOptions%


