#define rxlabel_kLOADED      1
#define rxlabel_kERRONEOUS   2
#define rxlabel_kINVALIDATED 4

/* If you know what's good for you, you'd get rid of this immediatly - XXX */
typedef struct rxterminal_t rxterminal_t;
typedef struct rxterminal_t
{ unsigned  int  length;
  void         * memory;
  ccclocktick_t  loaded;
            int  labels;
    const char * master;
} rxterminal_t;

rxterminal_t *
rxlinker_register_terminal(
  const char *master);

rxterminal_t *
rxlinker_onlyquery_terminal(
  const char *master);

/* If you know what's good for you, you'd get rid of this immediatly - XXX */
int
rxlinker_labelshas_terminal(
  const char *master, int labels);

/* If you know what's good for you, you'd get rid of this immediatly - XXX */
int
rxlinker_labelsadd_terminal(
  const char *master, int labels);

/* If you know what's good for you, you'd get rid of this immediatly - XXX */
int
rxlinker_labelsrem_terminal(
  const char *master, int labels);

/* If you know what's good for you, you'd get rid of this immediatly - XXX */
rxterminal_t *
rxlinker_resolve_terminal(
  const char *master);

/* If you know what's good for you, you'd get rid of this immediatly - XXX */
typedef enum rxlinkage_k
{
  rxlinkage_kSHADER_RESOURCE_VIEW,
  rxlinkage_kRENDER_TARGET_VIEW,
  rxlinkage_kVERTEX_LAYOUT,
  rxlinkage_kCOUNT,
} rxlinkage_k;

/* If you know what's good for you, you'd get rid of this immediatly - XXX */
typedef struct rxrestore_t rxrestore_t;
typedef struct rxrestore_t
{
  const    char *shader_model;
  const    char *shader_entry;
} rxrestore_t;

/* If you know what's good for you, you'd get rid of this immediatly - XXX */
typedef struct rxarticle_t rxarticle_t;
typedef struct rxarticle_t
{ rx_k                sorting;
  rxunknown_t         unknown;
  rxunknown_t         linkage[rxlinkage_kCOUNT];
  rxrestore_t         restore;
  const char      *    master;
  ccclocktick_t        loaded;
} rxarticle_t;
