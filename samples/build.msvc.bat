@IF "%INCLUDE%"==""  (
   @CALL vcvars64
)
@SETLOCAL
@SET myCompilerOptions=/options:strict /nologo /TC /Z7 /WX /W4
@SET myInclude=/I. /I.. /I../..
@SET myDebugFlags=/Od /D_DEBUG /MTd
@SET myReleaseFlags=/O2
@SET myCommon=%myCompilerOptions% %myInclude%
@SET myLinkerOptions=/INCREMENTAL:NO
@SET myGenFlags=%myDebugFlags%
@PUSHD build
@CALL cl %myCommon% %myDebugFlags% ../3d.c /link %myLinkerOptions% /SUBSYSTEM:CONSOLE
@POPD
@ENDLOCAL