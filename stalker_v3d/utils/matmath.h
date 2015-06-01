#include <vector>

typedef std::vector<double> vectype;

vectype linspace(const double a, const double b, const int n);

//void sph2cart(float * lth, float * lphi, lx, ly, lz, int sz);

//void cart2sph(float * lx, float * ly, float * lz, int sz);

void repmat1d(vectype m, const int repeat, const int dim, double *outm);

void transpose(double * src, double *dst, const int N, const int M);