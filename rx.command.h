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

typedef void (*rxcustom_t)();

enum
{ rx_kENABLE_ZTESTING = 0,
  rx_kENABLE_CLIPPING = 1,
  rx_kANTIALIASING    = 2,
  rx_kMULTISAMPLING   = 3,
  rx_kCOLOR_BLENDING  = 4,
};

typedef struct rxcom_t rxcom_t;
typedef struct rxcom_t
{
  const char *label;

  int   kind; /* Have separate enum for this - XXX - the one called rj */
  int   offset;
  int   length;

 union
 { struct
   { int x0,y0,
         x1,y1; };
   struct
   { int option;
     int enable; };

   rxrender_target_t   target;
          rxshader_t   shader;
         rxsampler_t   sampler;
         rxtexture_t   texture;
          rxcustom_t   custom;
          rxmatrix_t   matrix; };
} rxcom_t;
;

void
rxenable_ztesting(
  int enable );

void
rxenable_clipping(
  int enable );

void
rxenable_softline(
  int enable );



