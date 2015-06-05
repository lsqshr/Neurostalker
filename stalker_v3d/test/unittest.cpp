#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "unittest.h"
#include <cstdlib>
#include <time.h>       /* time */
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <vector>
#include "assert.h"
#include "utils/matmath.h"
#include "PressureSampler.h"
#include "stackutil.h"


using namespace std;
typedef vector<double> vectype ;


int approx_equal(double x, double y)
{  
   return abs(x - y) <= EPSILON;
}


int array_equal(double* v1, double* v2, int sz){
	for (int i=0; i<sz; ++i)
	{
        assert(approx_equal( v1[i], v2[i] ));
	}
}


int vector_equal(vectype v1, vectype v2){
	assert(v1.size() == v2.size());
    vectype::iterator itr1, itr2;
	for ( itr1 = v1.begin(), 
		  itr2 = v2.begin();
		 itr1 != v1.end();
	     ++itr1, ++itr2)
	{
		if (!approx_equal(*itr1, *itr2)) {
            return 0;
		}
	}
	return 1;
}

void TestLinspace(){
	cout<<"== Test Case : Testing Linspace"<<endl;
	vectype outvec = linspace(0.0, 1.0, 5);
	const double expect[5] = {0.0, 1.0/4.0, 2.0/4.0, 3.0/4.0, 1.0};
	vectype expectvec ( expect, expect + sizeof expect/ sizeof expect[0] );
    assert (outvec.size() == expectvec.size());	
    assert (vector_equal(outvec, expectvec));
	cout<<"== Test Case Passed"<<endl;
}


void TestRepmat(){
	// Vertical Stretch
	cout<<"== Test Case : Testing Repmat 1"<<endl;
	const double in[5] = {0.0, 1.0, 2.0, 3.0, 4.0};
	const vectype invec (in, in + sizeof(in) / sizeof(in[0]));
	const double expect1[25] = {0.0, 0.0, 0.0, 0.0, 0.0,
								1.0, 1.0, 1.0, 1.0, 1.0, 
								2.0, 2.0, 2.0, 2.0, 2.0, 
								3.0, 3.0, 3.0, 3.0, 3.0, 
								4.0, 4.0, 4.0, 4.0, 4.0};
	const vectype expectvec1 (expect1, expect1 + sizeof(expect1) / sizeof(expect1[0]));
	vectype outvec1 = repmat1d(invec, 5, 1);
    assert(vector_equal(expectvec1, outvec1));
	cout<<"== Test Case Passed"<<endl;

	// Horizontal Stretch
	cout<<"== Test Case : Testing Repmat 2"<<endl;
	const double expect2[25] = {0.0, 1.0, 2.0, 3.0, 4.0,
								0.0, 1.0, 2.0, 3.0, 4.0, 
								0.0, 1.0, 2.0, 3.0, 4.0, 
								0.0, 1.0, 2.0, 3.0, 4.0, 
								0.0, 1.0, 2.0, 3.0, 4.0};
	const vectype expectvec2 (expect2, expect2 + sizeof(expect2) / sizeof(expect2[0]));
	vectype outvec2 = repmat1d(invec, 5, 2);
    assert(vector_equal(expectvec2, outvec2));
	cout<<"== Test Case Passed"<<endl;
    
}


void TestTranspose(){
	cout<<"== Test Case : Testing Transpose"<<endl;
	const double in[25] = {0.0, 0.0, 0.0, 0.0, 0.0,
							1.0, 1.0, 1.0, 1.0, 1.0, 
							2.0, 2.0, 2.0, 2.0, 2.0, 
							3.0, 3.0, 3.0, 3.0, 3.0, 
							4.0, 4.0, 4.0, 4.0, 4.0};
	const double expect[25] = {0.0, 1.0, 2.0, 3.0, 4.0,
								0.0, 1.0, 2.0, 3.0, 4.0, 
								0.0, 1.0, 2.0, 3.0, 4.0, 
								0.0, 1.0, 2.0, 3.0, 4.0, 
								0.0, 1.0, 2.0, 3.0, 4.0};
    const vectype invec (in, in + sizeof in / sizeof in[0]);
    const vectype expectvec (expect, expect + sizeof expect / sizeof expect[0]);
    const vectype outvec = transpose(invec, 5, 5);
    assert(vector_equal(expectvec, outvec));
	cout<<"== Test Case Passed"<<endl;
}


void TestSph2CartThenCart2Sph(){
	cout<<"== Test Case : Testing Sph2Cart Then Cart2Sph"<<endl;
	// Make random x, y, z; Convert them to Sph and convert them back
	int ntest = 100;
	srand(time(0));
	vectype th(ntest), phi(ntest), r(ntest);
	vectype outth(ntest), outphi(ntest), outr(ntest);
	vectype x(ntest), y(ntest), z(ntest);
    vectype::iterator thitr, phiitr, ritr;
    for (thitr = th.begin(), phiitr = phi.begin(), ritr = r.begin();
    	 thitr != th.end();
    	 thitr++, phiitr++, ritr++
    	)
    {
    	*thitr = rand()/double(RAND_MAX) * 2.0 * M_PI + 0.0;
    	*phiitr = rand()/double(RAND_MAX) * M_PI + 0.0;
        *ritr = rand()/double(RAND_MAX)*100.0 + 0.0;
    }

    sph2cart(th, phi, r, &x, &y, &z);
    cart2sph(x, y, z, &outth, &outphi, &outr);

    assert(th.size() == outth.size());
    for (int i=0; i<th.size(); i++) 
    {
    	if (!approx_equal(th[i], outth[i]))
	    	cout<<th[i]<<","<<outth[i]<<endl;
    }

    assert(vector_equal(th, outth));
    assert(vector_equal(phi, outphi));
    assert(vector_equal(r, outr));
	cout<<"== Test Case Passed"<<endl;

}


void TestCart2SphThenCart2Sph(){
	cout<<"== Test Case : Testing Cart2Sph Then Sph2Cart"<<endl;
	int ntest = 100;
	srand(time(0));
	vectype x(ntest), y(ntest), z(ntest);
	vectype th(ntest), phi(ntest), r(ntest);
	vectype outx(ntest), outy(ntest), outz(ntest);
	vectype::iterator xitr, yitr, zitr;
	for (xitr = x.begin(), yitr = y.begin(), zitr = z.begin();
		 xitr != x.end();
		 xitr++, yitr++, zitr++)
	{
    	*xitr = rand()/double(RAND_MAX) * 10000.0 + 0.0;
    	*yitr = rand()/double(RAND_MAX) * 10000.0 + 0.0;
        *zitr = rand()/double(RAND_MAX) * 10000.0 + 0.0;
	}

	cart2sph(x, y, z, &th, &phi, &r);
	sph2cart(th, phi, r, &outx, &outy, &outz);

	assert(vector_equal(x, outx));
	assert(vector_equal(y, outy));
	assert(vector_equal(z, outz));
	cout<<"== Test Case Passed"<<endl;

}

void TestRadius(unsigned char * inimg1d, V3DLONG * sz)
{
    //==========================================radius estimation begin226.719 173.996 44.2629
    cout<<"== Test Case : Testing radius estimation"<<endl;
    int location_x = 35, location_y = 15, location_z = 20;
    double thresh = 0.01;
    int testr = appradius(inimg1d, sz, thresh, location_x, location_y , location_z);
    //cout<<"test data1d value"<<data1d[90]<<endl;
    //cout<<"test radius value: "<<testr<<endl;
    if (testr==3){cout<<"== Test case Passed"<<endl;}
    //printf("%s", data1d[90]);
    //==========================================radius estimation end
}


void TestMatMath(){
    TestLinspace();
    TestRepmat();
    TestTranspose();
    TestSph2CartThenCart2Sph();
    TestCart2SphThenCart2Sph();
}


void TestPressureSampler(ImagePointer OriginalImage, GradientImagePointer GVF)
{

    cout<<"== Test Case : FindVoxel2Sample"<<endl;
    PressureSampler p(60, 100, OriginalImage, GVF, 10);
     // initialize random seed: 
    srand (time(NULL));
    vectype  outx(p.density);
    vectype  outy(p.density);
    vectype  outz(p.density);

    float x, y, z;
    x = 3; y = 4; z = 5;  
    float phi = 0.8; float theta = 0.6; float radius = 5;
    p.FindVoxel2Sample(x, y, z, theta, phi, &outx, &outy, &outz, p.density);
    bool current_judge = true;
    float dirx = cos(phi) * sin(theta), diry = sin(phi) * sin(theta), dirz = cos(theta);
    float firstd = dirx * (outx)[1] + diry * (outy)[1] + dirz * (outz)[1];
    for (int n=p.density; n>0; n--) 
    {  
	    float d = dirx * (outx)[n] + diry * (outy)[n] + dirz * (outz)[n];
	    float center_distance = ((outx)[n] - x) * ((outx)[n] - x) + ((outy)[n] - y) * ((outy)[n] - y)\
	                            + ((outz)[n] - z) * ((outz)[n] - z);
	    current_judge = ((center_distance <=  ((p.radius) * (p.radius)))) && current_judge && (abs(d - firstd)<=0.0001); 
	 }   
    if (current_judge){cout<<"== Test case Passed"<<endl;}
}

