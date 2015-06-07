#include <iostream>     
#include <ctime>        
#include <algorithm>    
#include <vector>       

using namespace std;
// this script is used for projecting one vector
//along another vector  

void vecproj(vector<double>& u, vector<double>& v);
void vecnorm(vector<double>& u, vector<double>& v);

int main()
{
	//initialize two vectors 
    vector<double> u(3, 0);
    u[0] = 3; u[1] = 4; u[2] = 5;
    vector<double> v(3, 0);
    v[0] = 1; v[1] = 1; v[2] = 1;    
    vecnorm(u, v);
	cout<<u[0]<<u[1]<<u[2]<<endl;
    vecproj(u, v);
	cout<<u[0]<<u[1]<<u[2]<<endl;
    return 0;      
}

void vecproj(vector<double>& u, vector<double>& v)
{
	// Projv(u) = [ (u•v)/(v•v) ] · v project u ulong v
	double udotv = u[0] * v[0] + u[1] * v[1] + u[2] * v[2]; 
	double vdotv = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	u[0] = udotv / vdotv * v[0];
	u[1] = udotv / vdotv * v[1];
	u[2] = udotv / vdotv * v[2];
}

void vecnorm(vector<double>& u, vector<double>& v)
{
	// Projv(u) = [ (u•v)/(v•v) ] · v project u ulong v
	double udotv = u[0] * v[0] + u[1] * v[1] + u[2] * v[2]; 
	double vdotv = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	u[0] = u[0] - udotv / vdotv * v[0];
	u[1] = u[1] - udotv / vdotv * v[1];
	u[2] = u[2] - udotv / vdotv * v[2];
}