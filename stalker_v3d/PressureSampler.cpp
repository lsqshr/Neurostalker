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
    vector<idxentrypair> phipairs(this->ndir);
    for (int i=0; i<this->ndir; i++)
    {
        phipairs[i].idx = i;
        phipairs[i].value = this->basephi[i];
    }

    // Lambda sort needs std c++11, however some of the ITK libraries do not support c++11
    //std::sort(sortidx.begin(), sortidx.end(), [&phi2sort](const int& a, const int& b){return phi2sort[a] < phi2sort[b];}); 
    std::sort(phipairs.begin(), phipairs.end()); 

    vectype tth(this->ndir);
    vectype tphi(this->ndir);

    // Permute both th and phi
    for (int i=0; i<ndir; i++)
    {
        tth[i] = this->baseth[phipairs[i].idx];
        tphi[i] = this->basephi[phipairs[i].idx];
    }

    // Sort th with the same phi value
    int thctr = 1;
    vectype::iterator thitr = tth.begin();
    int thstartidx = 0;

    for (int i=1; i<this->ndir; i++)
    {
        if (tphi[i] == tphi[i-1] && i !=this->ndir - 1)
        { 
            thctr++;
            continue; 
        }
        else
        {
            if (i == this->ndir - 1) thctr++;
            sort(thitr, thitr + thctr);
            thstartidx = i - thctr + 1;
            // head and tail
            thitr += thctr;
            thctr = 1;
        }
    }

    // Loop through all directions horizontally
    // Add only the left and right neighbours to itself
    for (int i=1; i<this->ndir; i++)
    {
        if (tphi[i] == tphi[i-1] && i !=this->ndir - 1)
        { 
            thctr++;
            this->dirneighbours[phipairs[i].idx].neighbouridx.push_back(phipairs[i-1].idx);
            this->dirneighbours[phipairs[i-1].idx].neighbouridx.push_back(phipairs[i].idx);
            continue; 
        }
        else
        {
            if (i == this->ndir - 1) thctr++;
            thstartidx = i - thctr + 1;
            // head and tail
            this->dirneighbours[phipairs[i].idx].neighbouridx.push_back(phipairs[thstartidx].idx);
            this->dirneighbours[phipairs[thstartidx].idx].neighbouridx.push_back(phipairs[i].idx);
            thitr += thctr;
            thctr = 1;
        }
    }

    // -- Sort all phi according to th for neighbour finding -- reversed for previous section
    vector<idxentrypair> thpairs(this->ndir);
    for (int i=0; i<this->ndir; i++)
    {
        thpairs[i].idx = i;
        thpairs[i].value = this->baseth[i];
    }

    std::sort(thpairs.begin(), thpairs.end()); 

    // Permute both th and phi
    for (int i=0; i<ndir; i++)
    {
        tth[i] = this->baseth[thpairs[i].idx];
        tphi[i] = this->basephi[thpairs[i].idx];
    }

    // Sort th with the same phi value
    int phictr = 1;
    int phistartidx = 1;
    vectype::iterator phiitr = tphi.begin();

    for (int i=1; i<this->ndir; i++)
    {
        if (tth[i] == tth[i-1] && i !=this->ndir - 1)
        { 
            phictr++;
            continue; 
        }
        else
        {
            if (i == this->ndir - 1) phictr++;
            sort(phiitr, phiitr + phictr);
            phistartidx = i - phictr + 1;
            // head and tail
            thitr += thctr;
            thctr = 1;
        }
    }

    // Loop through all directions vertically, add neighbours
    for (int i=1; i<this->ndir; i++)
    {
        if (tth[i] == tth[i-1] && i !=this->ndir - 1)
        { 
            phictr++;
            // Add the left&right neighbours of its upper neighbour to itself
            if (this->dirneighbours[thpairs[i-1].idx].neighbouridx.size() >=2) 
            {
                this->dirneighbours[thpairs[i].idx].neighbouridx.push_back(
                                                                    this->dirneighbours[thpairs[i-1].idx].neighbouridx[0]
                                                                    );
                this->dirneighbours[thpairs[i].idx].neighbouridx.push_back(
                                                                    this->dirneighbours[thpairs[i-1].idx].neighbouridx[1]
                                                                    );
            }

            this->dirneighbours[thpairs[i].idx].neighbouridx.push_back(thpairs[i-1].idx);// Add its upper neighbour
            this->dirneighbours[thpairs[i-1].idx].neighbouridx.push_back(thpairs[i].idx); // Add itself to its upper neighbour

            // Add its left and right neighbours to its upper neighbour
            if (this->dirneighbours[thpairs[i].idx].neighbouridx.size() >=2) 
            {
                this->dirneighbours[thpairs[i-1].idx].neighbouridx.push_back(
                                                                    this->dirneighbours[thpairs[i].idx].neighbouridx[0]
                                                                    );
                this->dirneighbours[thpairs[i-1].idx].neighbouridx.push_back(
                                                                    this->dirneighbours[thpairs[i].idx].neighbouridx[1]
                                                                    );
            }

            continue; 
        }
        else
        {
            if (i == this->ndir - 1) phictr++;
            phistartidx = i - phictr + 1;
            // head and tail
            this->dirneighbours[thpairs[i].idx].neighbouridx.push_back(phistartidx);
            this->dirneighbours[thpairs[thstartidx].idx].neighbouridx.push_back(i);
            thitr += thctr;
            thctr = 1;
        }
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


vector<int> PressureSampler::FindPeaks()
{
    // The base sph before Findpeaks
    for (int i=0; i<this->ndir; i++)
    {
        cout<<i<<": "<<this->baseth[i]<<"\t"<<this->basephi[i]<<endl;
    }

    float r = 0;
    vector<int> peaks;
    
    for (int i=0; i<this->baseth.size(); i++)
    {
        r = lpressure[i];
        vector<int> neighbouridx = this->FindSphNeighbours(i);

        // DEBUG:
        cout<<"For "<<this->baseth[i]<<" "<<this->basephi[i]<<endl;
        cout<<neighbouridx.size()<<" neighbors found..."<<endl;        
        for(int j=0; j<neighbouridx.size(); j++)
        {
            cout<<"neighbour "<<j<<": "<<this->baseth[neighbouridx[j]]<<","<<this->basephi[neighbouridx[j]]<<"\t";
        }
        cout<<endl;

        bool minimal = true;
        // A direction is a local minimal if there is no neighbor 
        // direction has a smaller pressure sampled
        for (int j=0; j<neighbouridx.size(); j++) 
        {
            if (lpressure[neighbouridx[i]] < r) 
            {
                minimal = false;
                break;
            }
            cout<<"checking minimal at "<<i<<endl;
        }

        if (minimal == true)
        {
            cout<<"Found minimal at "<<i<<endl;
            peaks.push_back(i);
        }
        else
        {
            cout<<i<<" is not a minimal"<<endl;
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