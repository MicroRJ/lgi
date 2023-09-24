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
#ifndef _CC_C
#define _CC_C 1

/* todo: */
#ifdef _CC_LONG_INT
typedef signed   long int cci32_t;
typedef unsigned long int ccu32_t;
#else
typedef signed   int cci32_t;
typedef unsigned int ccu32_t;
#endif
#ifdef _MSC_VER
typedef signed   __int64 cci64_t;
typedef unsigned __int64 ccu64_t;
#else
typedef signed   long long int cci64_t;
typedef unsigned long long int ccu64_t;
#endif

// todo: remove
#define CCWITHIN(x,l,r) (((x)>=(l))&&((x)<=(r)))

#ifdef __cplusplus
extern "C" {
#endif

/* static buffer for formatting, a programming delicacy */
#ifndef _CC_NO_STATIC_MEMORY
#ifndef    _CC_STATIC_MEMORY
#define    _CC_STATIC_MEMORY 0x1000
# endif//_CC_STATIC_MEMORY
# endif//_CC_NO_STATIC_MEMORY


#define _CCGUID __COUNTER__
#define _CCFILE __FILE__
#define _CCLINE __LINE__
#define _CCFUNC __func__

#define ccfunc         static
#define ccthread_local __declspec(thread)
#define ccglobal       static

// todo:
# ifdef __forceinline
#define ccinle __forceinline
#  else
#define ccinle inline
# endif//__forceinline

#ifndef _CCASSERT
#define _CCASSERT(x) typedef char __CCASSERT__[((x)?1:-1)]
# endif//_CCASSERT

/* some controversial things going on here */
#ifndef cccast
#define cccast(T,mem) ((T)(mem))
# endif//cccast

/* c array literal length */
#ifndef  ccCarrlenL
#define  ccCarrlenL(x) cccast(ccu32_t,sizeof(x)/sizeof(x[0]))
# endif//ccCarrlenL

/* c string literal length */
# ifndef ccCstrlenL
# define ccCstrlenL(x) cccast(ccu32_t,ccCarrlenL(x)-1)
#  endif//ccCstrlenL

/* c string length */
#ifndef  ccCstrlenS
#define  ccCstrlenS(x) cccast(ccu32_t,strlen(x))
# endif//ccCstrlenS

	typedef enum {
		CC_ERROR_kNONE               = 0,
		CC_ERROR_kNOT_IN_TABLE,
		CC_ERROR_kALREADY_IN_TABLE,
	} ccerror_k;

	typedef struct {
		int           guid;
		const char   *file;
		int           line;
		const char   *func;
	} cccaller_t;

	typedef struct ccentry_t {
		cccaller_t  caller;
		struct ccentry_t * nex;
		cci32_t     len;
		char      * key;
		cci64_t     val;
	} ccentry_t;

	typedef struct {
		unsigned        rem_add: 1;
		unsigned        rem_rze: 1;
		ccentry_t     * entries;
		cci64_t         sze_max;
		cci64_t         sze_min;
	} ccdlb_t;

/* todo: please find a way to remove this */
	struct {
		ccerror_k  error;
		ccentry_t *entry;
#ifdef _CC_STATIC_MEMORY
		char static_memory[_CC_STATIC_MEMORY];
		int  static_cursor;
#endif
	} ccglobal ccthread_local cc;

// merge: ++ re-added ccerrnit()
#define ccerrset(x) ((cc.error=x),0)
#define ccerrnon()  (cc.error==CC_ERROR_kNONE)
#define ccerrnit()  (cc.error==CC_ERROR_kNOT_IN_TABLE)
#define ccerrait()  (cc.error==CC_ERROR_kALREADY_IN_TABLE)
#define ccerrstr()  (ccerror_ks[cc.error])
#define cckeyget()  ((cc.entry)?(cc.entry->key):ccnull)

#define cctrue  cccast(cci32_t,1)
#define ccfalse cccast(cci32_t,0)

	ccfunc ccinle int   ccformatvex (char *,int,const char *,va_list);
	ccfunc ccinle char *ccformatv   (const char *, va_list);
	ccfunc        int   ccformatex  (char *, int, const char *, ...);
	ccfunc        char *ccformat    (const char *, ...);


// todo: hmm...
#define ccnil  0
#define ccnull 0

#ifndef ccbreak
# define ccbreak __debugbreak
#endif

// todo: enhance these assertions? ...
#ifndef ccassert
# ifdef _DEBUG
#  define ccassert(is_true,...) do{ if(!(is_true)) { cctraceerr("assertion triggered",0); ccbreak(); } } while(0)
# else
#  define ccassert(is_true,...) ccnull
# endif
#endif

// Note:
	ccfunc ccinle cccaller_t cccaller(int guid, const char *file, int line, const char *func);
#define cccall() (cccaller(__COUNTER__,_CCFILE,_CCLINE,_CCFUNC))

	void *
	ccmalloc(size_t length)
	{
		return malloc(length);
	}

	void *
	ccrealloc(void *memory, size_t length)
	{
		return realloc(memory,length);
	}

	void
	ccfree(void const *memory)
	{
		free((void*)memory);
	}

#define rlMemory_allocType(T) cccast(T*,ccmalloc(sizeof(T)))


// note: add an item to a dynamic length buffer
	ccfunc cci64_t ccdlbadd_(void **, cci32_t item_size, cci64_t reserve_count, cci64_t commit_count);
// note: removes a number of items from the end of the dynamic length buffer
	ccfunc cci64_t ccdlbpop_(void **, cci32_t item_size, cci64_t removal_count);
// note: finds an item by key in a dynamic length buffer, if the item is not found,
// the last entry is returned, UNLESS create_always is specified, in which
// case a new entry is returned, UNLESS there are no entries in the bucket, in which
// case the first entry is returned.
	ccfunc int     cctblhsh_(cccaller_t, void **, cci32_t item_size, int len, char *key, int create_always);

// note: finds an item by key in a dynamic length buffer, if the item is not found
// the global error is set to 'CC_ERROR_kNOT_IN_TABLE', ('not in table') use 'ccerrnon()' or 'ccerrnit()' respectively.
	ccfunc cci64_t cctblgeti_(cccaller_t, void **, cci32_t item_size, cci32_t key_length, char *key);

// note: puts an item by key if not already present in a dynamic length buffer, if the item is present
// the global error is set to 'CC_ERROR_kALREADY_IN_TABLE' ('already in table') use 'ccerrnon()' or 'ccerrait()' respectively.
	ccfunc cci64_t cctblputi_(cccaller_t, void **, cci32_t item_size, cci32_t key_length, char *key);

// note: puts an item by key overwriting if already present or creating a new one otherwise
// in a dynamic length buffer, this function does not report errors.
	ccfunc cci64_t cctblseti_(cccaller_t, void **, cci32_t item_size, cci32_t key_length, char *key);

// note: commits/reserves memory in a string taking into account the null terminator that is
// always present for convenient compatibility.
	ccfunc cci64_t ccstraddi_(char **, cci64_t reserve_size, cci64_t commit_size, const char *);

// note: concatenates a variadic format specifier to a string.
	ccfunc cci64_t ccstrcatiV_(char **, const char *, va_list);

// note: concatenates a format specifier to a string.
	ccfunc cci64_t ccstrcatiF_(char **, const char *, ...);

// note: casts a memory address to a dynamic length buffer
#define ccdlb_(ccm) (cccast(ccdlb_t*,ccm)-1)
#define ccdlb(ccm) ((ccm)?ccdlb_(ccm):(ccnull))

// note: deletes a dynamic length buffer
	ccfunc void ccdlbdel_(ccdlb_t *dlb);
// note: deletes a dynamic length buffer, expects the memory address
#define ccdlbdel(ccm) ccdlbdel_(ccdlb(ccm))
/* Array nomenclature, it is \important that you use this macro \specifically for cc-arrays,
 this is for multiple reasons, but the most important one is that this macro will use the right
 allocator always, you could get away with a more general cc-free but cc-free assumes the default
 allocator so chances are sooner or later you will run into a bug, there's a slight possibility
 that in the future cc-free could take into account the allocator if all allocations have metadata
 within them but it could be a long while - rj */
#define rlArray_delete ccdlbdel

// note: queries the maximum length (item count) of the dynamic length buffer, the bytes
// that have been committed + reserved.
#define ccdlbmax_(ccm) (ccdlb_(ccm)->sze_max)
#define ccdlbmax(ccm) ((ccm)?ccdlbmax_(ccm):(0))
// note: queries the minimum length (item count) of the dynamic length buffer, the bytes
// that have been committed.
#define ccdlbmin_(ccm) (ccdlb_(ccm)->sze_min)
#define ccdlbmin(ccm) ((ccm)?ccdlbmin_(ccm):(0))
//
// note: the following functions are array based nomenclature
//
#define ccarrmax ccdlbmax
#define ccarrmin ccdlbmin
// note: 32-bit length of the array
#define rlArray_lengthOf(arr) cccast(ccu32_t,ccarrmin(arr))
// note: address of one past the last reserved item in the array
#define ccarrend_max(arr) ((arr)+ccarrmax(arr))
// note: address of one past the last committed item in the array
#define ccarrend_min(arr) ((arr)+ccarrmin(arr))
// note : default
#define ccarrend ccarrend_min
// note: sets to zero the entire array, including the reserved space also
#define ccarrzro_max(arr) memset(arr,0,ccarrmax(arr)*sizeof(*arr))
// note: sets to zero the entire array, including the committed space only
#define ccarrzro_min(arr) memset(arr,0,ccarrmin(arr)*sizeof(*arr))
// note : default
#define ccarrzro ccarrzro_min
// note: reserves a number of items from the array, returns the previous commit
// count, this function does not affect the commit count, you must commit what
// you reserve to prevent overwriting.
#define ccarrresi(arr,num) ccdlbadd_((void**)(&arr),sizeof(*(arr)),num,0)
// note: same as 'ccarrresi' but it  returns the address instead
#define ccarrres(arr,num) ((arr)+ccarrresi(arr,num))
// note: reserves and commits a number of items from the array, returns the previous
// commit count
#define ccarraddi(arr,num) ccdlbadd_((void**)(&arr),sizeof(*(arr)),num,num)
// note: same as 'ccarraddi' but it  returns the address instead
#define rlArray_add(arr,num) ((arr)+ccarraddi(arr,num))
/* You have to be careful with this because if you add items to the array it will
 use invalid memory - XXX - the one called rj */
#define ccarrfor_ex(arr,idx,len) for(idx=arr;idx<arr+len;++idx)
// note: iterates over the committed contents of an array by pointer
#define ccarrfor(arr,itr) for(itr=arr;itr<ccarrend(arr);++itr)
// note: iterates over the committed contents of an array by index
#define ccarrfori(arr,idx) for(idx=0;idx<rlArray_lengthOf(arr);++idx)
// note: "removes" a number of items in the array from the back, returns the previous
// commit count
#define ccarrpopi(arr,num) ccdlbpop_((void**)(&arr),sizeof(*(arr)),num)
// note: same as 'ccarrpopi' but it returns the address instead
#define ccarrpop(arr,num) ((arr)+ccarrpopi(arr,num))
// todo: possibly remove or rename at-least
#define ccarrfix(arr) ((arr)?ccdlb_(arr)->rem_rze=cctrue:(ccnull))
// Note: table
#define cclithsh(lit) ccCstrlenL(lit),cccast(char*,lit)
#define ccntshsh(nts) ccCstrlenS(nts),cccast(char*,nts)
#define ccinthsh(ptr) -cccast(cci32_t,sizeof(ptr)),cccast(char*,ptr)

// Note: these use the global error code _ccerror_k
// ccfunc cci64_t cctblgeti(void **, cci32_t, cci32_t, char *);
// ccfunc cci64_t cctblputi(void **, cci32_t, cci32_t, char *);
// ccfunc cci64_t cctblseti(void **, cci32_t, cci32_t, char *);
// ccfunc cci64_t cctblremi(void **, cci32_t, cci32_t, char *);

#define cctblgeti(...) cctblgeti_(cccall(),__VA_ARGS__)
#define cctblputi(...) cctblputi_(cccall(),__VA_ARGS__)
#define cctblseti(...) cctblseti_(cccall(),__VA_ARGS__)
#define cctblremi(...) cctblremi_(cccall(),__VA_ARGS__)

// -- Todo: these are to be reworked?
#define cctblgetL(tbl,lit) ((tbl)+cctblgeti((void**)(&tbl),sizeof(*(tbl)),cclithsh(lit)))
#define cctblgetS(tbl,nts) ((tbl)+cctblgeti((void**)(&tbl),sizeof(*(tbl)),ccntshsh(nts)))
#define cctblgetP(tbl,ptr) ((tbl)+cctblgeti((void**)(&tbl),sizeof(*(tbl)),ccinthsh(ptr)))

#define cctblsetN(tbl,len,key) ((tbl)+cctblseti((void**)(&tbl),sizeof(*tbl),len,cccast(char*,key)))
#define cctblsetL(tbl,lit) ((tbl)+cctblseti((void**)(&tbl),sizeof(*tbl),cclithsh(lit)))
#define cctblsetS(tbl,nts) ((tbl)+cctblseti((void**)(&tbl),sizeof(*tbl),ccntshsh(nts)))
#define cctblsetP(tbl,ptr) ((tbl)+cctblseti((void**)(&tbl),sizeof(*tbl),ccinthsh(ptr)))

#define cctblputN(tbl,len,key) ((tbl)+cctblputi((void**)(&tbl),sizeof(*tbl),len,cccast(char*,key)))
#define cctblputL(tbl,lit) ((tbl)+cctblputi((void**)(&tbl),sizeof(*tbl),cclithsh(lit)))
#define cctblputS(tbl,nts) ((tbl)+cctblputi((void**)(&tbl),sizeof(*tbl),ccntshsh(nts)))
#define cctblputP(tbl,ptr) ((tbl)+cctblputi((void**)(&tbl),sizeof(*tbl),ccinthsh(ptr)))

#define cctblremN(tbl,len,key) ((tbl)+cctblremi((void**)(&tbl),sizeof(*tbl),len,cccast(char*,key)))
#define cctblremL(tbl,lit) ((tbl)+cctblremi((void**)(&tbl),sizeof(*tbl),cclithsh(lit)))
#define cctblremS(tbl,nts) ((tbl)+cctblremi((void**)(&tbl),sizeof(*tbl),ccntshsh(nts)))
#define cctblremP(tbl,ptr) ((tbl)+cctblremi((void**)(&tbl),sizeof(*tbl),ccinthsh(ptr)))

// Note: string builder
#define ccstrdel rlArray_delete
#define ccstrmax ccarrmax
#define ccstrmin ccarrmin
#define ccstrlen rlArray_lengthOf

#define ccstraddN(arr,res,com) ((arr)+ccdlbadd_((void**)(&arr),sizeof(*(arr)),res,com))

#define ccstrcatN(arr,len,str) ((arr)+ccstraddi_(&(arr),len+1,len+0,str))
#define ccstrcatL(arr,str) ccstrcatN(arr,ccCstrlenL(str),str)
#define ccstrcatS(arr,str) ccstrcatN(arr,ccCstrlenS(str),str)

#define ccstrputN(arr,len,str) ((arr)+ccstraddi_(&(arr),len+1,len+1,str))
#define ccstrputL(arr,str) ccstrputN(arr,ccCstrlenL(str),str)
#define ccstrputS(arr,str) ccstrputN(arr,ccCstrlenS(str),str)

#define ccstrcatF(arr,fmt,...) ccstrcatiF_(&arr,fmt,__VA_ARGS__)



// todo: remove
	ccfunc void cctrace_(cccaller_t caller, const char *label, const char *format, ...);
#define cctrace(label,fmt,...) (cctrace_(cccaller(__COUNTER__,_CCFILE,_CCLINE,_CCFUNC),label,fmt,__VA_ARGS__),0)
#define cctracelog(fmt,...) (cctrace("log",fmt,__VA_ARGS__),0)
#define cctracewar(fmt,...) (cctrace("war",fmt,__VA_ARGS__),0)
#define cctraceerr(fmt,...) (cctrace("err",fmt,__VA_ARGS__),0)
#define ccdebuglog(fmt,...) (cctrace("dbg",fmt,__VA_ARGS__),0)
#define ccannoylog(fmt,...) (cctrace("any",fmt,__VA_ARGS__),0)
#ifdef _CCEXECLOG
# define ccexeclog(fmt,...) (cctrace("exec",fmt,__VA_ARGS__),0)
#else
# define ccexeclog(fmt,...) (0)
#endif

// todo: remove
	ccfunc const char *ccfnames(const char *name);

#if 0

ccfunc ccinle void
ccout(const char *string)
{
#if defined(_WIN32) && defined(_CCDEBUG)
	OutputDebugStringA(string);
#endif
	printf(string);
}
#endif

	ccfunc ccinle char *
	ccstatic_alloc(int length, int zero)
	{
  /* XX we may just want to allocate this as a buffer */
		ccassert(length <= _CC_STATIC_MEMORY);

		if(cc.static_cursor+length>=_CC_STATIC_MEMORY)
		cc.static_cursor=0;

	char *memory=cc.static_memory+cc.static_cursor;
	cc.static_cursor += length;

	if(zero != 0)
	{
		memset(memory,0,length);
	}

	return memory;
}

/* todo: this needs an include guard */
// #include <../stb/stb_sprintf.h>
ccfunc ccinle int
ccformatvex(char *buf, int len, const char * fmt, va_list vli)
{
	return stbsp_vsnprintf(buf,len,fmt,vli);
}

ccfunc char *
ccformatv(const char *fmt, va_list vli) {

	int length=ccformatvex(0,0,fmt,vli);

	int static_length=ccCarrlenL(cc.static_memory);
	if(cc.static_cursor+length>=static_length) {
		cc.static_cursor=0;
	}

	char *result = cc.static_memory+cc.static_cursor;

	ccformatvex(result,static_length-cc.static_cursor,fmt,vli);

	cc.static_cursor += length+1;

	return result;
}

ccfunc ccinle int
ccformatex(char *buf, int len, const char * fmt, ...) {
	va_list vli;
	va_start(vli,fmt);
	int result=ccformatvex(buf,len,fmt,vli);
	va_end(vli);
	return result;
}

ccfunc ccinle char *
ccformat(const char * fmt, ...) {
	va_list vli;
	va_start(vli,fmt);
	char *result=ccformatv(fmt,vli);
	va_end(vli);
	return result;
}

#include <stdio.h>
ccfunc void
cctrace_(cccaller_t caller, const char *label, const char *format, ...) {
	va_list vli;
	va_start(vli,format);
	printf("%s: %s[%i] %s() %s\n",label,ccfnames(caller.file),caller.line,caller.func,ccformatv(format,vli));
	va_end(vli);
}

// Todo:
ccfunc void
ccdlbdel_(ccdlb_t *dlb)
{
	if(dlb)
	{
		if(dlb->entries)
		{
			ccentry_t *i,*f,*e;
			ccarrfor(dlb->entries,e)
			{ for(i=e->nex;i;)
				{ f=i;
					i=i->nex;

					ccfree(f);
				}
			}

			rlArray_delete(dlb->entries);
		}

		ccfree(dlb);
	}
}

ccfunc cci64_t
ccdlbpop_(void **vala, cci32_t isze, cci64_t rsze)
{
	(void) isze;
	if(*vala)
	{
		ccdlb_t *dynlenbuf=ccdlb_(*vala);

		ccassert(dynlenbuf->sze_min>=rsze);

		cci64_t sze_min=dynlenbuf->sze_min;

		dynlenbuf->sze_min=sze_min-rsze;

		return sze_min;
	}

	return 0;
}

ccfunc cci64_t
ccdlbadd_(void **lpp, cci32_t isze, cci64_t rsze, cci64_t csze)
{
	if(rsze <= 0 && csze <= 0)
	{
		return 0;
	}

	ccdlb_t * dlb;
	int     rem_rze, rem_add;
	cci64_t   sze_max, sze_min;

	if(*lpp)
	{ dlb=ccdlb_(*lpp);
		sze_max  = dlb->sze_max;
		sze_min  = dlb->sze_min;
		rem_rze  = dlb->rem_rze;
		rem_add  = dlb->rem_add;
		ccassert(!rem_add);
	} else
	{ dlb=ccnull;
		sze_max=0;
		sze_min=0;
		rem_rze=0;
		rem_add=0;
	}

	ccassert(csze<=rsze+sze_max-sze_min);

	if(sze_max<sze_min+rsze)
	{
		ccassert(!rem_rze);

		sze_max<<=1;
		if(sze_max<sze_min+rsze)
		sze_max=sze_min+rsze;

	dlb = cccast(ccdlb_t*,ccrealloc(dlb,sizeof(*dlb)+sze_max*isze));

	if(!*lpp)
	{ dlb->rem_rze=rem_rze;
		dlb->rem_add=rem_add;
		dlb->entries=ccnull;
	}

	*lpp=dlb+1;

	dlb->sze_max=sze_max;
}

dlb->sze_min=sze_min+csze;
return sze_min;
}

ccfunc ccentry_t *
ccallocentry(cccaller_t caller)
{
	ccentry_t *e = ccmalloc(sizeof(*e));
	e->nex = ccnull;
	e->len = ccnull;
	e->key = ccnull;
	e->val =    -1;

	e->caller = caller;

	return e;
}

// todo: does this use a good hashing function for strings?
// todo: remove caller when not in debug mode!
ccfunc int
cctblhsh_(cccaller_t caller, void **ccm, cci32_t ize, int len, char *key, int create_always)
{
	ccassert(len!=0);
	ccassert(key!=0);

	ccdlb_t *dlb;

	if(!*ccm)
	{
    // initialize the table if we want to add an entry
		if(create_always)
		{
			ccdlbadd_(ccm,ize,1,0);

			dlb=ccdlb_(*ccm);

      // todo: we have to handle resizing the hash-table
			rlArray_add(dlb->entries,1);
			ccarrzro(dlb->entries);
			ccarrfix(dlb->entries);
		}
    // there are no entries to add and the table isn't even initialized
		else return ccfalse;
	}
	else dlb=ccdlb_(*ccm);

	ccu64_t hsh=5381;
	ccu64_t idx,max;

	if(len>0)
	for(idx=0;idx<len;hsh=hsh<<5,hsh=hsh+key[idx++]);
	else
	hsh=cccast(ccu64_t,key);

max=ccarrmax(dlb->entries);

idx=hsh%max;

ccassert(idx==0);

cc.entry=dlb->entries+idx;

  // find an entry that is free
for(;cc.entry->key;cc.entry=cc.entry->nex)
{
    // return immediately if we've found a match
	if(cc.entry->len==len)
	{ if(len>0)
		{ if(!memcmp(cc.entry->key,key,len))
			return cctrue;
		} else
		{ if(cc.entry->key==key)
			return cctrue;
		}
	}

    // otherwise return the last entry in this bucket
	if(!cc.entry->nex)
	break;
}

if(create_always)
{
	if(cc.entry->key)
	cc.entry=cc.entry->nex=ccallocentry(caller);

cci64_t val=ccdlbadd_(ccm,ize,1,1);

ccassert(cc.entry->key==0);
ccassert(cc.entry->len==0);
    // ccassert(cc.entry->val>=0);

cc.entry->len=len;
cc.entry->key=key;
cc.entry->val=val;

    // todo!!: remove this
memset(cccast(char*,*ccm)+val*ize,0,ize);
}

return ccfalse;
}

ccfunc cci64_t
cctblremi_(cccaller_t caller, void **ccm, cci32_t ize, cci32_t len, char *key)
{ ccassert(ccm!=0);
	ccerrset(CC_ERROR_kNOT_IN_TABLE);
	if(cctblhsh_(caller,ccm,ize,len,key,ccfalse))
	{ ccerrset(CC_ERROR_kNONE);

		cc.entry->key = 0;
		cc.entry->len = 0;
	}
	return cc.entry? cc.entry->val :0;
}

// Todo: remove caller in release mode!
ccfunc cci64_t
cctblgeti_(cccaller_t caller, void **ccm, cci32_t ize, cci32_t len, char *key)
{ ccassert(ccm!=0);
	ccerrset(CC_ERROR_kNOT_IN_TABLE);
	if(cctblhsh_(caller,ccm,ize,len,key,ccfalse))
	ccerrset(CC_ERROR_kNONE);
return cc.entry? cc.entry->val :0;
}

// Todo: remove caller in release mode!
ccfunc cci64_t
cctblputi_(cccaller_t caller, void **ccm, cci32_t ize, cci32_t len, char *key)
{ ccassert(ccm!=0);
	ccerrset(CC_ERROR_kALREADY_IN_TABLE);
	if(cctblhsh_(caller,ccm,ize,len,key,cctrue))
	{ cctrace_(caller,"war","already in table, added by %s[%i]::%s()",
	ccfnames(cc.entry->caller.file),cc.entry->caller.line,cc.entry->caller.func);
} else
ccerrset(CC_ERROR_kNONE);
return cc.entry? cc.entry->val :0;
}

// Todo: remove caller in release mode!
ccfunc cci64_t
cctblseti_(cccaller_t caller, void **ccm, cci32_t ize, cci32_t len, char *key)
{ ccassert(ccm!=0);
	ccerrset(CC_ERROR_kNONE);
	cctblhsh_(caller,ccm,ize,len,key,cctrue);

	return cc.entry? cc.entry->val :0;
}

ccfunc cci64_t
ccstraddi_(char **ccm, cci64_t res, cci64_t com, const char *mem)
{
  // Note: assuming that you'll reserve at-least one more byte for the null terminator ...
	ccassert(res!=0);
	ccassert(com!=0);

	cci64_t result=ccdlbadd_(cccast(void**,ccm),sizeof(char),res,com);

	char *cur=*ccm+result;

	memcpy(cur,mem,res-1);
	cur[res-1]=0;

	return result;
}

// Todo: remove from here?
ccfunc cci64_t
ccstrcatiV_(char **ccm, const char *fmt, va_list vli)
{ int len=stbsp_vsnprintf(0,0,fmt,vli);
	cci64_t res=ccdlbadd_(cccast(void**,ccm),1,len+1,len);
	len=stbsp_vsnprintf((char*)*ccm+res,len+1,fmt,vli);
	return res;
}

// Todo: remove from here?
ccfunc cci64_t
ccstrcatiF_(char **ccm, const char *fmt, ...)
{ va_list vli;
	va_start(vli,fmt);
	cci64_t res=ccstrcatiV_(ccm,fmt,vli);
	va_end(vli);
	return res;
}

#ifdef __cplusplus
}
#endif

#ifdef _CCTEST_COMPILE
unsigned int cctestxorshift32(unsigned int *p)
{ unsigned int q = *p;
	q = q ^ q << 13;
	q = q ^ q >> 17;
	q = q ^ q << 5;
	return *p=q;
}
unsigned int cctestrandi(unsigned int limit)
{ ccglobal ccthread_local unsigned int ccxorstate32=38747;
	return cctestxorshift32(&ccxorstate32)%limit;
}
void cctest()
{
	char h[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	char r[1024][128];
	typedef struct _t { char *s; } _t;

	_t *v=ccnull;
	for(int i=0; i<256; ++i)
	rlArray_add(v,1)->s=h;

rlArray_delete(v);

ccdbenter("test");

int i,j;
for(i=0; i<ccCarrlenL(r); r[i++][j]=0)
for(j=0; j<ccCarrlenL(r[0])-1; r[i][j++]=h[cctestrandi(ccCstrlenL(h))]);

{
	_t *tt=ccnull;
	for(j=0;j<1024;++j)
	{ for(i=0; i<ccCarrlenL(r); ++i)
		{ _t *a=cctblputS(tt,r[i]);
			ccassert(ccerrnon());
			a->s=cckeyget();
			_t *b=cctblremS(tt,r[i]);
			ccassert(ccerrnon());
			ccassert(a==b);
			ccassert(!memcmp(a->s,b->s,sizeof(h)));
		}
	}
	ccdlbdel(tt);
}
{
	_t *tt=ccnull;
	for(i=0; i<ccCarrlenL(r); ++i)
	{ _t *a=cctblputS(tt,r[i]);
		ccassert(ccerrnon());
		a->s=cckeyget();
		_t *b=cctblgetS(tt,r[i]);
		ccassert(ccerrnon());
		ccassert(a==b);
		ccassert(!memcmp(a->s,b->s,sizeof(h)));
	}
	ccdlbdel(tt);
}

ccdbenter("test-ccdlbputS");
_t *a=ccnull;
for(i=0; i<ccCarrlenL(r); ++i)
{ _t *t=cctblputS(a,r[i]);
	ccassert(ccerrnon());
	t->s=cckeyget();
	ccassert(!strcmp(t->s,r[i]));
}
ccdbleave("test-ccdlbputS");

ccdbenter("test-ccdlbgetS");
for(i=0; i<ccCarrlenL(r); ++i)
{ _t *t=cctblgetS(a,r[i]);
	ccassert(ccerrnon());
	ccassert(t->s!=0);
	ccassert(!strcmp(t->s,r[i]));
	ccassert(!strcmp(t->s,cckeyget()));

	t=cctblgetS(a,r[i]);
}

ccdbleave("test-ccdlbgetS");

ccdbenter("test-ccdlbdel");
ccdlbdel(a);
ccdbleave("test-ccdlbdel");

ccdbleave("test");
}
#endif

#ifdef _WIN32

ccfunc ccinle cccaller_t
cccaller(int guid, const char *file, int line, const char *func)
{
	cccaller_t t;
	t.guid=guid;
	t.file=file;
	t.line=line;
	t.func=func;
	return t;
}

#include "ccio.h"
#include "ccio.cc"

#endif
#endif