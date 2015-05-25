// q_EigenVectors3D.cpp
// by Lei Qu
// 2015-03-08

#include <stdio.h>
#include "math.h"
#include <vector>
#include "EigenDecomposition3.h"

//calculates the eigen values and vectors of the 3D hessian images
//        |Ixx Ixy Ixz|
//hessian=|Ixy Iyy Iyz|
//        |Ixz Iyz Izz|
//eigenvalue: eigval3>eigval2>eigval1
bool q_eigenvectors3D(float *p_Ixx,float *p_Ixy,float *p_Ixz,float *p_Iyy,float *p_Iyz,float *p_Izz,
					  long sz_img[4],
					  float *&p_eigval3,float *&p_eigval2,float *&p_eigval1,
					  float *&p_eigvec3x,float *&p_eigvec3y,float *&p_eigvec3z,
					  float *&p_eigvec2x,float *&p_eigvec2y,float *&p_eigvec2z,
					  float *&p_eigvec1x,float *&p_eigvec1y,float *&p_eigvec1z)
{
	if(sz_img[0]<=0 || sz_img[1]<=0 || sz_img[2]<=0 || sz_img[3]<=0)
	{
		printf("ERROR: Invalid input image size!\n");
		return false;
	}
	if(sz_img[3]>1)
	{
		printf("ERROR: Input image should be single channel!\n");
		return false;
	}
	if(p_Ixx==0 || p_Ixy==0 ||p_Ixz==0 ||p_Iyy==0 ||p_Iyz==0 ||p_Izz==0)
	{
		printf("ERROR: Invalid input 3D hessian image pointer!\n");
		return false;
	}
	if(p_eigval3 || p_eigval2 || p_eigval1 || 
	   p_eigvec3x ||p_eigvec3y ||p_eigvec3z ||
	   p_eigvec2x ||p_eigvec2y ||p_eigvec2z ||
	   p_eigvec1x ||p_eigvec1y ||p_eigvec1z)
	{
		printf("WARNNING: Output image pointer is not NULL, original data will be cleared!\n");
		if(p_eigval3) 	{delete []p_eigval3;		p_eigval3=0;}
		if(p_eigval2) 	{delete []p_eigval2;		p_eigval2=0;}
		if(p_eigval1) 	{delete []p_eigval1;		p_eigval1=0;}
		if(p_eigvec3x) 	{delete []p_eigvec3x;		p_eigvec3x=0;}
		if(p_eigvec3y) 	{delete []p_eigvec3y;		p_eigvec3y=0;}
		if(p_eigvec3z) 	{delete []p_eigvec3z;		p_eigvec3z=0;}
		if(p_eigvec2x) 	{delete []p_eigvec2x;		p_eigvec2x=0;}
		if(p_eigvec2y) 	{delete []p_eigvec2y;		p_eigvec2y=0;}
		if(p_eigvec2z) 	{delete []p_eigvec2z;		p_eigvec2z=0;}
		if(p_eigvec1x) 	{delete []p_eigvec1x;		p_eigvec1x=0;}
		if(p_eigvec1y) 	{delete []p_eigvec1y;		p_eigvec1y=0;}
		if(p_eigvec1z) 	{delete []p_eigvec1z;		p_eigvec1z=0;}
	}

	//allocate memory
	long l_npixels=sz_img[0]*sz_img[1]*sz_img[2]*sz_img[3];
	p_eigval3=new(std::nothrow) float[l_npixels]();	p_eigval2=new(std::nothrow) float[l_npixels](); p_eigval1=new(std::nothrow) float[l_npixels]();
	p_eigvec3x=new(std::nothrow) float[l_npixels]();	p_eigvec3y=new(std::nothrow) float[l_npixels]();p_eigvec3z=new(std::nothrow) float[l_npixels]();
	p_eigvec2x=new(std::nothrow) float[l_npixels]();	p_eigvec2y=new(std::nothrow) float[l_npixels]();p_eigvec2z=new(std::nothrow) float[l_npixels]();
	p_eigvec1x=new(std::nothrow) float[l_npixels]();	p_eigvec1y=new(std::nothrow) float[l_npixels]();p_eigvec1z=new(std::nothrow) float[l_npixels]();
	if(!p_eigval3 ||!p_eigval2 ||!p_eigval3 ||
	   !p_eigvec3x ||!p_eigvec3y ||!p_eigvec3z ||
	   !p_eigvec2x ||!p_eigvec2y ||!p_eigvec2z ||
	   !p_eigvec1x ||!p_eigvec1y ||!p_eigvec1z)

	{
		printf("ERROR: Fail to allocate memory for output images!\n");
		if(p_eigval3) 	{delete []p_eigval3;		p_eigval3=0;}
		if(p_eigval2) 	{delete []p_eigval2;		p_eigval2=0;}
		if(p_eigval1) 	{delete []p_eigval1;		p_eigval1=0;}
		if(p_eigvec3x) 	{delete []p_eigvec3x;		p_eigvec3x=0;}
		if(p_eigvec3y) 	{delete []p_eigvec3y;		p_eigvec3y=0;}
		if(p_eigvec3z) 	{delete []p_eigvec3z;		p_eigvec3z=0;}
		if(p_eigvec2x) 	{delete []p_eigvec2x;		p_eigvec2x=0;}
		if(p_eigvec2y) 	{delete []p_eigvec2y;		p_eigvec2y=0;}
		if(p_eigvec2z) 	{delete []p_eigvec2z;		p_eigvec2z=0;}
		if(p_eigvec1x) 	{delete []p_eigvec1x;		p_eigvec1x=0;}
		if(p_eigvec1y) 	{delete []p_eigvec1y;		p_eigvec1y=0;}
		if(p_eigvec1z) 	{delete []p_eigvec1z;		p_eigvec1z=0;}
		return false;
	}

	//decompose pixel by pixel
	double Mat[3][3];
	double eigvec[3][3];
	double eigval[3];
	for(long i=0; i<l_npixels; i++) 
	{
		Mat[0][0]=(double)p_Ixx[i]; Mat[0][1]=(double)p_Ixy[i]; Mat[0][2]=(double)p_Ixz[i];
		Mat[1][0]=(double)p_Ixy[i]; Mat[1][1]=(double)p_Iyy[i]; Mat[1][2]=(double)p_Iyz[i];
		Mat[2][0]=(double)p_Ixz[i]; Mat[2][1]=(double)p_Iyz[i]; Mat[2][2]=(double)p_Izz[i];
		eigen_decomposition(Mat, eigvec, eigval);

		p_eigval3[i]=(float)eigval[2]; 
		p_eigval2[i]=(float)eigval[1]; 
		p_eigval1[i]=(float)eigval[0];

		p_eigvec3x[i]=(float)eigvec[0][2]; p_eigvec3y[i]=(float)eigvec[1][2]; p_eigvec3z[i]=(float)eigvec[2][2];
		p_eigvec2x[i]=(float)eigvec[0][1]; p_eigvec2y[i]=(float)eigvec[1][1]; p_eigvec2z[i]=(float)eigvec[2][1];
		p_eigvec1x[i]=(float)eigvec[0][0]; p_eigvec1y[i]=(float)eigvec[1][0]; p_eigvec1z[i]=(float)eigvec[2][0];
	}

	return true;
}

bool q_eigenvectors3D(float *p_Ixx,float *p_Ixy,float *p_Ixz,float *p_Iyy,float *p_Iyz,float *p_Izz,
					  long sz_img[4],unsigned char *p_mask,
					  float *&p_eigval3,float *&p_eigval2,float *&p_eigval1,
					  float *&p_eigvec3x,float *&p_eigvec3y,float *&p_eigvec3z,
					  float *&p_eigvec2x,float *&p_eigvec2y,float *&p_eigvec2z,
					  float *&p_eigvec1x,float *&p_eigvec1y,float *&p_eigvec1z)
{
	if(sz_img[0]<=0 || sz_img[1]<=0 || sz_img[2]<=0 || sz_img[3]<=0)
	{
		printf("ERROR: Invalid input image size!\n");
		return false;
	}
	if(sz_img[3]>1)
	{
		printf("ERROR: Input image should be single channel!\n");
		return false;
	}
	if(p_Ixx==0 || p_Ixy==0 ||p_Ixz==0 ||p_Iyy==0 ||p_Iyz==0 ||p_Izz==0 || p_mask==0)
	{
		printf("ERROR: Invalid input 3D hessian image pointer!\n");
		return false;
	}
	if(p_eigval3 || p_eigval2 || p_eigval1 || 
		p_eigvec3x ||p_eigvec3y ||p_eigvec3z ||
		p_eigvec2x ||p_eigvec2y ||p_eigvec2z ||
		p_eigvec1x ||p_eigvec1y ||p_eigvec1z)
	{
		printf("WARNNING: Output image pointer is not NULL, original data will be cleared!\n");
		if(p_eigval3) 	{delete []p_eigval3;		p_eigval3=0;}
		if(p_eigval2) 	{delete []p_eigval2;		p_eigval2=0;}
		if(p_eigval1) 	{delete []p_eigval1;		p_eigval1=0;}
		if(p_eigvec3x) 	{delete []p_eigvec3x;		p_eigvec3x=0;}
		if(p_eigvec3y) 	{delete []p_eigvec3y;		p_eigvec3y=0;}
		if(p_eigvec3z) 	{delete []p_eigvec3z;		p_eigvec3z=0;}
		if(p_eigvec2x) 	{delete []p_eigvec2x;		p_eigvec2x=0;}
		if(p_eigvec2y) 	{delete []p_eigvec2y;		p_eigvec2y=0;}
		if(p_eigvec2z) 	{delete []p_eigvec2z;		p_eigvec2z=0;}
		if(p_eigvec1x) 	{delete []p_eigvec1x;		p_eigvec1x=0;}
		if(p_eigvec1y) 	{delete []p_eigvec1y;		p_eigvec1y=0;}
		if(p_eigvec1z) 	{delete []p_eigvec1z;		p_eigvec1z=0;}
	}

	//allocate memory
	long l_npixels=sz_img[0]*sz_img[1]*sz_img[2]*sz_img[3];
	p_eigval3=new(std::nothrow) float[l_npixels]();	p_eigval2=new(std::nothrow) float[l_npixels](); p_eigval1=new(std::nothrow) float[l_npixels]();
	p_eigvec3x=new(std::nothrow) float[l_npixels]();	p_eigvec3y=new(std::nothrow) float[l_npixels]();p_eigvec3z=new(std::nothrow) float[l_npixels]();
	p_eigvec2x=new(std::nothrow) float[l_npixels]();	p_eigvec2y=new(std::nothrow) float[l_npixels]();p_eigvec2z=new(std::nothrow) float[l_npixels]();
	p_eigvec1x=new(std::nothrow) float[l_npixels]();	p_eigvec1y=new(std::nothrow) float[l_npixels]();p_eigvec1z=new(std::nothrow) float[l_npixels]();
	if(!p_eigval3 ||!p_eigval2 ||!p_eigval3 ||
		!p_eigvec3x ||!p_eigvec3y ||!p_eigvec3z ||
		!p_eigvec2x ||!p_eigvec2y ||!p_eigvec2z ||
		!p_eigvec1x ||!p_eigvec1y ||!p_eigvec1z)

	{
		printf("ERROR: Fail to allocate memory for output images!\n");
		if(p_eigval3) 	{delete []p_eigval3;		p_eigval3=0;}
		if(p_eigval2) 	{delete []p_eigval2;		p_eigval2=0;}
		if(p_eigval1) 	{delete []p_eigval1;		p_eigval1=0;}
		if(p_eigvec3x) 	{delete []p_eigvec3x;		p_eigvec3x=0;}
		if(p_eigvec3y) 	{delete []p_eigvec3y;		p_eigvec3y=0;}
		if(p_eigvec3z) 	{delete []p_eigvec3z;		p_eigvec3z=0;}
		if(p_eigvec2x) 	{delete []p_eigvec2x;		p_eigvec2x=0;}
		if(p_eigvec2y) 	{delete []p_eigvec2y;		p_eigvec2y=0;}
		if(p_eigvec2z) 	{delete []p_eigvec2z;		p_eigvec2z=0;}
		if(p_eigvec1x) 	{delete []p_eigvec1x;		p_eigvec1x=0;}
		if(p_eigvec1y) 	{delete []p_eigvec1y;		p_eigvec1y=0;}
		if(p_eigvec1z) 	{delete []p_eigvec1z;		p_eigvec1z=0;}
		return false;
	}

	//decompose pixel by pixel
	double Mat[3][3];
	double eigvec[3][3];
	double eigval[3];
	for(long i=0; i<l_npixels; i++) 
	{
		if(p_mask[i]>125)
		{
			Mat[0][0]=(double)p_Ixx[i]; Mat[0][1]=(double)p_Ixy[i]; Mat[0][2]=(double)p_Ixz[i];
			Mat[1][0]=(double)p_Ixy[i]; Mat[1][1]=(double)p_Iyy[i]; Mat[1][2]=(double)p_Iyz[i];
			Mat[2][0]=(double)p_Ixz[i]; Mat[2][1]=(double)p_Iyz[i]; Mat[2][2]=(double)p_Izz[i];
			eigen_decomposition(Mat, eigvec, eigval);

			p_eigval3[i]=(float)eigval[2]; 
			p_eigval2[i]=(float)eigval[1]; 
			p_eigval1[i]=(float)eigval[0];

			p_eigvec3x[i]=(float)eigvec[0][2]; p_eigvec3y[i]=(float)eigvec[1][2]; p_eigvec3z[i]=(float)eigvec[2][2];
			p_eigvec2x[i]=(float)eigvec[0][1]; p_eigvec2y[i]=(float)eigvec[1][1]; p_eigvec2z[i]=(float)eigvec[2][1];
			p_eigvec1x[i]=(float)eigvec[0][0]; p_eigvec1y[i]=(float)eigvec[1][0]; p_eigvec1z[i]=(float)eigvec[2][0];
		}
		else
		{
			p_eigval3[i]=0; 
			p_eigval2[i]=0; 
			p_eigval1[i]=0;

			p_eigvec3x[i]=0; p_eigvec3y[i]=0; p_eigvec3z[i]=0;
			p_eigvec2x[i]=0; p_eigvec2y[i]=0; p_eigvec2z[i]=0;
			p_eigvec1x[i]=0; p_eigvec1y[i]=0; p_eigvec1z[i]=0;
		}
	}

	return true;
}
