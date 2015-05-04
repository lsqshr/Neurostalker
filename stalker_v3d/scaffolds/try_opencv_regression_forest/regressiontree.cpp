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
    // I am tring CVrTrees ignore me
    const String filename = "data01.xml"; // The dataset used in OpenCV logistic regression sample

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

    //--------------------- 2. Set up the learning parameters 
    Ptr<RTrees> tree = RTrees::create();
    cout << "Starting Training Process"<<endl;
    //tree->setActiveVarCount(2);
    tree->setMaxDepth(10);
    tree->setMinSampleCount(1); 
    tree->setRegressionAccuracy(0.001f);
    tree->train(data, ROW_SAMPLE, labels);
    cout << "Finish Training Trees"<<endl;

    tree->save("shittytree3.xml");

}
