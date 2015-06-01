// Some C++ Implementations for the matlab classic calls


vector<double> linspace(double a, double b, int n) {
// Equals to linspace in matlab
    vector<double> array;
    double step = (b-a) / (n-1);

    while(a <= b) {
        array.push_back(a);
        a += step;           // could recode to better handle rounding errors
    }
    return array;
}

void repmat1d(vector<double> m, int repeat, int dim, double *outm){
	double* invec = new double[m.size()];
	int out_szrow, out_szcol;

	for (std::vector<double>::iterator it = myvector.begin(), int i = 0; it != myvector.end(); ++it, ++i)
	    1dvec[i] = *it;
    
	swith (dim){
	    case 1:
	        out_szrow = repeat;
	        out_szcol = m.size();
			int loc = 0;
			// Assume the caller has allocated the memory for outm
			for (int col = 0; col < out_szcol; col++)
			    for(int row = 0; row < out_szrow; row++)
			    {
		            int loc = col *  out_szrow + row;
		            outm[loc] = invec[col];
			    }
	    }break;
	    case 2:
	    {
	        out_szrow = m.size();
	        out_szcol = repeat;
			int loc = 0;
			// Assume the caller has allocated the memory for outm
			for (int col = 0; col < out_szcol; col++)
			    for(int row = 0; row < out_szrow; row++)
			    {
		            int loc = col *  out_szrow + row;
		            outm[loc] = invec[row];
			    }
	    }break;
	    default: throw 1; break;
	}

    delete [] invec;
}