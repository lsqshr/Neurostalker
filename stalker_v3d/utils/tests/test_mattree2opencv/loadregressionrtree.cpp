#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/ml/ml.hpp>

#define NTRAINING_SAMPLES   100         // Number of training samples per class
#define FRAC_LINEAR_SEP     0.9f        // Fraction of samples which compose the linear separable part

using namespace std;
using namespace cv;
using namespace cv::ml;

int main(int argc, char** argv )
{
    const String filename = "../../data/mat2opencvdata.xml"; // The mock dataset produced with 'input2opencvdata.m'

    // Data for visual representation
    //--------------------- 1. Load Dataset ---------------------------------------
    Mat data, labels;
    {
        cout << "loading the dataset...";
        FileStorage f;
        if(f.open(filename, FileStorage::READ))
        {
            f["datamat"] >> data;
            f["labelsmat"] >> labels;
            f.release();
        }
        else
        {
            cerr << "file can not be opened: " << filename << endl;
            return 1;
        }
        data.convertTo(data, CV_32F);
        labels.convertTo(labels, CV_32F);
        cout << "read " << data.rows << " rows of data" << endl;
    }
    cout<<"Data loaded..."<<endl;

    //--------------------- 2. Set up the learning parameters & Load Tree from example_tree.xml produced from 'mattree2opencv.m'
    cout << "Loading Forest..."<<endl;
    Ptr<RTrees> tree = Algorithm::load<RTrees>("../../data/example_tree.xml");
    //Ptr<RTrees> tree = Algorithm::load<RTrees>("../../data/shittytree3.xml");
    cout << "Forest Loaded..."<<endl;
    //tree->train(data, ROW_SAMPLE, labels);
    cout << "Finish Training Trees"<<endl;
    cout<<"NTREEVAR:"<<tree->getActiveVarCount()<<"NMATVAR"<<data.size()<<endl;

    int nlabel = labels.size().height; // Vector is read in vertically
    //float * responses = new float[nlabel]();
    Mat responses(nlabel, 1, CV_32FC2, Scalar::all(0));
    float returned = tree->predict(data, responses, RTrees::RAW_OUTPUT);
    cout<< "Is Classifier? " << tree->isClassifier() << endl;
    cout<< "responses: " << responses.t() << returned <<endl;
    cout<< "labels: " << labels.t() << returned <<endl;

    //tree->save("shittytree3.xml");

}