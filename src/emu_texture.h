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

#if 0
enum
{ EMU_HOST_READ_BIT         = 1 << 0,
  EMU_HOST_WRITE_BIT        = 1 << 1,
  EMU_GPU_READ_BIT          = 1 << 2,
  EMU_GPU_WRITE_BIT         = 1 << 3,

  EMU_CPU_READ_BIT          = EMU_HOST_READ_BIT,
  EMU_CPU_WRITE_BIT         = EMU_HOST_WRITE_BIT,

  EMU_COLOR_ATTACHMENT_BIT  = 1 << 4,
  EMU_DEPTH_ATTACHMENT_BIT  = 1 << 5,

  EMU_USAGE_DEFAULT_BIT     = EMU_GPU_READ_BIT|EMU_GPU_WRITE_BIT,
  EMU_USAGE_DYNAMIC_BIT     = EMU_CPU_WRITE_BIT|EMU_GPU_READ_BIT,
  EMU_USAGE_IMMUTABLE_BIT   = EMU_GPU_READ_BIT,
  EMU_USAGE_STAGING_BIT     = EMU_GPU_WRITE_BIT|EMU_CPU_READ_BIT,
};
#endif

