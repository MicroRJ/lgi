
// note: this is not yet a well integrated file and is subject
// to radical changes

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