//
// Created by CJ Hillbrand on 2019-11-21.
//

#include "HandProcessor.h"
HandProcessor::HandProcessor(const int videoNum) : video(VideoCapture(0)), newVideo(true), videoNum(videoNum) {
    recorder.open("./HandProcessing/videos/airhockey_test_" + to_string(videoNum) + ".mov",
                  VideoWriter::fourcc('m', 'p', '4', 'v'), video.get(CAP_PROP_FPS),
                  Size(video.get(CAP_PROP_FRAME_WIDTH), video.get(CAP_PROP_FRAME_HEIGHT)));

    if (!video.isOpened()) {
        throw "NO ACCESS TO VIDEO";
    }
    createNewTestImage();
}

HandProcessor::HandProcessor(const string name, const int videoNum) : video(VideoCapture(name)), newVideo(false), videoNum(videoNum) {
    if (!video.isOpened()) {
        throw "NO ACCESS TO FILE";
    }
    for (int i = 0; i < TEMPLATE_NUM; i++) {
        templates[i] = imread("./HandProcessing/TemplateImages/template_" + to_string(i) + "_test_" + to_string(videoNum) + ".jpg");
        templates[i] = preProcesses(templates[i]);
        templates[i] = findImageGradients(templates[i]);
    }
}

bool HandProcessor::createNewTestImage() {
    Mat frames[TEMPLATE_NUM];
    video >> frames[0];
    char exitCode = ' ';
    Rect rectangles[] = { Rect(10, 10, VIDEO_WIDTH / 4, VIDEO_HEIGHT / 2),
                          Rect(VIDEO_WIDTH / 4 - 10, 10, VIDEO_WIDTH / 4, VIDEO_HEIGHT / 2),
                          Rect(10, VIDEO_HEIGHT / 2, VIDEO_WIDTH / 4, VIDEO_HEIGHT / 2),
                          Rect(VIDEO_WIDTH / 4 - 10, VIDEO_HEIGHT / 2, VIDEO_WIDTH / 4, VIDEO_HEIGHT / 2),
                          Rect(VIDEO_WIDTH / 8, VIDEO_HEIGHT / 4, VIDEO_WIDTH / 4, VIDEO_HEIGHT / 2)};
    int i = 0;
    while (!frames[i].empty()) {
        rectangle(frames[i], rectangles[i], GREEN, 1);
        imshow("Video", frames[i]);
        exitCode = (char)waitKey(TEMPLATE_NUM);
        if (exitCode == 's') {
            templates[i] = Mat(frames[i], rectangles[i]);
            imwrite("./HandProcessing/TemplateImages/template_" + to_string(i) + "_test_" + to_string(videoNum) + ".jpg", templates[i]);
            templates[i] = preProcesses(templates[i]);
            templates[i] = findImageGradients(templates[i]);
            i++;
        }
        if (i == TEMPLATE_NUM) break;
        video >> frames[i];

    }
    return false;
}

Point HandProcessor::getCurrentCoord() {
    Point result;
    Mat frame;
    video >> frame;
    if (newVideo) {
        recorder << frame;
    }
    if (frame.empty()) {
        done = true;
        recorder.release();
        result = Point(-2, -2);
        return result;
    }
    frame = preProcesses(frame);
    return findKeyPoint(frame);
}

Mat HandProcessor::preProcesses(const Mat &frame) {
    Mat temp = frame.clone();
    resize(temp, temp, Size(), SIZE_FACTOR, SIZE_FACTOR);
    for (int i = 0; i < SMOOTHING_ITERATIONS; i++) {
        GaussianBlur(temp, temp, Size(SMOOTH_KERNEL_SIZE, SMOOTH_KERNEL_SIZE),
                     STANDARD_DEVIATION, STANDARD_DEVIATION);
    }


    if (frame.channels() == 3) {
        cvtColor(temp, temp, COLOR_BGR2GRAY);
    }
    return temp;
}

Mat HandProcessor::findImageGradients(const Mat &temp) {
    Mat gradients, abs_gradients;
    Laplacian(temp, gradients, CV_16S, 3, 1, 0, BORDER_DEFAULT);
    convertScaleAbs(gradients, abs_gradients);
    return abs_gradients;
}

Point HandProcessor::findHandLocationGradient(const Mat &image) {
    Mat result;

    double minVal, maxVal;
    double maxValAbs = 0;
    Point minLoc, maxLoc;
    Point matchLoc;

    for (int i = 0; i < TEMPLATE_NUM; i++) {
        matchTemplate( image, templates[i], result, TM_CCOEFF_NORMED);
        minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
        if (maxVal > maxValAbs) {
            maxValAbs = maxVal;
            matchLoc = maxLoc;
        }
    }

    if (maxVal < CORRELATION_THRESHOLD) {
        matchLoc.x = 0;
        matchLoc.y = 0;
    }
    return matchLoc;
}

Point HandProcessor::findKeyPoint(const Mat& frame) {
    Mat temp = frame.clone();
    temp = findImageGradients(temp);
    Point matchLoc = findHandLocationGradient(temp);
    Rect location(matchLoc.x, matchLoc.y, templates[0].cols, templates[0].rows);
    rectangle(temp, location, WHITE);
    imshow("RESULT", temp);

    return matchLoc;
}

bool HandProcessor::isDone() {
    return done;
}

void HandProcessor::close() {
    if (!newVideo) {
        recorder.release();
    }
    video.release();
}

void HandProcessor::setCorrelationThreshold(const double thresh) {
    CORRELATION_THRESHOLD = thresh;
}

// This method should only be used with the test files.
void HandProcessor::annotateLoadedVideo() {
    if (newVideo) {
        cout << "You must construct the HandProcessor with a video before annotating" << endl;
        return;
    }
    annotations = vector<Point>();
    Mat frame;
    video >> frame;
    namedWindow("ANNOTATE WINDOW", 1);
    Point *p = new Point();
    setMouseCallback("ANNOTATE WINDOW", mouseHandler, p); // add point to annotations
    while (!frame.empty()) {
        p -> x = -1;
        p -> y = -1;
        imshow("ANNOTATE WINDOW", frame);
        waitKey(0);
        annotations.push_back(*p);
        video >> frame;
    }
    ofstream output;
    output.open("./HandProcessing/annotations/annotations_" + to_string(videoNum) + ".psv");
    int size = annotations.size();
    for (int i = 1; i <= size; i++) {
        Point temp = annotations.back();
        output << temp.x << "," << temp.y << "|";
        annotations.pop_back();
        if (i % 20 == 0)
            output << endl;
    }
    output << endl;
    output.close();
    delete p;
}

void HandProcessor::mouseHandler(int event, int x, int y, int flags, void *param) {
    Point* temp = (Point*)param;
    if (event == EVENT_LBUTTONDOWN) {
        temp -> x = x;
        temp -> y = y;
    }
}