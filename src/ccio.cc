/*
  Copyright (c) 2022 Dayan Rodriguez

  https://github.com/MicroRJ/cc */
#ifdef _WIN32

/* XXX */
ccfunc const char *
ccfnames(const char *name)
{
	const char *result=name;

  for(result=name;*name;++name)
    if((*name=='/')||(*name=='\\'))
      result=name+1;

  return result;
}

ccfunc const char *
ccfileext(const char *name)
{
	const char *result=name;

  for(result=name;*name;++name)
    if((*name=='.'))
      result=name+1;

  return result;
}

// Note: also check for the file attributes
ccfunc int
ccrealfile(void *file)
{
	return (HANDLE)file!=INVALID_HANDLE_VALUE;
}

ccfunc ccu32_t
ccfilesize(void *file)
{
	return GetFileSize((HANDLE)file,0x00);
}

ccfunc int
ccflushfile(void *file)
{
	int Result = FlushFileBuffers((HANDLE)file);
	if(Result == FALSE)
		cctracewar("flush failed");

	return Result;
}

ccfunc void
ccclosefile(void *file)
{
	CloseHandle((HANDLE)file);
}

/* XXX proper file disposition */
ccfunc void *
ccopenfile(const char *name, const char *format)
{
  DWORD flags=0;
  if(format)
  	for(;*format;format++)
    	if(*format=='r') flags|=GENERIC_READ; else
    	if(*format=='w') flags|=GENERIC_WRITE;
	else
		return ccnull;

  ccassert(flags!=0);

  void *file;

  file = CreateFileA(name,flags,
  	FILE_SHARE_WRITE|FILE_SHARE_READ,0x00,
  		OPEN_ALWAYS,0x00,0x00);

  if(!ccrealfile(file))
  {
    cctracewar("'%s': invalid file",name);

    file = ccnull;
  }

  return file;
}

ccfunc const char *
ccfnameh__(void *file)
{
  void *buffer = ccstatic_alloc(MAX_PATH+4,TRUE);
  int  *length = (int *)((int*)buffer);
  char *string = (char*)(length+1);
  *length = GetFinalPathNameByHandle(file,string,MAX_PATH,VOLUME_NAME_NT);
  return string;
}

ccfunc const char *
ccfnameh(void *file)
{
	return ccfnames(ccfnameh__(file));
}

ccfunc void *
ccpullfile(void *file, ccu32_t offset, ccu32_t *lpio_length)
{
  if(!ccrealfile(file))
  {
    cctraceerr("invalid file",0);
    return 0;
  }

  ccdebuglog("'%s': %i,%i",
  	ccfnameh(file),offset,lpio_length ? *lpio_length : 0);

  ccu32_t length=ccfilesize(file);

  void *result=0;

  if(length != 0)
  {
    if(lpio_length)
    {
      if(*lpio_length != 0)
      {
        if(*lpio_length + offset > length)
          length = length-offset;
        else
          length = *lpio_length;
      }
    } else
    {
      lpio_length = &length;
    }

    result=ccmalloc(length);

    if(!ReadFile((HANDLE)file,result,length,(LPDWORD)lpio_length,0x00))
    {
      cctraceerr("'%s': could not be read",
        ccfnameh(file));

      ccfree(result);

      result = 0;
    }
  }
  return result;
}

// ccfunc ccu32_t
// ccpushfile(void *file, ccu32_t offset, ccu32_t length, void *file_data)
// {
//   if(!ccrealfile(file))
//   {
//     cctraceerr("invalid file",0);
//     return 0;
//   }

//   ccdebuglog("'%s': %i,%i",
//   	ccfnameh(file),offset,length);

//   ccu32_t file_size=ccfilesize(file);

//   if(file_size<offset) return 0;

//   DWORD Wrote = 0;
//   if(!WriteFile((HANDLE)file,file_data,length,&Wrote,0x00))
//   {
//     cctraceerr("'%s': write failed", ccfnameh(file));
//     return 0;
//   }

//   ccassert(ccflushfile(file));

//   return length;
// }

#endif
