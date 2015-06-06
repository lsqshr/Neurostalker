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
<<<<<<< HEAD
        (*outx)[n] = rl * cos(t) * (-sin(phi)) + rl * sin(t) * cos(th) * cos(phi) +  this->x;
        (*outy)[n] = rl * cos(t) * cos(phi) + rl * sin(t) * cos(th) * sin(phi) +  this->y;
        (*outz)[n] = rl * sin(t) * (-sin(th)) + this->z;
=======
        (*outx)[n] = rl * cos(t) * (-sin(phi)) + rl * sin(t) * cos(th) * cos(phi) +  x;
        (*outy)[n] = rl * cos(t) * cos(phi) + rl * sin(t) * cos(th) * sin(phi) +  y;
        (*outz)[n] = rl * sin(t) * (-sin(th)) + z;


        //cout<<d<<endl;  
        //cout<<(*outx)[n]<<" "<<(*outy)[n]<<" "<<(*outz)[n]<<"center distance"<<center_distance<<endl;
>>>>>>> origin/voxelsample
    }

    //cout<<boolalpha<<"judge flag is :"<<current_judge<<endl;
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

    return lvg;
}


void PressureSampler::SampleVoxels(const vector<float> lx, const vector<float> ly, const vector<float> lz)
{
// Sample the distortion energy at each direction
// E = 1/N * sum_i{l_i * f_i}
// where N is the total number of voxels, l_i is the distance between the sampling position and the center of the plane;
// f_i is the orthogonal resultant force on the plane

    assert(lx.size() == ly.size() && ly.size() == lz.size());
    vector<float> distance(lx.size());

    vectype l = eucdistance2center(this->x, this->y, this->z, lx, ly, lz);
}


void PressureSampler::UpdatePosition(float x, float y, float z)
{
    this->x = x; this->y = y; this->z = z;
}