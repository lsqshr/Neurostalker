#ifndef __VN_IMGPREPROCESS__
#define __VN_IMGPREPROCESS__
#include <vector>
#include "lib/ImageOperation.h"

typedef std::vector<double> vectype;

class PressureSampler
{
public:

    PressureSampler(int ndir, 
    	             int density,
    	             ImagePointer OriginalImg,
    	             GradientImagePointer GVF,
    	             float radius = 0);
    ~PressureSampler();
	void RandRotateSph();
	void RandSample(double x, double y, double z); // Randomly sample the pressure at specific postion (x,y,z)
	vectype GetBaseTh();
	vectype GetBasePhi();
	vectype GetPeakTh();
	vectype GetPeakPhi();
	friend void TestPressureSampler(ImagePointer, GradientImagePointer);

private:
	ImagePointer OriginalImg;
	GradientImagePointer GVF;
    float radius;
	int ndir; // Number of directions
    int density; // The density of the sampled points on each sample plane
	vectype baseth; // The theta values of the base vectors
	vectype basephi; // The theta values of the base vectors
    vectype lpressure; // The pressure sampled at each direction
    void SampleVoxels(int * lx, int * ly, int * lz, int sz); // Sample the distortion energy at each direction
    std::vector<float> GetGradientAtIndex(int x, int y, int z);
	void GenSph(); // Generate the base spherical directions
    void FindVoxel2Sample(float x, float y, float z, float th, float phi, vectype * outx, vectype* outy, vectype* outz, int pointrange);
};

#endif