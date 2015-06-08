#include <iostream>     
#include <ctime>        
#include <algorithm>    
#include <vector>       

using namespace std;
// this script is used for projecting one vector
//along another vector

typedef vector<double> vectype;  

void vecproj(vectype& u, vectype& v);
void vecnorm(vectype& u, vectype& v);
void Testvecproj();

int main()
{
	Testvecproj();
    return 0;      
}

void vecproj(vectype& u, vectype& v)
{
	// Projv(u) = [ (u•v)/(v•v) ] · v project u ulong v
	double udotv = u[0] * v[0] + u[1] * v[1] + u[2] * v[2]; 
	double vdotv = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	u[0] = udotv / vdotv * v[0];
	u[1] = udotv / vdotv * v[1];
	u[2] = udotv / vdotv * v[2];
}

void vecnorm(vectype& u, vectype& v)
{
	// Projv(u) = [ (u•v)/(v•v) ] · v project u ulong v
	double udotv = u[0] * v[0] + u[1] * v[1] + u[2] * v[2]; 
	double vdotv = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	u[0] = u[0] - udotv / vdotv * v[0];
	u[1] = u[1] - udotv / vdotv * v[1];
	u[2] = u[2] - udotv / vdotv * v[2];
}

void Testvecproj(){
	cout<<"== Test Case : Testing vecproj Then vecnorm"<<endl;
		//initialize two vectors 
    vectype u(3, 0);
    u[0] = 3; u[1] = 4; u[2] = 5;
    vectype v(3, 0);
    v[0] = 1; v[1] = 1; v[2] = 1;    
    vectype a(3, 0);
    a[0] = 3; a[1] = 4; a[2] = 5;    
    vecnorm(u, v);
    vecproj(a, v);
    cout<<a[0]<<endl;
    if ((u[0] == -1) && (u[1] == 0) && (u[2] == 1) && (a[0] == 4) && (a[1] == 4) && (a[2] == 4))
    {
		cout<<"Test Case Passed"<<endl;
	}
}