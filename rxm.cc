#ifndef _RXM_HEADER
#define _RXM_HEADER

#define rxPI_F 3.14159265358979323846f
#define rxPI   3.14159265358979323846
#define rxABS(v)   ((v)<(0)?-(v):(v))
#define rxMIN(x,y) ((x)<(y)? (x):(y))
#define rxMAX(x,y) ((x)>(y)? (x):(y))


typedef struct rxvec2i16_t rxvec2i16_t;
typedef struct rxvec2i16_t
{
  short x,y;
} rxvec2i16_t;
typedef struct rxvec2i_t rxvec2i_t;
typedef struct rxvec2i_t
{
  int x,y;
} rxvec2i_t;
typedef struct rxvec3i_t rxvec3i_t;
typedef struct rxvec3i_t
{ union
  {
    struct
    {
      int x,y,z;
    };

    rxvec2i_t xy;
  };
} rxvec3i_t;
rxvec3i_t  rxvec3i    (float xyz);
rxvec3i_t  rxvec3i_xyz(float x, float y, float z);
rxvec3i_t  rxvec3i_xy (float x, float y);
rxvec3i_t  rxvec3i_x  (float x);
rxvec3i_t  rxvec3i_y  (float y);
rxvec3i_t  rxvec3i_z  (float z);

typedef struct rxvec2_t rxvec2_t;
typedef struct rxvec2_t
{
  float x,y;
} rxvec2_t;
typedef struct rxvec3_t rxvec3_t;
typedef struct rxvec3_t
{ union
  {
    struct
    {
      float x,y,z;
    };

    rxvec2_t xy;
  };
} rxvec3_t;
rxvec3_t   rxvec3    (float xyz);
rxvec3_t   rxvec3_xyz(float x, float y, float z);
rxvec3_t   rxvec3_xy (float x, float y);
rxvec3_t   rxvec3_x  (float x);
rxvec3_t   rxvec3_y  (float y);
rxvec3_t   rxvec3_z  (float z);
/* this will have to be renamed to float4 instead, usually vec3 has the w component implicitly, oops,
	 - XXX - the one called rj */
typedef struct rxvec4_t rxvec4_t;
typedef struct rxvec4_t
{ union
  { struct { float x,y,z,w; };
    struct { float r,g,b,a; };
    rxvec3_t xyz;
    rxvec3_t rgb;
    rxvec2_t xy;
  };
} rxvec4_t;

typedef struct rxmatrix_t rxmatrix_t;
typedef struct rxmatrix_t
{ float m[4][4];
} rxmatrix_t;
rxmatrix_t rxmatrix_identity();
rxmatrix_t rxmatrix_multiply(rxmatrix_t, rxmatrix_t);

/* ++ added 'rxclamp'
	 ++ switched to 64-bit floats instead */
double rxclamp(double val, double min, double max)
{
  return val < min ? min : val > max ? max : val;
}

int rxpow2i(int x)
{
  return x*x;
}

int rxclampi(int val, int min, int max)
{
  return val<min?min:
         val>max?max: val;
}

float rxmax(float x, float y)
{
  return x>y?x:y;
}

float rxmin(float x, float y)
{
  return x<y?x:y;
}

int rxmaxi(int x, int y)
{
  return x>y?x:y;
}

int rxmini(int x, int y)
{
  return x<y?x:y;
}

rxvec3_t rxvec3_xyz(float x, float y, float z)
{
  rxvec3_t r;
  r.x = x;
  r.y = y;
  r.z = z;
  return r;
}

rxvec3_t rxvec3_xy(float x, float y)
{
  return rxvec3_xyz(x,y,0);
}

rxvec3_t rxvec3_x(float x)
{
  return rxvec3_xyz(x,0,0);
}

rxvec3_t rxvec3_y(float y)
{
  return rxvec3_xyz(0,y,0);
}

rxvec3_t rxvec3_z(float z)
{
  return rxvec3_xyz(0,0,z);
}

rxvec3_t rxvec3(float xyz)
{
  return rxvec3_xyz(xyz,xyz,xyz);
}

float rxvector_dot(rxvec3_t a, rxvec3_t b)
{
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

rxvec3_t rxvector_cross(rxvec3_t a, rxvec3_t b)
{
  // note: this is how I memorize the cross product formula, think of rotations around an axis.
  return rxvec3_xyz
    ( a.y*b.z - a.z*b.y,    // x-axis -> z/y plane
      a.z*b.x - a.x*b.z,    // y-axis -> x/z plane
      a.x*b.y - a.y*b.x );  // z-axis -> y/x plane
}

float rxvector_length(rxvec3_t a)
{
  return sqrtf(rxvector_dot(a,a));
}

float rxvec2_dot(rxvec2_t a, rxvec2_t b)
{
  return a.x*b.x + a.y*b.y;
}

float rxvec2_len(rxvec2_t a)
{
  return sqrtf(rxvec2_dot(a,a));
}

rxvec2_t rxvec2_add(rxvec2_t a, rxvec2_t b)
{
  rxvec2_t r;
  r.x = a.x+b.x;
  r.y = a.y+b.y;
  return r;
}

rxvec2_t rxvec2_sub(rxvec2_t a, rxvec2_t b)
{
  rxvec2_t r;
  r.x = a.x-b.x;
  r.y = a.y-b.y;
  return r;
}

rxvec3_t rxvector_add(rxvec3_t a, rxvec3_t b)
{
  rxvec3_t r;
  r.x = a.x+b.x;
  r.y = a.y+b.y;
  r.z = a.z+b.z;
  return r;
}

rxvec3_t rxvector_sub(rxvec3_t a, rxvec3_t b)
{
  rxvec3_t r;
  r.x = a.x-b.x;
  r.y = a.y-b.y;
  r.z = a.z-b.z;
  return r;
}

rxvec3_t rxvector_mul(rxvec3_t a, rxvec3_t b)
{
  rxvec3_t r;
  r.x = a.x*b.x;
  r.y = a.y*b.y;
  r.z = a.z*b.z;
  return r;
}

rxvec3_t rxvector_downscale(rxvec3_t a, float b)
{
  rxvec3_t r;
  r.x = a.x/b;
  r.y = a.y/b;
  r.z = a.z/b;
  return r;
}

rxvec3_t rxvector_scale(rxvec3_t a, float b)
{
  rxvec3_t r;
  r.x = a.x*b;
  r.y = a.y*b;
  r.z = a.z*b;
  return r;
}

rxvec3_t rxvector_negate(rxvec3_t a)
{
  rxvec3_t r;
  r.x = - a.x;
  r.y = - a.y;
  r.z = - a.z;
  return r;
}

rxvec3_t rxvector_min(rxvec3_t a, float min)
{
  rxvec3_t r;
  r.x = a.x > min ? min : a.x;
  r.y = a.y > min ? min : a.y;
  r.z = a.z > min ? min : a.z;
  return r;
}

rxvec3_t rxvector_max(rxvec3_t a, float max)
{
  rxvec3_t r;
  r.x = a.x < max ? max : a.x;
  r.y = a.y < max ? max : a.y;
  r.z = a.z < max ? max : a.z;
  return r;
}

rxvec3_t rxvector_abs(rxvec3_t a)
{
  rxvec3_t r;
  r.x = a.x < 0 ? - a.x : a.x;
  r.y = a.y < 0 ? - a.y : a.y;
  r.z = a.z < 0 ? - a.z : a.z;
  return r;
}


rxvec3_t rxvector_normalize(rxvec3_t a)
{
  float length=rxvector_length(a);

  if(length != 0)
    a=rxvector_scale(a,1/length);

  return a;
}

rxmatrix_t rxmatrix_projection(double r, double v, double zmin, double zmax)
{
  rxmatrix_t m = rxmatrix_identity();
  v = 1. / tan(v / 180 * rxPI_F * .5);

  m.m[0][0] = r * v;
  m.m[1][1] = v;
  m.m[2][2] = zmax / (zmax - zmin);
  m.m[3][2] = - (zmax * zmin) / (zmax - zmin);
  m.m[3][3] = 0.;
  m.m[2][3] = 1.;
  return m;
}

rxmatrix_t rxmatrix_identity()
{
  rxmatrix_t r;
  r.m[0][0]=1.f;r.m[1][0]=0.f;r.m[2][0]=0.f;r.m[3][0]=0.f;
  r.m[0][1]=0.f;r.m[1][1]=1.f;r.m[2][1]=0.f;r.m[3][1]=0.f;
  r.m[0][2]=0.f;r.m[1][2]=0.f;r.m[2][2]=1.f;r.m[3][2]=0.f;
  r.m[0][3]=0.f;r.m[1][3]=0.f;r.m[2][3]=0.f;r.m[3][3]=1.f;
  return r;
}



// todo!!: remove loop
rxmatrix_t rxmatrix_multiply(rxmatrix_t a, rxmatrix_t b)
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

rxmatrix_t rxmatrix_rotZ(float angle)
{ rxmatrix_t result=rxmatrix_identity();
  float cosres=cosf(angle);
  float sinres=sinf(angle);
  result.m[0][0]=+cosres;
  result.m[0][1]=+sinres;
  result.m[1][0]=-sinres;
  result.m[1][1]=+cosres;
  return result;
}

rxmatrix_t rxmatrix_translate_xyz(float x, float y, float z)
{ rxmatrix_t result=rxmatrix_identity();
  result.m[3][0]=x;
  result.m[3][1]=y;
  result.m[3][2]=z;
  return result;
}

rxmatrix_t rxmatrix_flip_vertically()
{ rxmatrix_t result=rxmatrix_identity();
  result.m[1][1]=-1.f;
  return result;
}

#endif