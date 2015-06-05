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


void TestMatMath(){
    TestLinspace();
    TestRepmat();
    TestTranspose();
    TestSph2CartThenCart2Sph();
    TestCart2SphThenCart2Sph();
}


void TestPressureSampler(ImagePointer OriginalImage, GradientImagePointer GVF)
{

    PressureSampler p(60, 100, OriginalImage, GVF, 10);

    cout<<"==== Test Case : FindVoxel2Sample"<<endl;
    srand (time(NULL));
    vectype outx(p.density);
    vectype outy(p.density);
    vectype outz(p.density);

    float x, y, z;
    x = 3; y = 4; z = 5;  
    float phi = 0.8; float theta = 0.6; float radius = 5;
    p.FindVoxel2Sample(x, y, z, theta, phi, &outx, &outy, &outz, p.density);
	cout<<"== Test Case Passed"<<endl;

    cout<<"==== Test Case : GenSph"<<endl;
    // -- Save spheres with multiple sampling rate for visual check
    // Pls drag the csv files generated in test/testdata/*sph.csv in matlab to check 
    // whether the points were flatly distributed in a unit sphere
    int ndir = 100;
    p.SetNDir(ndir);
    vectype rvec100(p.ndir, 1);
    cout<<"baseth:"<<p.baseth.size()<<"\tbasephi: "<<
        p.basephi.size()<<"\trvecsize:"<<rvec100.size()<<endl;
    assert(p.baseth.size() == p.basephi.size());
    assert(p.baseth.size() < ndir);
    vectype x100(p.ndir), y100(p.ndir), z100(p.ndir);
    sph2cart(p.baseth, p.basephi, rvec100, &x100, &y100, &z100);
    savepts2csv(x100, y100, z100, "test/testdata/100sph.csv");

    ndir = 10000;
    p.SetNDir(ndir);
	vectype rvec10000(p.ndir, 1);
    cout<<"baseth:"<<p.baseth.size()<<"\tbasephi: "<<
        p.basephi.size()<<"\trvecsize:"<<rvec10000.size()<<endl;
    assert(p.baseth.size() == p.basephi.size());
    assert(p.baseth.size() < ndir);
    vectype x10000(p.ndir), y10000(p.ndir), z10000(p.ndir);
    sph2cart(p.baseth, p.basephi, rvec10000, &x10000, &y10000, &z10000);
    savepts2csv(x10000, y10000, z10000, "test/testdata/10000sph.csv");
	cout<<"== Test Case Passed"<<endl;

    cout<<"==== Test Case : GetGradientAtIndex"<<endl;
    GradientImageType::SizeType sz = p.GVF->GetLargestPossibleRegion().GetSize();
    cout<<"GVG Size: "<<sz[0]<<","<<sz[1]<<","<<sz[2]<<endl;
    for (int x=0;x<sz[0];x++)
        for (int y=0;y<sz[1];y++)
        	for (int z=0; z<sz[2];z++)
        	{
			    std::vector<float> v = p.GetGradientAtIndex(x, y, z);
			    //cout<<v[0]<<","<<v[1]<<","<<v[2]<<endl;
        	}

	cout<<"== Test Case Passed"<<endl;
}
