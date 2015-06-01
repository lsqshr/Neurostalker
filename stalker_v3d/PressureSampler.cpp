#include "PressureSampler.h"

PressureSampler::PressureSampler(int ndir, 
                                 int density,
                                 unsigned char * OriginalImg,
                                 unsigned char * BinaryImg,
                                 float * GVF,
                                 float radius): 
                                                   ndir(ndir),
                                                   density(density), 
                                                   OriginalImg(OriginalImg), 
                                                   BinaryImg(BinaryImg), 
                                                   GVF(GVF), 
                                                   radius(radius)
{
    this->baseth = new float[ndir];
    this->basephi = new float[ndir];
    this->lpressure = new float[ndir*density];
    this->GenSph();
}


PressureSampler::~PressureSampler(){
   if(this->baseth) delete baseth;
   if(this->basephi) delete basephi;
   if(this->lpressure) delete lpressure;
}


PressureSampler::GenSph(){
// Uniformly pick directions on a unit sphere
// Ref: http://mathworld.wolfram.com/SpherePointPicking.html

    int nphi = (int) pow(((double)this->ndir / 2.0), 0.5)
    int nth = (int) 2 * nphi;
    itk::Matrix<double, nphi, nth> uM;
    itk::Matrix<double, nth, nphi> vM;

    vector<double> u = linspace(0, 1, nth);
    vector<double> v = linspace(0, 1, nphi);

    // Strech u vertically to nphi rows
    for (int i = 0; i < nphi; i ++)
    {
        for (int j = 0; j < nth; j++)
        {
            uM(i, j) = u[j];
        }
    }

    // Strech v vertically to nth rows
    for (int i = 0; i < nth; i ++)
    {
        for (int j = 0; j < nphi; j++)
        {
            vM(i, j) = u[j];
        }
    }

    

}
