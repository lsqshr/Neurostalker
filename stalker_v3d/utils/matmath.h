#include <vector>

typedef std::vector<double> vectype;

vectype linspace(double a, double b, int n);

//void sph2cart(float * lth, float * lphi, lx, ly, lz, int sz);

//void cart2sph(float * lx, float * ly, float * lz, int sz);

void repmat1d(vectype m, int repeat, int dim, double *outm, int szx, int szy);