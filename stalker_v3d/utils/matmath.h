#include <itkMatrix.h>
#include <itkVector.h>
#include <vector>

vector<double> linspace(double a, double b, int n);

//void sph2cart(float * lth, float * lphi, lx, ly, lz, int sz);

//void cart2sph(float * lx, float * ly, float * lz, int sz);

void repmat1d(vector<double> m, int repeat, int dim, double *outm, int szx, int szy);