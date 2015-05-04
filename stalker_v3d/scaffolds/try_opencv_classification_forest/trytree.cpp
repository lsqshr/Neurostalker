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

    // Data for visual representation
    const int WIDTH = 512, HEIGHT = 512;
    Mat I = Mat::zeros(HEIGHT, WIDTH, CV_8UC3);

     //--------------------- 1. Set up training data randomly ---------------------------------------
    Mat trainData(2*NTRAINING_SAMPLES, 2, CV_32FC1);
    Mat labels   (2*NTRAINING_SAMPLES, 1, CV_32SC1);

    RNG rng(100); // Random value generation class

    // Set up the linearly separable part of the training data
    int nLinearSamples = (int) (FRAC_LINEAR_SEP * NTRAINING_SAMPLES);

    //! [setup1]
    // Generate random points for the class 1
    Mat trainClass = trainData.rowRange(0, nLinearSamples);
    // The x coordinate of the points is in [0, 0.4)
    Mat c = trainClass.colRange(0, 1);
    rng.fill(c, RNG::UNIFORM, Scalar(1), Scalar(0.4 * WIDTH));
    // The y coordinate of the points is in [0, 1)
    c = trainClass.colRange(1,2);
    rng.fill(c, RNG::UNIFORM, Scalar(1), Scalar(HEIGHT));

    // Generate random points for the class 2
    trainClass = trainData.rowRange(2*NTRAINING_SAMPLES-nLinearSamples, 2*NTRAINING_SAMPLES);
    // The x coordinate of the points is in [0.6, 1]
    c = trainClass.colRange(0 , 1);
    rng.fill(c, RNG::UNIFORM, Scalar(0.6*WIDTH), Scalar(WIDTH));
    // The y coordinate of the points is in [0, 1)
    c = trainClass.colRange(1,2);
    rng.fill(c, RNG::UNIFORM, Scalar(1), Scalar(HEIGHT));

    //------------------ Set up the non-linearly separable part of the training data ---------------
    //! [setup2]
    // Generate random points for the classes 1 and 2
    trainClass = trainData.rowRange(  nLinearSamples, 2*NTRAINING_SAMPLES-nLinearSamples);
    // The x coordinate of the points is in [0.4, 0.6)
    c = trainClass.colRange(0,1);
    rng.fill(c, RNG::UNIFORM, Scalar(0.4*WIDTH), Scalar(0.6*WIDTH));
    // The y coordinate of the points is in [0, 1)
    c = trainClass.colRange(1,2);
    rng.fill(c, RNG::UNIFORM, Scalar(1), Scalar(HEIGHT));
    //! [setup2]
    //------------------------- Set up the labels for the classes ---------------------------------
    labels.rowRange(0, NTRAINING_SAMPLES).setTo(1);  // Class 1
    labels.rowRange(NTRAINING_SAMPLES, 2*NTRAINING_SAMPLES).setTo(2);  // Class 2

    //--------------------- 2. Set up the support vector machine parameters 
    Ptr<RTrees> tree = RTrees::create();
    cout << "Starting Training Process"<<endl;
    tree->setActiveVarCount(2);
    tree->setMaxDepth(10);
    tree->setMinSampleCount(10); 
    tree->setRegressionAccuracy(1);

    tree->train(trainData, ROW_SAMPLE, labels);
    cout << "Finish Training Trees"<<endl;

    //! [show]
    Vec3b green(0,100,0), blue (100,0,0);
    for (int i = 0; i < I.rows; ++i)
        for (int j = 0; j < I.cols; ++j)
        {
            Mat sampleMat = (Mat_<float>(1,2) << i, j);
            float response = tree->predict(sampleMat);

            if      (response == 1)    I.at<Vec3b>(j, i)  = green;
            else if (response == 2)    I.at<Vec3b>(j, i)  = blue;
        }
    //! [show]

    //----------------------- 5. Show the training data --------------------------------------------
    //! [show_data]
    int thick = -1;
    int lineType = 8;
    float px, py;
    // Class 1
    for (int i = 0; i < NTRAINING_SAMPLES; ++i)
    {
        px = trainData.at<float>(i,0);
        py = trainData.at<float>(i,1);
        circle(I, Point( (int) px,  (int) py ), 3, Scalar(0, 255, 0), thick, lineType);
    }
    // Class 2
    for (int i = NTRAINING_SAMPLES; i <2*NTRAINING_SAMPLES; ++i)
    {
        px = trainData.at<float>(i,0);
        py = trainData.at<float>(i,1);
        circle(I, Point( (int) px, (int) py ), 3, Scalar(255, 0, 0), thick, lineType);
    }
    //! [show_data]
    //imshow("shit", I);
    waitKey(0);

    tree->save("shittytree3.xml");

}
