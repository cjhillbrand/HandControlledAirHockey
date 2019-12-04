//
// Created by CJ Hillbrand on 2019-11-21.
//

#ifndef FINALPROJECT487_HANDPROCESSOR_H
#define FINALPROJECT487_HANDPROCESSOR_H

#include "opencv2/opencv.hpp"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

class HandProcessor {
public:
    HandProcessor(const int); //Default to camera
    HandProcessor(const string, const int); // .mov file to read in
    bool createNewTestImage();
    bool isDone();
    Point getCurrentCoord();
    void annotateLoadedVideo();
    void close();
    void setCorrelationThreshold(const double);


private:
    VideoCapture video;
    VideoWriter recorder;

    Mat preProcesses(const Mat&);
    Mat findImageGradients(const Mat&);
    Point findHandLocationGradient(const Mat&);

    Point findKeyPoint(const Mat& frame);

    static void mouseHandler(int event, int x, int y, int flags, void*);

    const int SMOOTH_KERNEL_SIZE = 3;
    const int STANDARD_DEVIATION = 0;
    const int SMOOTHING_ITERATIONS = 3;
    const double SIZE_FACTOR = 0.5;
    double CORRELATION_THRESHOLD = 0.2;
    const int TEMPLATE_NUM = 5;

    // Video dimensions
    const int VIDEO_HEIGHT = video.get(CAP_PROP_FRAME_HEIGHT);
    const int VIDEO_WIDTH = video.get(CAP_PROP_FRAME_WIDTH);

    // Color consts
    const Scalar GREEN = Scalar(0, 240, 0);
    const Scalar WHITE = Scalar(255, 255, 255);

    int videoNum;
    bool done;
    bool newVideo;
    Mat templates[5];
    Mat templatePrime[5];
    float templatePrimeSquaredSum[5];
    vector<Point> annotations;
};


#endif //FINALPROJECT487_HANDPROCESSOR_H
