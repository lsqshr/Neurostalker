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
typedef vector<float> vectype;


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
    	*thitr = rand()/float(RAND_MAX) * 2.0 * M_PI + 0.0;
    	*phiitr = rand()/float(RAND_MAX) * M_PI + 0.0;
        *ritr = rand()/float(RAND_MAX)*100.0 + 0.0;
    }

    sph2cart(th, phi, r, &x, &y, &z);
    cart2sph(x, y, z, &outth, &outphi, &outr);

    assert(th.size() == outth.size());

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
    	*xitr = rand()/float(RAND_MAX) * 10000.0 + 0.0;
    	*yitr = rand()/float(RAND_MAX) * 10000.0 + 0.0;
        *zitr = rand()/float(RAND_MAX) * 10000.0 + 0.0;
	}

	cart2sph(x, y, z, &th, &phi, &r);
	sph2cart(th, phi, r, &outx, &outy, &outz);

	assert(vector_equal(x, outx));
	assert(vector_equal(y, outy));
	assert(vector_equal(z, outz));
}

void TestEucDistance2Center(){
	float x, y, z;
	x = y = z = 5.0;
	vectype lx, ly, lz;
	lx.push_back(2.0); // Point1
	ly.push_back(3.0); // Point1
	lz.push_back(1.0); // Point1
	lx.push_back(0.8); // Point1
	ly.push_back(2.3); // Point2
	lz.push_back(9.6); // Point3

	vectype result = eucdistance2center(x, y, z, lx, ly, lz);
	assert(result.size() == 2);
	vectype expect;
	expect.push_back(pow(29, 0.5));
	expect.push_back(pow(46.09, 0.5));
	assert(vector_equal(result, expect));
}


void TestRadius(unsigned char * inimg1d, V3DLONG * sz)
{
    //=========radius estimation begin226.719 173.996 44.2629
    cout<<"== Test Case : Testing radius estimation"<<endl;
    int location_x = 35, location_y = 15, location_z = 20;
    double thresh = 0.01;
    int testr = appradius(inimg1d, sz, thresh, location_x, location_y , location_z);
    assert(testr==3);
	cout<<"== Test case Passed"<<endl;
    //=========radius estimation end
}

void TestvecProj()
{
	cout<<"== Test Case : Testing Vecproj Then Vecnorm"<<endl;
		//initialize two vectors 
    vectype u(3, 0);
    u[0] = 3; u[1] = 4; u[2] = 5;
    vectype v(3, 0);
    v[0] = 1; v[1] = 1; v[2] = 1;    
    vectype a(3, 0);
    a[0] = 3; a[1] = 4; a[2] = 5;    
    vecnorm(u, v);
    vecproj(a, v);
    if ((u[0] == -1) && (u[1] == 0) && (u[2] == 1) && (a[0] == 4) && (a[1] == 4) && (a[2] == 4))
    {
		cout<<"== Test Case Passed"<<endl;
	}
}


void TestMatMath(){
    TestLinspace();
    TestRepmat();
    TestTranspose();
    TestSph2CartThenCart2Sph();
    TestCart2SphThenCart2Sph();
    TestvecProj();
    TestEucDistance2Center();
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
    x = 3.0; y = 4.0; z = 5.0;
    float phi = 0.8; float theta = 0.6; float radius = 5;
    p.UpdatePosition(x, y, z);
    p.FindVoxel2Sample(theta, phi, &outx, &outy, &outz, p.density);
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
    if (current_judge){cout<<"== Test Case Passed"<<endl;}

    cout<<"==== Test Case : GenSph"<<endl;
    // -- Save spheres with multiple sampling rate for visual check
    // Pls drag the csv files generated in test/testdata/*sph.csv in matlab to check 
    // whether the points were flatly distributed in a unit sphere
    int ndir = 100;
    p.SetNDir(ndir);
    vectype rvec100(p.ndir, 1);
    assert(p.baseth.size() == p.basephi.size());
    assert(p.baseth.size() < ndir);
    vectype x100(p.ndir), y100(p.ndir), z100(p.ndir);
    sph2cart(p.baseth, p.basephi, rvec100, &x100, &y100, &z100);
    savepts2csv(x100, y100, z100, "test/testdata/100sph.csv");

    ndir = 10000;
    p.SetNDir(ndir);
	vectype rvec10000(p.ndir, 1);
    assert(p.baseth.size() == p.basephi.size());
    assert(p.baseth.size() < ndir);
    vectype x10000(p.ndir), y10000(p.ndir), z10000(p.ndir);
    sph2cart(p.baseth, p.basephi, rvec10000, &x10000, &y10000, &z10000);
    savepts2csv(x10000, y10000, z10000, "test/testdata/10000sph.csv");
	cout<<"== Test Case Passed"<<endl;

    cout<<"==== Test Case : GetGradientAtIndex"<<endl;
    GradientImageType::SizeType sz = p.GVF->GetLargestPossibleRegion().GetSize();
    cout<<"GVG Size: "<<sz[0]<<","<<sz[1]<<","<<sz[2]<<endl;
    vector<int> lx, ly, lz;

    for(int i=0;i<100;i++)
    {
	    lx.push_back(rand() % sz[0]);
	    ly.push_back(rand() % sz[1]);
	    lz.push_back(rand() % sz[2]);
    }

    vector<GradientPixelType> lvg = p.GetGradientAtIndex(lx, ly, lz);
	cout<<"== Test Case Passed"<<endl;

    cout<<"==== Test Case : RandRotateSph"<<endl;
    p.RandRotateSph();
    assert(p.baseth.size() == p.ndir);
    assert(p.basephi.size() == p.ndir);
    sph2cart(p.baseth, p.basephi, rvec10000, &x10000, &y10000, &z10000);
    savepts2csv(x10000, y10000, z10000, "test/testdata/RandRotated1.csv");
    p.RandRotateSph();
    sph2cart(p.baseth, p.basephi, rvec10000, &x10000, &y10000, &z10000);
    savepts2csv(x10000, y10000, z10000, "test/testdata/RandRotated2.csv");
    p.RandRotateSph();
    sph2cart(p.baseth, p.basephi, rvec10000, &x10000, &y10000, &z10000);
    savepts2csv(x10000, y10000, z10000, "test/testdata/RandRotated3.csv");
    cout<<"== Test Case Passed"<<endl;

}
