/*
**
**                      -+- cc -+-
**
**                 Mundane Utilities API.
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
**                 github.com/MicroRJ/cc
**
**
*/
typedef struct ccfile_t ccfile_t;
typedef struct ccfile_t
{
  HANDLE handle;
} ccfile_t;

#ifdef _CC_DEPRECATED
ccfunc void *   ccopenfile  ( const char *, const char *);
ccfunc int      ccrealfile  ( void * );
ccfunc void     ccclosefile ( void * );
ccfunc void   * ccpullfile  ( void *, ccu32_t, ccu32_t *      );
ccfunc ccu32_t  ccpushfile  ( void *, ccu32_t, ccu32_t,void * );
ccfunc ccu32_t  ccfilesize  ( void * );
#endif

ccfunc void ccfileclose(ccfile_t);
ccfunc ccfile_t ccfileopen(int flags, const char *floppy);
ccfunc void *ccfilepull(ccfile_t, size_t *length);
ccfunc void *ccfilepush(ccfile_t, size_t length, void *buffer);
ccfunc void ccfileclose(ccfile_t);