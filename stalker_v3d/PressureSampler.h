#ifndef __VN_IMGPREPROCESS__
#define __VN_IMGPREPROCESS__

class PressureSampler
{
public:

    PressureSampler(int ndir, 
    	             int density,
    	             unsigned char * OriginalImg,
    	             unsigned char * BinaryImg,
    	             float * GVF,
    	             float radius = 0);
    ~PressureSampler();
	void RandRotateSph();
	void RandSample(double x, double y, double z); // Randomly sample the pressure at specific postion (x,y,z)
	float *  GetBaseTh();
	float *  GetBasePhi();
	float *  GetPeakTh();
	float *  GetPeakPhi();
	friend void test(); // For Unit-Testing

private:
	unsigned char * OriginalImg;
	unsigned char * BinaryImg;
	float * GVF;
    float radius;
	int ndir; // Number of directions
    float density; // The density of the sampled points on each sample plane
	float * baseth; // The theta values of the base vectors
	float * basephi; // The theta values of the base vectors
    float * lpressure; // The pressure sampled at each direction
    float * SampleVoxels(int * lx, int * ly, int * lz, int sz);
	void GenSph(); // Generate the base spherical directions
	void FindVoxel2Sample(int x, int y, int z, double th, double phi);
};

#endif