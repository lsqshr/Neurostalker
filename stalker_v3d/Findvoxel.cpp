/* rand example: guess the number */
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h> 
#include <iostream>
#include <vector>

using namespace std;

#define PI 3.14159265
typedef std::vector<double> vectype;
void FindVoxel2Sample(float x, float y, float z, float theta, float phi,vectype & outx,vectype & outy,vectype & outz, float radius);

int main ()
{
  /* initialize random seed: */
  srand (time(NULL));
  size_t size = 100;
  vectype  outx(size);
  vectype  outy(size);
  vectype  outz(size);

  float x, y, z;
  x = 3; y = 4; z = 5;  
  float phi = 0.8; float theta = 0.6; float radius = 5;
  FindVoxel2Sample(x, y, z, theta, phi, outx, outy, outz, radius);
    return 0;
}
    void FindVoxel2Sample(float x, float y, float z, float theta, float phi,vectype & outx,vectype & outy,vectype & outz, float radius)
  {     
      float rl; float random; float random_r; float t;
        for (int n=99; n>0; n--) 
      {  
      //random ranges from 0 to 1
      random = ((float) rand()) / (float) RAND_MAX;
      random_r = ((float) rand()) / (float) RAND_MAX;
    //  cout<<"Random between 0 and 1:"<<random<<endl;
      //assign theta phi value to the normal vector
       t = 2 * PI * random;  rl = radius * random_r;
      outx[n] = rl * cos(t) * (-sin(phi)) + radius * sin(t) * cos(theta) * cos(phi) +  x;
      outy[n] = rl * cos(t) * cos(phi) + radius * sin(t) * cos(theta) * sin(phi) +  y;
      outz[n] = rl * sin(t) * (-sin(theta)) +  z;
    /*  cout<<"single point x direction:"<<singleP[0]<<"single point y direction:"\
      <<singleP[1]<<"single point z direction:"<<singleP[2]<<endl;
    */ cout<<outx[n]<<" "<<outy[n]<<" "<<outz[n]<<endl;
      }
    return;  
  }
/*void PressureSampler::FindVoxel2Sample(float x, float y, float z, float th, float phi, vectype * outx, vectype* outy, vectype* outz){

}*/

