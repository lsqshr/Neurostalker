class PressureSampler
{
public

    PresssureSampler(int ndir, 
    	             int density,
    	             unsigned char * OriginalImg,
    	             unsigned char * BinaryImg,
    	             float * GVF,
    	             float radius = 0);
	void GenSph();
	void RandRotateSph();
	void RandSample(int x, int y, int z);
	float *  GetBaseTh();
	float *  GetBasePhi();
	float *  GetPeakTh();
	float *  GetPeakPhi();

private:
	unsigned char * OriginalImg;
	unsigned char * BinaryImg;
	float * GVF;
    float radius;	
	int ndir; // Number of directions
	float * baseth; // The theta values of the base vectors
	float * basephi; // The theta values of the base vectors
    float * lpressure; // The pressure sampled at each direction
    float * SampleVoxels(int * lx, int * ly, int * lz);
	void FindVoxel2Sample(float x, float y, float z, float th, float phi);
}