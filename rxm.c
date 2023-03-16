#ifndef _RXM_C
#define _RXM_C

typedef struct rxf32x2_t rxf32x2_t;
typedef struct rxf32x2_t
{
  float x,y;
} rxf32x2_t;

typedef struct rxf32x4_t rxf32x4_t;
typedef struct rxf32x4_t
{
  float x,y,z,w;
} rxf32x4_t;

typedef struct rxmatrix_t rxmatrix_t;
typedef struct rxmatrix_t
{ float m[4][4];
} rxmatrix_t;

typedef struct rxi32x2_t rxi32x2_t;
typedef struct rxi32x2_t
{
  int x,y;
} rxi32x2_t;

typedef struct rxi32x4_t rxi32x4_t;
typedef struct rxi32x4_t
{
  int x,y,z,w;
} rxi32x4_t;

#ifndef rxf32x4
# define rxf32x4(x,y,z,w) (rxf32x4_t){(float)x,(float)y,(float)z,(float)w}
#endif

rxmatrix_t
rxmatrix_identity()
{
	rxmatrix_t result;
	result.m[0][0]=1.f;
  result.m[0][1]=0.f;
  result.m[0][2]=0.f;
  result.m[0][3]=0.f;

  result.m[1][0]=0.f;
  result.m[1][1]=1.f;
  result.m[1][2]=0.f;
  result.m[1][3]=0.f;

  result.m[2][0]=0.f;
  result.m[2][1]=0.f;
  result.m[2][2]=1.f;
  result.m[2][3]=0.f;

  result.m[3][0]=0.f;
  result.m[3][1]=0.f;
  result.m[3][2]=0.f;
  result.m[3][3]=1.f;

  return result;
}

rxmatrix_t
rxmatrix_rotZ(float angle)
{
	rxmatrix_t result=rxmatrix_identity();

  float cosres = cosf(angle);
  float sinres = sinf(angle);

	result.m[0][0]=+cosres;
  result.m[0][1]=+sinres;
  result.m[1][0]=-sinres;
  result.m[1][1]=+cosres;
  return result;
}

rxmatrix_t
rxmatrix_translate_xyz(float x, float y, float z)
{
	rxmatrix_t result=rxmatrix_identity();

  result.m[3][0]=x;
  result.m[3][1]=y;
  result.m[3][2]=z;
  return result;
}

rxmatrix_t
rxmatrix_flip_vertically()
{
	rxmatrix_t result=rxmatrix_identity();
  result.m[1][1]=-1.f;

  return result;
}

rxmatrix_t rxmatrix_mulM(rxmatrix_t a, rxmatrix_t b)
{ rxmatrix_t result;
  for(int r=0; r<4; ++r)
  { for(int c=0; c<4; ++c)
    {  result.m[r][c] =
        (a.m[r][0]*b.m[0][c]) +
        (a.m[r][1]*b.m[1][c]) +
        (a.m[r][2]*b.m[2][c]) +
        (a.m[r][3]*b.m[3][c]);
    }
  }
  return result;
}

#endif