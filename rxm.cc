#ifndef _RXM_HEADER
#define _RXM_HEADER

typedef struct rxpoint16_t rxpoint16_t;
typedef struct rxpoint16_t
{
  short x,y;
} rxpoint16_t;
typedef struct rxvector2_t rxvector2_t;
typedef struct rxvector2_t
{
  float x,y;
} rxvector2_t;
typedef struct rxvector3_t rxvector3_t;
typedef struct rxvector3_t
{
  float x,y,z;
} rxvector3_t;
rxvector3_t   rxvector    (float xyz);
rxvector3_t   rxvector_xyz(float x, float y, float z);
rxvector3_t   rxvector_xy (float x, float y);
rxvector3_t   rxvector_x  (float x);
rxvector3_t   rxvector_y  (float y);
rxvector3_t   rxvector_z  (float z);
typedef struct rxvector4_t rxvector4_t;
typedef struct rxvector4_t
{ union
  { struct
    {
      float x,y,z,w;
    };
    struct
    {
      float r,g,b,a;
    };
    rxvector3_t xyz;
    rxvector3_t rgb;
  };
} rxvector4_t;
typedef struct rxmatrix_t rxmatrix_t;
typedef struct rxmatrix_t
{ float m[4][4];
} rxmatrix_t;
rxmatrix_t rxmatrix_identity();
rxmatrix_t rxmatrix_multiply(rxmatrix_t, rxmatrix_t);


#endif