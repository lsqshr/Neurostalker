#include "DandelionTracer.h"
#include "utils/matmath.h"

Dandelion::Dandelion(PressureSampler sampler, Point3D position, float step, 
	                 float fromth, float fromphi, LabelImagePointer wallimg,
	                 DandelionTree* tree): 
sampler(sampler), 
position(position), 
step(step),
fromth(fromth), 
fromphi(fromphi),
wallimg(wallimg),
tree(tree)
{
	this->EstimateRadius();
}


vector<Dandelion*> Dandelion::walk()
{
	vector<Dandelion*> nextgen;
	// Get Peaks from its pressure sampler
    this->sampler.RandRotateSph();
    this->sampler.UpdatePosition(this->position.x, this->position.y, this->position.z);
    this->sampler.RandSample();
    vectype peakth = this->sampler.GetPeakTh();
    vectype peakphi = this->sampler.GetPeakTh();
    assert(peakth.size() == peakphi.size());

    // TODO: Filtering the peaks

    // Find the peak pointing to the reverse of its comming direction
    float reverseth = this->fromth + M_PI;
    float reversephi = M_PI - this->fromphi;
    int ndir = peakth.size();
    vectype reversethvec (ndir, reverseth);
    vectype reversephivec (ndir, reversephi);
    vectype cosvec = sphveccos(peakth, peakphi, reversethvec, reversephivec); // Largest cosine means the closest direction
    int fromidx = distance(cosvec.begin(), max_element(cosvec.begin(), cosvec.end()));

    // Calculate the displacement vectors for cartisian
    vectype dx (ndir);
    vectype dy (ndir);
    vectype dz (ndir);
    vectype rvec(ndir, this->step);
    sph2cart(peakth, peakphi, rvec, &dx, &dy, &dz);

    // Create a new dandelion for new directions, except its coming direction    
    // The last one in the list is itself
    for (int i=0; i<ndir; i++)
    {
        if (i == fromidx) continue;

    	// Calculate the new position
    	Point3D newposition;
    	// TODO: Hinder by the wall
    	newposition.x = dx[i] + this->position.x;
     	newposition.y = dy[i] + this->position.y;
    	newposition.z = dz[i] + this->position.z;

        if (i == ndir - 1) // Use itself
        {
            this->position = newposition;            
            this->fromth = peakth[i] + M_PI,
            this->fromphi = M_PI - peakphi[i],
            nextgen.push_back(this);
        }
        else // Create new Dandelions
        {
            Dandelion * d = new Dandelion (this->sampler, // Send a copy of its own sampler
            	         newposition, 
            	         this->step, 
			            // assign the from direction as the reversed 
            	         peakth[i] + M_PI,
            	         M_PI - peakphi[i],
            	         this->wallimg,
            	         this->tree
            	         );
            nextgen.push_back(d);
        }

        //TODO: Update Tree
    }
}


DandelionTree Dandelion::GetTree()
{
	return *(this->tree);
}


void Dandelion::EstimateRadius(){
	this->sampler.GetRadius();
}
