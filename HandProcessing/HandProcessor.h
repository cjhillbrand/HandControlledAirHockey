//
// Created by CJ Hillbrand on 2019-11-21.
//

// Header for the hand proccessor for more descriptive method definitions
// Please see the .cpp file

#ifndef FINALPROJECT487_HANDPROCESSOR_H
#define FINALPROJECT487_HANDPROCESSOR_H

#include "opencv2/opencv.hpp"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

class HandProcessor {
public:
    // Constructors for the HandProccesor
    HandProcessor(const int); //Default to camera
    HandProcessor(const string, const int); // .mov file to read in

    // Creates and saves new test images
    bool createNewTestImage();

    // Returns true if there is no more video to process.
    bool isDone();

    // Returns the coordnate of the hand in the current frame
    Point getCurrentCoord();

    // Initiates the work flow to annotate a video.
    void annotateLoadedVideo();

    // Closes all streams
    void close();

    // Sets the threshold used for template matching
    void setCorrelationThreshold(const double);

    // Shows the next frame
    void showFrame();


private:
    VideoCapture video;     // Video stream
    VideoWriter recorder;   // Video recorder

    Mat preProcesses(const Mat&);   // Pre-process image before finding gradients
    Mat findImageGradients(const Mat&); // Calculates gradients of an image
    Point findHandLocationGradient(const Mat&); // Returns the location of the hand in the current frame.

    Point findKeyPoint(const Mat& frame);   // Returns the location of the hand in the current frame.

    static void mouseHandler(int event, int x, int y, int flags, void*); // Handler for mouse click events

    const int SMOOTH_KERNEL_SIZE = 3;   // Size for gaussian smoothing
    const int STANDARD_DEVIATION = 0;   // Standard deviation for gaussian smoothing
    const int SMOOTHING_ITERATIONS = 3; // Number of iterations performed for smoothing
    const double SIZE_FACTOR = 0.5;     // How much to reduce the dimensions of the frame.
    double CORRELATION_THRESHOLD = 0.2; // What value to threshold the best correlation against.
    const int TEMPLATE_NUM = 5;         // How many templates that are stored

    // Video dimensions
    const int VIDEO_HEIGHT = static_cast<int>(video.get(CAP_PROP_FRAME_HEIGHT));
    const int VIDEO_WIDTH = static_cast<int>(video.get(CAP_PROP_FRAME_WIDTH));

    // Color consts
    const Scalar GREEN = Scalar(0, 240, 0);
    const Scalar WHITE = Scalar(255, 255, 255);

    int videoNum;   // Video number
    bool done;      // Stores if the HP is done
    bool newVideo;  // Whether or not a new video is being made
    Mat templates[5];   // Array to store the templates
    Mat currentFrame;   // Current frame 
    vector<Point> annotations;  // Vector to hold points for the annotations.

    // Path to all of the resources that this object writes to or reads from.
    // Must be updated to machine specs
    const string PATH_BASE = "C:\\Users\\cj\\source\\repos\\HandControlledAirHockey\\HandProcessing\\";
    const string PATH_TO_VIDEO = PATH_BASE + "videos\\";
    const string PATH_TO_TEMPLATES = PATH_BASE + "TemplateImages\\";
    const string PATH_TO_ANNOTATIONS = PATH_BASE + "annotations\\";
};


#endif //FINALPROJECT487_HANDPROCESSOR_H
