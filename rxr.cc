#ifndef _RXTANGIBLE_HEADER
#define _RXTANGIBLE_HEADER

typedef enum rxresource_k
{ rxr_kSHADER_COMPUTE,
  rxr_kSHADER_PIXEL,
  rxr_kSHADER_VERTEX,

  rxr_kVERTEX_FORMAT,

  rxr_kBUFFER_UNIFORM,
  rxr_kBUFFER_STRUCT,
  rxr_kBUFFER_VERTEX,
  rxr_kBUFFER_INDEX,

  rxr_kSAMPLER,
  rxr_kTEXTURE,

  rxr_kRENDER_TARGET,
} rxresource_k;

// note:
// One if the features that I always want to have out of the box is live-reload, especially when
// working with shaders. The way we do this is by keeping track of every file we load and whether its
// contents have changed or not, this also means that live-reload works for anything that is bound to
// a name on disk implicitly when using the default 'rxload-contents' function.
// todo!!: we should instead hash the file's contents!
typedef struct rxcontents_t rxcontents_t;
typedef struct rxcontents_t
{ unsigned  int  length;
  void         * memory;
  ccclocktick_t  loaded;
  // note: we keep track of whether the contents of this file
  // are erroneous, in which case all instances bound to this
  // file shouldn't be reloaded/restored if not changed.
  unsigned          is_erroneous: 1;
} rxcontents_t;

// note: the parameters we need to restore the instance,
// could probably have come up with a better name.
typedef struct rxrestore_t rxrestore_t;
typedef struct rxrestore_t
{
  const    char *shader_model;
  const    char *shader_entry;
} rxrestore_t;

typedef struct rxtangible_t rxtangible_t;
typedef struct rxtangible_t
{ rx_k                   sort;
  rxunknown_t         unknown;
  const    char   *    master;
  ccclocktick_t        loaded;
  rxrestore_t         restore;
} rxtangible_t;

void
rxdelete_instance(
  rxunknown_t unknown);

void
rxcontents_erroneous(
  const char *master, int erroneous);

#endif

#ifdef _RXTANGIBLE_IMPLEMENTATION

void
rxcontents_erroneous(
  const char *master, int erroneous)
{
  rxcontents_t *i=cctblgetS(rx.contents_table,master);
  ccassert(ccerrnon());

  i->is_erroneous=erroneous;
}

rxtangible_t *
rxremove_tangible(
  rxunknown_t unknown)
{
  ccassert(unknown != 0);

  rxtangible_t *i=cctblremP(rx.instance_table,unknown);
  ccassert(ccerrnon());

  return i;
}

void
rxdelete_instance(
  rxunknown_t unknown)
{
  rxtangible_t *i=rxremove_tangible(unknown);

  ccassert(i->unknown != NULL);
  ID3D11DeviceChild_Release(i->unknown);
  i->unknown=NULL;
}
#endif
