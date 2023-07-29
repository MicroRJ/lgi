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

/* - XXX - the one called rj */
rxborrowed_t
rxbuffer_borrow(rxunknown_t buffer);

typedef struct rxuniform_buffer_t rxuniform_buffer_t;
typedef struct rxuniform_buffer_t
{
  rxunknown_t unknown;
} rxuniform_buffer_t;

void
rxuniform_buffer_delete(
  rxuniform_buffer_t buffer );

void
rxuniform_buffer_update(
  rxuniform_buffer_t uniform, void *memory, size_t length );

void
rxuniform_buffer_bind_ex(
  rxuniform_buffer_t buffer, int offset);

/* Should we store the size or format of the structures here? - XXX - the one called rj */
typedef struct rxstruct_buffer_t rxstruct_buffer_t;
typedef struct rxstruct_buffer_t
{
  rxunknown_t unknown;
} rxstruct_buffer_t;

/* Should we store the size or format of the indices here? - XXX - the one called rj */
typedef struct rxindex_buffer_t rxindex_buffer_t;
typedef struct rxindex_buffer_t
{
  rxunknown_t unknown;
} rxindex_buffer_t;

/* Should we store the size or format of the vertices here? - XXX - the one called rj */
typedef struct rxvertex_buffer_t rxvertex_buffer_t;
typedef struct rxvertex_buffer_t
{
  rxunknown_t unknown;
} rxvertex_buffer_t;