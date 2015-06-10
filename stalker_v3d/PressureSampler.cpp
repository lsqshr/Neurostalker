#include "PressureSampler.h"
#include "utils/matmath.h"
#include <vector>
#include <algorithm> // sort
#include <math.h>
#include <stdlib.h>     /* srand, rand */
#include <iostream>

using namespace std;
typedef std::vector<float> vectype;

struct idxentrypair{ // For sort and obtain the indices
    int idx;
    float value;

    bool operator < (const idxentrypair& pair) const 
    {
        return value < pair.value;
    }
};

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

    // Cache neighbours for speed 

    this->dirneighbours.clear();
    this->dirneighbours.resize(this->ndir);

    // -- Sort all th according to phi for neighbour finding
    vector<idxentrypair> phipairs(this->ndir), thpairs(this->ndir);
    for (int i=0; i<this->ndir; i++)
    {
        phipairs[i].idx = i;
        phipairs[i].value = this->basephi[i];
        thpairs[i].idx = i;
        thpairs[i].value = this->baseth[i];
    }

    // Lambda sort needs std c++11, however some of the ITK libraries do not support c++11
    //std::sort(sortidx.begin(), sortidx.end(), [&phi2sort](const int& a, const int& b){return phi2sort[a] < phi2sort[b];}); 
    std::sort(phipairs.begin(), phipairs.end()); 

    vectype tth(this->ndir);
    vectype tphi(this->ndir);

    vector<idxentrypair> t_thpairs(this->ndir); // For permuting th idx
    // Permute both th and phi
    for (int i=0; i<ndir; i++)
    {
        tth[i] = this->baseth[phipairs[i].idx];
        tphi[i] = this->basephi[phipairs[i].idx];
        t_thpairs[i] = thpairs[phipairs[i].idx];
    }
    thpairs = t_thpairs;

    // Sort th with the same phi value
    int thctr = 1;
    vector<idxentrypair>::iterator thitr = thpairs.begin();
    int thstartidx = 0;

    for (int i=1; i<this->ndir; i++)
    {
        if (tphi[i] == tphi[i-1])
        { 
            thctr++;
            if (i!=this->ndir-1)
                continue;
        }
        sort(thitr, thitr + thctr);
        // head and tail
        thitr += thctr;
        thctr = 1;
    }

    // Loop through all directions horizontally
    // Add only the left and right neighbours to itself
    // The head and the tail are considered as neighbours as well
    for (int i=1; i<this->ndir; i++)
    {
        if (tphi[i] == tphi[i-1])
        { 
            thctr++;
            this->dirneighbours[thpairs[i].idx].neighbouridx.push_back(thpairs[i-1].idx);
            this->dirneighbours[thpairs[i-1].idx].neighbouridx.push_back(thpairs[i].idx);
            if (i!=this->ndir-1)
                continue;
        }

        int curidx = -1;
        if (i == this->ndir - 1) {curidx = i; thstartidx = i - thctr + 1;}
        else {curidx = i - 1; thstartidx = i - thctr;}

        // head and tail
        this->dirneighbours[thpairs[curidx].idx].neighbouridx.push_back(thpairs[thstartidx].idx);
        this->dirneighbours[thpairs[thstartidx].idx].neighbouridx.push_back(thpairs[curidx].idx);
        thitr += thctr;
        thctr = 1;
    }

    // -- Sort all phi according to th for neighbour finding -- reversed for previous section
    std::sort(thpairs.begin(), thpairs.end()); 

    // Permute both th and phi
    for (int i=0; i<ndir; i++)
    {
        tth[i] = this->baseth[thpairs[i].idx];
        tphi[i] = this->basephi[thpairs[i].idx];
        phipairs[i].idx = thpairs[i].idx;
        phipairs[i].value = tphi[i];
    }

    // Sort phi with the same th value
    int phictr = 1;
    vector<idxentrypair>::iterator phiitr = phipairs.begin();
    int phistartidx = 0;

    for (int i=1; i<this->ndir; i++)
    {
        if (tth[i] == tth[i-1])
        { 
            phictr++;
            if (i != this->ndir-1)
                continue;
        }

        sort(phiitr, phiitr + phictr);
        // head and tail
        phiitr += phictr;
        phictr = 1;
    }

    // Loop through all directions vertically, add neighbours
    for (int i=1; i<this->ndir; i++)
    {
        if (tth[i] == tth[i-1])
        { 
            phictr++;
            // Add the left&right neighbours of its upper neighbour to itself
            if (this->dirneighbours[phipairs[i-1].idx].neighbouridx.size() >=2) 
            {
                this->dirneighbours[phipairs[i].idx].neighbouridx.push_back(
                                                                    this->dirneighbours[phipairs[i-1].idx].neighbouridx[0]
                                                                    );
                this->dirneighbours[phipairs[i].idx].neighbouridx.push_back(
                                                                    this->dirneighbours[phipairs[i-1].idx].neighbouridx[1]
                                                                    );
            }

            // Add its left and right neighbours to its upper neighbour
            if (this->dirneighbours[phipairs[i].idx].neighbouridx.size() >=2) 
            {
                this->dirneighbours[phipairs[i-1].idx].neighbouridx.push_back(
                                                                    this->dirneighbours[phipairs[i].idx].neighbouridx[0]
                                                                    );
                this->dirneighbours[phipairs[i-1].idx].neighbouridx.push_back(
                                                                    this->dirneighbours[phipairs[i].idx].neighbouridx[1]
                                                                    );
            }

            this->dirneighbours[phipairs[i].idx].neighbouridx.push_back(phipairs[i-1].idx);// Add its upper neighbour
            this->dirneighbours[phipairs[i-1].idx].neighbouridx.push_back(phipairs[i].idx); // Add itself to its upper neighbour

            if (i!=this->ndir-1)
                continue;
        }

        int curidx = -1;
        if (i == this->ndir - 1) {curidx = i; phistartidx = i - phictr + 1;}
        else {curidx = i - 1; phistartidx = i - phictr;}

        // head and tail

        //this->dirneighbours[thpairs[i].idx].neighbouridx.push_back(phistartidx);
        //this->dirneighbours[thpairs[thstartidx].idx].neighbouridx.push_back(i);
        thitr += thctr;
        thctr = 1;
    }

    this->originbaseth = this->baseth;
    this->originbasephi = this->basephi;
}


void PressureSampler::SetNDir(int ndir){
    this->ndir = ndir;
    this->GenSph();
    this->lpressure.clear();
    this->lpressure.resize(ndir);
}


void PressureSampler::FindVoxel2Sample(float th, float phi, vectype * outx, vectype* outy, vectype* outz, int pointrange)
{
    float rl, random, random_r, t;
    LabelImageType::SizeType size = this->GVF->GetLargestPossibleRegion().GetSize();
    int M = size[0];
    int N = size[1];
    int Z = size[2];

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
        (*outx)[n] = constrain((*outx)[n], 0, M);
        (*outy)[n] = constrain((*outy)[n], 0, N);
        (*outz)[n] = constrain((*outz)[n], 0, Z);
    }

    return; 
}


vector<GradientPixelType> PressureSampler::GetGradientAtIndex(vector<int> lx, vector<int> ly, vector<int> lz)
{
    LabelImageType::SizeType size = this->GVF->GetLargestPossibleRegion().GetSize();
    int M = size[0];
    int N = size[1];
    int Z = size[2];

    vector<GradientPixelType> lvg;
    for (int i = 0; i < lx.size(); i++)
    {
        GradientImageType::IndexType idx;
        typedef itk::VectorLinearInterpolateImageFunction<GradientImageType, float >
            GradientInterpolatorType;
        GradientInterpolatorType::Pointer interpolator = GradientInterpolatorType::New();
        interpolator->SetInputImage(this->GVF);
        idx[0] = constrain(lx[i], 0, M);
        idx[1] = constrain(ly[i], 0, N);
        idx[2] = constrain(lz[i], 0, Z);
        GradientPixelType gpixel = interpolator->EvaluateAtIndex(idx);
        lvg.push_back(gpixel);
    }
 return lvg; } void PressureSampler::UpdatePosition(float x, float y, float z)
{
    this->x = x; this->y = y; this->z = z;
    this->GetRadius();
}


void PressureSampler::RandRotateSph()
{
    assert(this->baseth.size() == this->basephi.size());
    float dth = ((float) rand()) / (float) RAND_MAX * 2 * M_PI;
    float dphi = ((float) rand()) / (float) RAND_MAX * 2 * M_PI;

    for (int i=0; i<this->baseth.size(); i++)
    {
        this->baseth[i] = fmod(this->originbaseth[i] + dth, (float)(2*M_PI));
        this->basephi[i] = fmod(this->originbasephi[i] + dphi, (float)(M_PI));
    }
}


float PressureSampler::Moment(vectype v, vectype xvec, vectype yvec, vectype zvec){
    // Convert float vectors to int 
    vector<int> txvec(xvec.begin(), xvec.end());
    vector<int> tyvec(yvec.begin(), yvec.end());
    vector<int> tzvec(zvec.begin(), zvec.end());

    vector<GradientPixelType> lvg = this->GetGradientAtIndex(txvec, tyvec, tzvec);

    float testradius = this->density;
    vectype u(3);
    float tempdis = 0;
    float orthfl = 0;
    float orthf = 0;

    vectype dis = eucdistance2center(this->x, this->y, this->z, xvec, yvec, zvec);
    assert(dis.size() == xvec.size());

    for(int n=0; n<this->density; n++)
    {
        u[0] = lvg[n][0];
        u[1] = lvg[n][1];
        u[2] = lvg[n][2];
        vecnorm(&u, v); // vector projection to u
        orthf = pow(u[0], 2) + pow(u[1], 2)+ pow(u[2], 2);
        orthf = pow(orthf, 0.5);
        orthfl = orthf * dis[n] + orthfl;
    }

    return orthfl / this->density;
}


vector<int> PressureSampler::FindPeaks()
{

    float r = 0;
    vector<int> peaks;
    
    for (int i=0; i<this->baseth.size(); i++)
    {
        r = lpressure[i];
        vector<int> neighbouridx = this->FindSphNeighbours(i);

        bool minimal = true;
        // A direction is a local minimal if there is no neighbor 
        // direction has a smaller pressure sampled
        for (int j=0; j<neighbouridx.size(); j++) 
        {
            if (lpressure[neighbouridx[j]] < r) 
            {
                minimal = false;
                break;
            }
        }

        if (minimal == true)
        {
            peaks.push_back(i);
        }
    }
    return peaks;
}


vector<int> PressureSampler::FindSphNeighbours(int i)
{
    return this->dirneighbours[i].neighbouridx; // return the neighbours of the ith dir
}


const int PressureSampler::FindDirIdx(float th, float phi){
    // baseth were presorted according to basephi; Find the start of the phi list and finding th would be faster
    int phiidx = find(this->basephi.begin(), this->basephi.end(), phi) - this->basephi.begin();
    int thidx = find(this->baseth.begin() + phiidx, this->baseth.end(), th) - this->baseth.begin();
    return (thidx == this->ndir)? -1 : thidx; // If thidx==this->ndir it means not found
}   


void PressureSampler::RandSample()
{
    vectype rvec(this->ndir, 1);
    vectype xvec(this->ndir), yvec(this->ndir), zvec(this->ndir);
    vectype cartsampledir(3);
    vectype samplex(this->density), sampley(this->density), samplez(this->density);
    sph2cart(this->baseth, this->basephi, rvec, &xvec, &yvec, &zvec);

    for (int i = 0; i < this->ndir; i++)
    {
        cartsampledir[0] = xvec[i];
        cartsampledir[1] = yvec[i];
        cartsampledir[2] = zvec[i];
        this->FindVoxel2Sample(this->baseth[i], this->basephi[i], &samplex, &sampley, &samplez, this->radius);
        this->lpressure[i] = this->Moment(cartsampledir, samplex, sampley, samplez);
    }

    this->FindPeaks();
}


// It will be called when UpdatePosition
float PressureSampler::GetRadius()
{
    // TODO;  
    this->radius = -1;
    return this->radius;
}
