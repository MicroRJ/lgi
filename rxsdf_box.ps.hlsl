#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xy  
// POSITION                 1   xyzw        1     NONE   float   xyzw
// COLOR                    0   xyzw        2     NONE   float   xyzw
// TEXCOORD                 0   xyzw        3     NONE   float   x  w
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Target                0   xyzw        0   TARGET   float   xyzw
//
ps_5_0
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[5], immediateIndexed
dcl_input_ps_siv linear noperspective v0.xy, position
dcl_input_ps linear v1.xyzw
dcl_input_ps linear v2.xyzw
dcl_input_ps linear v3.xw
dcl_output o0.xyzw
dcl_temps 1
add r0.y, -v0.y, cb0[4].y
mov r0.x, v0.x
add r0.xy, r0.xyxx, -v1.xyxx
add r0.zw, v1.zzzw, -v3.xxxx
add r0.xy, -r0.zwzz, |r0.xyxx|
max r0.zw, r0.xxxy, l(0.000000, 0.000000, 0.000000, 0.000000)
max r0.x, r0.y, r0.x
min r0.x, r0.x, l(0.000000)
dp2 r0.y, r0.zwzz, r0.zwzz
sqrt r0.y, r0.y
add r0.x, r0.x, r0.y
add r0.x, r0.x, -v3.x
add r0.x, -r0.x, l(1.000000)
div r0.y, l(1.000000, 1.000000, 1.000000, 1.000000), v3.w
mul_sat r0.x, r0.y, r0.x
mad r0.y, r0.x, l(-2.000000), l(3.000000)
mul r0.x, r0.x, r0.x
mul r0.x, r0.x, r0.y
mul o0.w, r0.x, v2.w
mov o0.xyz, v2.xyzx
ret 
// Approximately 0 instruction slots used
#endif

const BYTE rx_ps_sdf_box_sb[] =
{
     68,  88,  66,  67, 232, 178, 
    194,  37, 158, 135, 183,  72, 
    229,  13, 212, 251, 235,   2, 
      8, 218,   1,   0,   0,   0, 
    188,   3,   0,   0,   3,   0, 
      0,   0,  44,   0,   0,   0, 
    192,   0,   0,   0, 244,   0, 
      0,   0,  73,  83,  71,  78, 
    140,   0,   0,   0,   4,   0, 
      0,   0,   8,   0,   0,   0, 
    104,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   3,   0,   0, 
    116,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,  15,   0,   0, 
    125,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,  15,  15,   0,   0, 
    131,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   3,   0, 
      0,   0,  15,   9,   0,   0, 
     83,  86,  95,  80,  79,  83, 
     73,  84,  73,  79,  78,   0, 
     80,  79,  83,  73,  84,  73, 
     79,  78,   0,  67,  79,  76, 
     79,  82,   0,  84,  69,  88, 
     67,  79,  79,  82,  68,   0, 
     79,  83,  71,  78,  44,   0, 
      0,   0,   1,   0,   0,   0, 
      8,   0,   0,   0,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  83,  86, 
     95,  84,  97, 114, 103, 101, 
    116,   0, 171, 171,  83,  72, 
     69,  88, 192,   2,   0,   0, 
     80,   0,   0,   0, 176,   0, 
      0,   0, 106,   8,   0,   1, 
     89,   0,   0,   4,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      5,   0,   0,   0, 100,  32, 
      0,   4,  50,  16,  16,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  98,  16,   0,   3, 
    242,  16,  16,   0,   1,   0, 
      0,   0,  98,  16,   0,   3, 
    242,  16,  16,   0,   2,   0, 
      0,   0,  98,  16,   0,   3, 
    146,  16,  16,   0,   3,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   0,   0, 
      0,   0, 104,   0,   0,   2, 
      1,   0,   0,   0,   0,   0, 
      0,   9,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,  16, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  26, 128, 
     32,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,  54,   0, 
      0,   5,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,  16, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,   8,  50,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   0,  16,   0,   0,   0, 
      0,   0,  70,  16,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   8, 
    194,   0,  16,   0,   0,   0, 
      0,   0, 166,  30,  16,   0, 
      1,   0,   0,   0,   6,  16, 
     16, 128,  65,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   9,  50,   0,  16,   0, 
      0,   0,   0,   0, 230,  10, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  70,   0, 
     16, 128, 129,   0,   0,   0, 
      0,   0,   0,   0,  52,   0, 
      0,  10, 194,   0,  16,   0, 
      0,   0,   0,   0,   6,   4, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  52,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     51,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   7,  34,   0,  16,   0, 
      0,   0,   0,   0, 230,  10, 
     16,   0,   0,   0,   0,   0, 
    230,  10,  16,   0,   0,   0, 
      0,   0,  75,   0,   0,   5, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   8, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  10,  16, 
     16, 128,  65,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   8,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     14,   0,   0,  10,  34,   0, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,  58,  16,  16,   0, 
      3,   0,   0,   0,  56,  32, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0, 192, 
      1,  64,   0,   0,   0,   0, 
     64,  64,  56,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7, 130,  32,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     58,  16,  16,   0,   2,   0, 
      0,   0,  54,   0,   0,   5, 
    114,  32,  16,   0,   0,   0, 
      0,   0,  70,  18,  16,   0, 
      2,   0,   0,   0,  62,   0, 
      0,   1
};
