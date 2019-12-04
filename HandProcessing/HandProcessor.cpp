//
// Created by CJ Hillbrand on 2019-11-21.
//

// This class handles the hand recogniton. The main functionality is
// to feed a video or live stream to the object and then to ask what the position
// of the hand is by calling getCurrentCoord.
// Also can annotate a video, if the user decides to instantiate with a premade video.

#include "HandProcessor.h"

// Precondition: A video number. This number will be used to record to a certain file.
// Postcondition: Creates a video recorder that writes the images captured by the video to a file.
HandProcessor::HandProcessor(const int videoNum) : video(VideoCapture(0)), newVideo(true), videoNum(videoNum) {
    recorder.open(PATH_TO_VIDEO + "airhockey_test_" + to_string(videoNum) + ".mov",
                  VideoWriter::fourcc('m', 'p', '4', 'v'), video.get(CAP_PROP_FPS),
                  Size(static_cast<int>(video.get(CAP_PROP_FRAME_WIDTH)), static_cast<int>(video.get(CAP_PROP_FRAME_HEIGHT))));

    if (!video.isOpened()) {
        throw "NO ACCESS TO VIDEO";
    }
    createNewTestImage();
}

// Precondition: Takes in a video name and the associated video number.
// Postcondition: Reads the template images for the associated video number and
//                loads the video into memory.
HandProcessor::HandProcessor(const string name, const int videoNum) : video(VideoCapture(name)), newVideo(false), videoNum(videoNum), done(false) {
    if (!video.isOpened()) {
        throw "NO ACCESS TO FILE";
    }
    for (int i = 0; i < TEMPLATE_NUM; i++) {
        templates[i] = imread(PATH_TO_TEMPLATES + "template_" + to_string(i) + "_test_" + to_string(videoNum) + ".jpg");
        templates[i] = preProcesses(templates[i]);
        templates[i] = findImageGradients(templates[i]);
    }
}

// Precondition: Creates 5 new test images. These images are used for the template matching.
// Postcondition: Writes the template images to a jpg file.
bool HandProcessor::createNewTestImage() {
    vector<Mat>frames(TEMPLATE_NUM);
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
            imwrite(PATH_TO_TEMPLATES + "template_" + to_string(i) + "_test_" + to_string(videoNum) + ".jpg", templates[i]);
            templates[i] = preProcesses(templates[i]);
            templates[i] = findImageGradients(templates[i]);
            i++;
        }
        if (i == TEMPLATE_NUM) break;
        video >> frames[i];

    }
    return false;
}

// Precondtion: N/A
// Postcondition: Returns the coordinates of the hand in the current frame.
Point HandProcessor::getCurrentCoord() {
    Point result;
    Mat frame;
    video >> frame;
    currentFrame = frame.clone();
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

// Precondtion: N/A
// Postcondition: Shows the current frame to the screen.
void HandProcessor::showFrame() {
    imshow("FRAME", currentFrame);
    waitKey(5);
}

// Precondition: Passed a Mat.
// Postcondition: returns a Mat that has been resized, smoothed, and converted to greyscale.
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

// Precondition: Gives a mat that has been preprocessed.
// Postcondition: Returns a mat that stores the image gradients at each
//                pixel location.
Mat HandProcessor::findImageGradients(const Mat &temp) {
    Mat gradients, abs_gradients;
    Laplacian(temp, gradients, CV_16S, 3, 1, 0, BORDER_DEFAULT);
    convertScaleAbs(gradients, abs_gradients);
    return abs_gradients;
}

// Precondition: Passed a mat that stores the gradients of an image.
// Postcondition: returns the Point where the uppper left corner of the hand
//                is located at.
Point HandProcessor::findHandLocationGradient(const Mat &image) {
    Mat result;

    double minVal;
    double maxVal = 0.0;
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

// Precondition: Passed a pre-processed image.
// Postcondition: Returns the upper left location of the hand in the current frame.
Point HandProcessor::findKeyPoint(const Mat& frame) {
    Mat temp = frame.clone();
    temp = findImageGradients(temp);
    Point matchLoc = findHandLocationGradient(temp);
    Rect location(matchLoc.x, matchLoc.y, templates[0].cols, templates[0].rows);
    rectangle(temp, location, WHITE);
    imshow("RESULT", temp);

    return matchLoc;
}

// Precondition: N/A
// Postconditon: Returns if the HandProcessor is done
bool HandProcessor::isDone() {
    return done;
}

// Precondition: N/A
// Postcondition; Closes the video and recording streams.
void HandProcessor::close() {
    if (!newVideo) {
        recorder.release();
    }
    video.release();
}

// Precondtion: Passed a double
// Postcondition: Sets the correlation threshold for the template matching.
void HandProcessor::setCorrelationThreshold(const double thresh) {
    CORRELATION_THRESHOLD = thresh;
}

// Precondition: A pre-recorded video must be passed during the instantiation of this object
// Postcondition: Allows the user to annotate the pre-loaded video. The user must click the screen where
//                 the middle of the hand is located and then press the space bar for the next image.
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
    output.open(PATH_TO_ANNOTATIONS + "annotations_" + to_string(videoNum) + ".psv");
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

// The mouse Handler passed to the frame so that it can record the location
// at which the user pressed.
void HandProcessor::mouseHandler(int event, int x, int y, int flags, void *param) {
    Point* temp = (Point*)param;
    if (event == EVENT_LBUTTONDOWN) {
        temp -> x = x;
        temp -> y = y;
    }
}