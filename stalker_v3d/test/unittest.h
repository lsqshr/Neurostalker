// Test "utils/matmath.h"
#include <limits> 
#include "lib/ImageOperation.h"
#include "stackutil.h"
#define EPSILON 1E-2; //Since we use floats, do not be too harsh

void TestMatMath();

void TestPressureSampler(ImagePointer, GradientImagePointer);

void TestRadius(unsigned char * inimg1d, V3DLONG * sz);
