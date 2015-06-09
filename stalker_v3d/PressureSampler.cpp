#include "PressureSampler.h"
#include "utils/matmath.h"
#include <vector>
#include <math.h>
#include <stdlib.h>     /* srand, rand */
#include <iostream>

using namespace std;
typedef std::vector<float> vectype;

PressureSampler::PressureSampler(int ndir, 
                                 int density,
                                 ImagePointer OriginalImg,
                                 GradientImagePointer GVF,
                                 float radius): 
                                               ndir(ndir),
                                               density(density), 
                                               OriginalImg(OriginalImg), 
                                               GVF(GVF), 
                                               radius(radius)
{
    this->SetNDir(ndir);
}


PressureSampler::~PressureSampler(){
}


void PressureSampler::GenSph(){
// Uniformly pick directions on a unit sphere
// Ref: http://mathworld.wolfram.com/SpherePointPicking.html

    this->baseth.clear();
    this->basephi.clear();
    int nphi = (int) floor(pow(((double) this->ndir / 2.0), 0.5));
    int nth = (int) 2 * nphi;
    this->ndir = nphi * nth; // Refine the ndir according to the spherical distribution

    vectype u = linspace(0, 1, nth);
    vectype v = linspace(0, 1, nphi);

    vectype repu = repmat1d(u, nphi, 2);
    vectype repv = repmat1d(v, nth, 2);
    vectype transv = transpose(repv, nphi, nth);// Transpose repv matrix

    for (int i = 0; i < this->ndir; i++)
    {
        this->baseth.push_back(2.0 * M_PI * repu[i]);
    }

    for (int i = 0; i < this->ndir; i++)
    {
        this->basephi.push_back(acos(2.0 * transv[i] - 1));
    }
}


void PressureSampler::SetNDir(int ndir){
    this->ndir = ndir;
    this->GenSph();
}


void PressureSampler::FindVoxel2Sample(float th, float phi, vectype * outx, vectype* outy, vectype* outz, int pointrange)
{
    float rl; float random; float random_r; float t;

    for (int n=pointrange; n>0; n--) 
    {  
        //random ranges from 0 to 1
        random = ((float) rand()) / (float) RAND_MAX;
        random_r = ((float) rand()) / (float) RAND_MAX;
    
        //assign theta phi value to the normal vector
        t = 2 * M_PI * random;  rl = this->radius * random_r;
        (*outx)[n] = rl * cos(t) * (-sin(phi)) + rl * sin(t) * cos(th) * cos(phi) +  this->x;
        (*outy)[n] = rl * cos(t) * cos(phi) + rl * sin(t) * cos(th) * sin(phi) +  this->y;
        (*outz)[n] = rl * sin(t) * (-sin(th)) + this->z;
    }

    return; 
}


vector<GradientPixelType> PressureSampler::GetGradientAtIndex(vector<int> lx, vector<int> ly, vector<int> lz)
{

    vector<GradientPixelType> lvg;
    for (int i = 0; i < lx.size(); i++)
    {
        GradientImageType::IndexType idx;
        typedef itk::VectorLinearInterpolateImageFunction<GradientImageType, float >
            GradientInterpolatorType;
        GradientInterpolatorType::Pointer interpolator = GradientInterpolatorType::New();
        interpolator->SetInputImage(this->GVF);
        idx[0] = lx[i];
        idx[1] = ly[i];
        idx[2] = lz[i];
        GradientPixelType gpixel = interpolator->EvaluateAtIndex(idx);
        lvg.push_back(gpixel);
    }
 return lvg; } void PressureSampler::UpdatePosition(float x, float y, float z)
{
    this->x = x; this->y = y; this->z = z;
}


void PressureSampler::RandRotateSph()
{
    assert(this->baseth.size() == this->basephi.size());
    float dth = ((float) rand()) / (float) RAND_MAX * 2 * M_PI;
    float dphi = ((float) rand()) / (float) RAND_MAX * 2 * M_PI;

    for (int i=0;i<this->baseth.size();i++)
    {
        this->baseth[i] += dth;
        this->basephi[i] += dphi;
    }
}


float PressureSampler::Moment(vectype v, vectype xvec, vectype yvec, vectype zvec){
    // Convert float vectors to int 
    vector<int> txvec(xvec.begin(), xvec.end());
    vector<int> tyvec(yvec.begin(), yvec.end());
    vector<int> tzvec(zvec.begin(), zvec.end());

    vector<GradientPixelType> lvg = this->GetGradientAtIndex(txvec, tyvec, tzvec);

    float testradius = this->density;
    vectype u(3, 0);
    float tempdis, temp_f, temp_fl;
    u[0] = 3;
    u[1] = 4;
    u[2] = 5;

    for(int n=this->density; n>0; n--)
    {
        u[0] = lvg[n][0];
        u[1] = lvg[n][0];
        u[2] = lvg[n][0];
        vecnorm(u, v);
        tempdis = (this->x - txvec[n]) * (this->x - txvec[n]) + (this->y - tyvec[n]) * (this->y - tyvec[n])
                  + (this->z - tzvec[n]) * (this->z - tzvec[n]);
        tempdis = pow(tempdis, 0.5);
        temp_f = u[0] * u[0] + u[1] * u[1] + u[2] * u[2];
        temp_f = pow(temp_f, 0.5);
        temp_fl = temp_f * tempdis + temp_fl;
    }

    return temp_fl / this->density;
}