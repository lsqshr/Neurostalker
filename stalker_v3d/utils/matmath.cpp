// Some C++ Implementations for the matlab classic calls
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


void sph2cart(vectype thvec, vectype phivec, vectype r, vectype * xvec, vectype * yvec, vectype * zvec ){
	vectype::iterator xitr, yitr, zitr, thitr, phiitr, ritr;
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
		 xitr++, yitr++, thitr++){
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
        
        // Constrain th within [0, 2*pi]
    	double newth = remainder(*thitr, (2.0 * M_PI));
        *thitr = newth < 0 ? (2*M_PI + newth) : newth;

        *ritr = pow(pow(*xitr, 2) + pow(*yitr, 2) + pow(*zitr, 2), 0.5);
        
        *phiitr = acos(*zitr / *ritr);
	}

}
