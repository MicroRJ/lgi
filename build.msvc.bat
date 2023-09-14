@IF "%INCLUDE%"=="" (
	@CALL vcvars64
)
@SETLOCAL
@REM /Qstrip_reflect /Qstrip_priv
@SET myOptions=/Qstrip_debug rx.hlsl
@SET compilerOptions=/O3 /WX
@SET compilerOptions=/Od
@CALL fxc /nologo /T vs_5_0 /E MainVS %compilerOptions% /Fh rxvs.hlsl /Vn rx_vs_shader_bytecode %myOptions%
@CALL fxc /nologo /T ps_5_0 /E MainPS %compilerOptions% /Fh rxps.hlsl /Vn rx_ps_shader_bytecode %myOptions%

@CALL fxc /nologo /T vs_5_0 /E MainVS_SDF       %compilerOptions% /Fh rxsdf.vs.hlsl     /Vn rx_vs_sdf_sb     %myOptions%
@CALL fxc /nologo /T ps_5_0 /E MainPS_CircleSDF %compilerOptions% /Fh rxsdf_cir.ps.hlsl /Vn rx_ps_sdf_cir_sb %myOptions%
@CALL fxc /nologo /T ps_5_0 /E MainPS_BoxSDF    %compilerOptions% /Fh rxsdf_box.ps.hlsl /Vn rx_ps_sdf_box_sb %myOptions%

@CALL fxc /nologo /T ps_5_0 /E MainPS_Text  %compilerOptions% /Fh rxtxt.ps.hlsl /Vn rx_ps_txt_sb %myOptions%
@CALL fxc /nologo /T ps_5_0 /E MainPS_TextSDF  %compilerOptions% /Fh rxtxt_sdf.ps.hlsl /Vn rx_ps_txt_sdf_sb %myOptions%


