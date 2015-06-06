#ifndef __VN_IMGPREPROCESS__
#define __VN_IMGPREPROCESS__
#include <vector>
#include "lib/ImageOperation.h"

typedef std::vector<float> vectype;
using namespace std;

class PressureSampler
{
public:

    PressureSampler(int ndir, 
    	             int density,
    	             ImagePointer OriginalImg,
    	             GradientImagePointer GVF,
    	             float radius = 10);
    ~PressureSampler();
	void RandRotateSph();
	void RandSample(float x, float y, float z); // Randomly sample the pressure at specific postion (x,y,z)
	void SetNDir(int ndir);
	vectype GetBaseTh();
	vectype GetBasePhi();
	vectype GetPeakTh();
	vectype GetPeakPhi();
	void UpdatePosition(float x, float y, float z);
	friend void TestPressureSampler(ImagePointer, GradientImagePointer);

private:
	ImagePointer OriginalImg;
	GradientImagePointer GVF;
    float radius;
    float x, y, z;// position
	int ndir; // Number of directions
    int density; // The density of the sampled points on each sample plane
	vectype baseth; // The theta values of the base vectors
	vectype basephi; // The theta values of the base vectors
    vectype lpressure; // The pressure sampled at each direction
    void SampleVoxels(vectype, vectype, vectype); // Sample the distortion energy at each direction
    vector<GradientPixelType> GetGradientAtIndex(vector<int> x, vector<int> y, vector<int> z);
	void GenSph(); // Generate the base spherical directions
    void FindVoxel2Sample(float th,
                          float phi, vectype * outx, vectype* outy,
                          vectype* outz, int pointrange);
};

#endif