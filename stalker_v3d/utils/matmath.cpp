// Some C++ Implementations for the matlab classic calls
#include <iostream>
#include <fstream>
#include "matmath.h"
#include "assert.h"
#include <cmath>

using namespace std;

vectype linspace(const double a, const double b, const int n) {
// Equals to linspace in matlab
    vectype outvec (n);
    double step = (b-a) / (n-1);
    double t = a;
    vectype::iterator outitr = outvec.begin();
    while(t <= b && outitr != outvec.end()) {
        *outitr = t;
        outitr++;
        t += step;           // could recode to better handle rounding errors
    }
    return outvec;
}


vectype repmat1d(vectype invec, int repeat, int dim){
	assert(invec.size() != 0);
	const int nvar = repeat * invec.size();
	vectype outvec (nvar);
    vectype::iterator outitr = outvec.begin();
	int out_szrow, out_szcol;

	switch (dim){
	    case 1:
	        {
	        out_szrow = repeat;
	        out_szcol = invec.size();
			// Assume the caller has allocated the memory for outarray
			for (int col = 0; col < out_szcol; col++)
			    for(int row = 0; row < out_szrow; row++, outitr++)
			    {
		            *outitr = invec[col];
			    }
		    break;
			}
	    case 2:
	        {
	        out_szrow = invec.size();
	        out_szcol = repeat;
			// Assume the caller has allocated the memory for outarray
			for (int col = 0; col < out_szcol; col++)
			    for(int row = 0; row < out_szrow; row++, outitr++)
			    {
		            *outitr = invec[row];
			    }
		    break;
		    }
	    default: throw 1; break;
	}

	return outvec;
}


vectype transpose(const vectype invec, const int N, const int M) {
    vectype outvec(invec.size());
    vectype::iterator outitr = outvec.begin();
    int i,j;

	for (int n=0; n<M*N; n++, outitr++)
	{
        i = n/N;
        j = n%N;
        *outitr = invec[M*j + i];
    }

    return outvec;
}


void sph2cart(vectype thvec, vectype phivec, vectype rvec, vectype * xvec, vectype * yvec, vectype * zvec ){
	assert(thvec.size() == phivec.size() && phivec.size() == rvec.size());
	assert(thvec.size() == xvec->size() && thvec.size() == yvec->size() && thvec.size() == zvec->size());
	vectype::iterator xitr, yitr, zitr, thitr, phiitr, ritr;

	for (xitr = xvec->begin(), 
		 yitr = yvec->begin(), 
		 zitr = zvec->begin(), 
		 thitr = thvec.begin(),
		 phiitr = phivec.begin(),
		 ritr = rvec.begin();
		 thitr != thvec.end();
		 xitr++, yitr++, zitr++,
		 thitr++, phiitr++, ritr++)
	{
	    *xitr = *ritr * cos(*thitr)	* sin(*phiitr);
	    *yitr = *ritr * sin(*thitr)	* sin(*phiitr);
	    *zitr = *ritr * cos(*phiitr);
	}
}


void cart2sph(vectype xvec, vectype yvec, vectype zvec, vectype* thvec, vectype* phivec, vectype* rvec ){
// Convert cartisian coordinates to spherical coordinates
// Using the mathmatician definition of the S^2
//                                2*pi > th > 0
//                                pi > phi > 0
// Ref: http://en.wikipedia.org/wiki/List_of_common_coordinate_transformations
// Assume thvec, phivec, rvec are initialsed before callling this function

	assert(xvec.size() == yvec.size() && yvec.size() == zvec.size());
	assert(xvec.size() == thvec->size() && xvec.size() == phivec->size() && xvec.size() == rvec->size());
	vectype::iterator xitr, yitr, zitr, thitr, phiitr, ritr;

	for (xitr = xvec.begin(), 
		 yitr = yvec.begin(), 
		 zitr = zvec.begin(), 
		 thitr = thvec->begin(),
		 phiitr = phivec->begin(),
		 ritr = rvec->begin();
		 xitr != xvec.end();
		 xitr++, yitr++, zitr++,
		 thitr++, phiitr++, ritr++)
	{
	    if (*xitr == 0)	
	    {
	    	if (*yitr > 0) *thitr = M_PI;
	    	else if (*yitr < 0) *thitr = 3 * M_PI / 2;
    		else *thitr = 0;
	    }
	    else
	    {
	        *thitr = atan(abs(*yitr / *xitr));
	    }
        
        // Put th to the right domain (atan only outputs [0, PI/2])
        if (*xitr < 0.0 && *yitr > 0.0)
            *thitr = M_PI - *thitr; // --  QII
        else if (*xitr < 0.0 && *yitr < 0.0)
            *thitr += M_PI; // --  QIII
        else if (*xitr > 0.0 && *yitr < 0.0)  // -- QIV
        	*thitr = 2.0 * M_PI - *thitr;


        *ritr = pow(pow(*xitr, 2) + pow(*yitr, 2) + pow(*zitr, 2), 0.5);
        
        *phiitr = acos(*zitr / *ritr);
	}

}


void savepts2csv(vectype a, vectype b, vectype c, const char* filename){
// Save a list of points to a text file
// Each point occupies a line
// Since three coordinates are saved, this function works both for Cartisian and Spherical 
	ofstream f;
	f.open(filename);
	assert(a.size() == b.size() && b.size() == c.size());

	for (int i = 0; i < a.size(); i++)
	{
	   f<<a[i]<<","<<b[i]<<","<<c[i]<<endl;
	}

	f.close();
}


vectype eucdistance2center(float x, float y, float z, vectype lx, vectype ly, vectype lz)
{
	assert(lx.size() == ly.size() && ly.size() == lz.size());

    vectype vd(lx.size());
    
    for (int i = 0; i < lx.size(); i++)	
    {
        vd[i] = pow( pow(lx[i] - x, 2) + pow(ly[i] - y, 2) + pow(lz[i] - z, 2), 0.5);
    }

    return vd;
}
