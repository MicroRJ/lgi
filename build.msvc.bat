@IF "%INCLUDE%"=="" (
	@CALL vcvars64
)
@SETLOCAL
@REM /Qstrip_reflect /Qstrip_priv /Qstrip_debug
@SET mySources=..\..\rx.hlsl
@REM @SET compilerOptions=/O3
@SET compilerOptions=/WX /Od
@SET myOptions=/nologo /Zi /Zss
@PUSHD "src\hlsl"
@REM // /Vn
@CALL fxc %myOptions% /T vs_5_0 /E MainVS %compilerOptions% /Fh rxvs.h  %mySources%
@CALL fxc %myOptions% /T ps_5_0 /E MainPS %compilerOptions% /Fh rxps.h  %mySources%

@CALL fxc %myOptions% /T vs_5_0 /E MainVS_SDF       %compilerOptions% /Fh rxsdf.vs.h          %mySources%
@CALL fxc %myOptions% /T ps_5_0 /E MainPS_CircleSDF %compilerOptions% /Fh rxsdf_cir.ps.h  %mySources%
@CALL fxc %myOptions% /T ps_5_0 /E MainPS_BoxSDF    %compilerOptions% /Fh rxsdf_box.ps.h  %mySources%

@CALL fxc %myOptions% /T ps_5_0 /E MainPS_Text  %compilerOptions% /Fh rxtxt.ps.h  %mySources%
@CALL fxc %myOptions% /T ps_5_0 /E MainPS_TextSDF  %compilerOptions% /Fh rxtxt_sdf.ps.h  %mySources%
@POPD

