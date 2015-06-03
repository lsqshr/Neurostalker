#include "PressureSampler.h"
#include "utils/matmath.h"
#include <vector>
#include <math.h>
#include <stdlib.h>     /* srand, rand */
#include <iostream>

using namespace std;
typedef std::vector<double> vectype;

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


void PressureSampler::GenSph(){
// Uniformly pick directions on a unit sphere
// Ref: http://mathworld.wolfram.com/SpherePointPicking.html

    int nphi = (int) pow(((double)this->ndir / 2.0), 0.5);
    int nth = (int) 2 * nphi;
    //itk::Matrix<double, nphi, nth> uM;
    //itk::Matrix<double, nth, nphi> vM;

    vectype u = linspace(0, 1, nth);
    vectype v = linspace(0, 1, nphi);

    vectype repu = repmat1d(u, nphi, 2);

    vectype repv = repmat1d(v, nth, 2);
    vectype transv = transpose(repv, v.size(), nth);// Transpose repv matrix

    for (int i = 0; i < u.size() * nphi; i++)
        this->baseth[i] = 2 * M_PI * repu[i];

    for (int i = 0; i < v.size() * nth; i++)
        this->basephi[i] = acos(2 * transv[i] - 1);
}


void PressureSampler::FindVoxel2Sample(float x, float y, float z, float th, float phi, vectype * outx, vectype* outy, vectype* outz, int pointrange)
{
    float rl; float random; float random_r; float t;
    bool current_judge = true; 
    for (int n=pointrange; n>0; n--) 
    {  
        //random ranges from 0 to 1
        random = ((float) rand()) / (float) RAND_MAX;
        random_r = ((float) rand()) / (float) RAND_MAX;
    
        //assign theta phi value to the normal vector
        t = 2 * M_PI * random;  rl = this->radius * random_r;
        (*outx)[n] = rl * cos(t) * (-sin(phi)) + rl * sin(t) * cos(th) * cos(phi) +  x;
        (*outy)[n] = rl * cos(t) * cos(phi) + rl * sin(t) * cos(th) * sin(phi) +  y;
        (*outz)[n] = rl * sin(t) * (-sin(th)) + z;
        float center_distance = ((*outx)[n] - x) * ((*outx)[n] - x) + ((*outy)[n] - y) * ((*outy)[n] - y)\
         + ((*outz)[n] - z) * ((*outz)[n] - z);
        current_judge = ((center_distance <=  ((this->radius) * (this->radius)))) && current_judge;  
        cout<<boolalpha<<"judge flag is :"<<current_judge<<endl; 
        //cout<<(*outx)[n]<<" "<<(*outy)[n]<<" "<<(*outz)[n]<<"center distance"<<center_distance<<endl;
    }
    return; 
}


