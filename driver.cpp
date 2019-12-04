//
// Created by CJ Hillbrand on 2019-11-08.
//

//
// This is the driver of the Hand Controlled Air Hockey.
// This program has been compiled for MSVS 2017 for Windows OS
// and has been compiled using the g++ compiler for Unix based systems
// BEFORE RUNNING MAKE SURE: to change the variable PATH_BASE in driver.cpp, and HandProcessor.h
// ALSO MAKE SURE to change the PATH_TO_VIDEO in AirHockeyEngine.cpp
// These paths should signify where your video, TemplateImage, Results, and annotations are located. 

// This Porgram is capable of four options:
// (P)lay:      by pressing 'p' the user can play the air hockey game.
// (R)ecord:    by pressing 'r' the user can record a new video, without having to play.
// (A)nnotate:  by pressing 'a' the user can Annotate a already-recorded video.
// (C)alculate: by pressing 'c' the user can Calculate the precision and recall for a video that
//              has already been recorded AND has already been annotated.

#include "AirHockeyEngine.h"
#include "./HandProcessing/HandProcessor.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;
using namespace cv;


// These are the strings that contain the Path to your video, results, and annotation folders.
const static string PATH_BASE = "C:\\Users\\cj\\source\\repos\\HandControlledAirHockey\\HandProcessing\\";
const static string PATH_TO_VIDEO = PATH_BASE + "videos\\";
const static string PATH_TO_RESULTS = PATH_BASE + "results\\";
const static string PATH_TO_ANNOTATIONS = PATH_BASE + "annotations\\";

// Constant to determine the number of frames used for recording and calculations.
const static int NUM_FRAMES = 500;

// Preconditions:   An empty vector of points, and a valid video number. 
// Postconditions:  Fills the vecotr of points with the contents of the video numbers
//                  annotations file. 
void fillAnnotations(vector<Point> &annotations, const int testNum) {
    ifstream file;
    file.open(PATH_TO_ANNOTATIONS + "annotations_" + to_string(testNum) + ".psv");
    while (file.good()) { //should read the first coordinate value
        Point p;
        string point;
        getline(file, point, '|');
        stringstream tempLine(point);
        string xVal, yVal;
        getline(tempLine, xVal, ',');
        getline(tempLine, yVal, '\n');
        if (!xVal.empty() && !yVal.empty()) {

            p.x = stoi(xVal);
            p.y = stoi(yVal);
            if (p.x != -1) {
                p.x = (p.x - 120) / 2;
                p.y = (p.y - 120) / 2;
            }
        }
        annotations.push_back(p);
    }
    file.close();
}


// Precondition: The program must have access to an embedded camera.
// Postcondtion: Prompts the user for a video number to store the recorded video.
//               records for the set number of frame determined by the static const NUM_FRAMES.
void recordVideo() {
    int num;
    cout << "What number video would you like to identify this with? " << endl;
    cin >> num;
    HandProcessor hp(num);
    Point next;

    for (int i = 0; i < NUM_FRAMES; i++) {
        next = hp.getCurrentCoord();
        hp.showFrame();
    }

    hp.close();
}

// Precondition:    Prompts the user for a video number. This number must have a video associated
//                  and a annotation psv file associated.
// Postcondtion:    Writes the true positive, false positive, true negative, and false positive to the 
//                  results sheet.
void calculateVideo() {
    // Lets run some tests && record results
    int videoNum;
    double threshold;
    cout << "What video number would you like to test?" << endl;
    cin >> videoNum;
    cout << "What threshold would you like to test?" << endl;
    cin >> threshold;
    HandProcessor hp(PATH_TO_VIDEO + "airhockey_test_" + to_string(videoNum) + ".mov", videoNum);
    hp.setCorrelationThreshold(threshold);
    const int THRESHOLD_WIDTH = 60;
    const int THRESHOLD_HEIGHT = 60;

    vector<Point> annotations;
    fillAnnotations(annotations, videoNum);
    Point estimate, real;
    int count[] = {0, 0, 0, 0};
    for (int i = 1; i <= NUM_FRAMES && i <= annotations.size(); i++) {
        estimate = hp.getCurrentCoord();
        real = annotations[annotations.size() - i];
        cout << real << " " << estimate << endl;
        // True negative
        if (real.x == -1 && real.y == -1 && estimate.x == 0 && estimate.y == 0) {
            count[2]++;
        // False negative
        } else if (estimate.x == 0 && estimate.y == 0) {
            count[3]++;
        // False Positive
        } else if (real.x == -1 && real.y == -1) {
            count[1]++;
        // True Positive
        } else if (real.x + THRESHOLD_WIDTH >= estimate.x && real.x - THRESHOLD_WIDTH <= estimate.x &&
                   real.y + THRESHOLD_HEIGHT >= estimate.y && real.y - THRESHOLD_HEIGHT <= estimate.y) {
            count[0]++;
        } else {
            count[1]++;
        }

    }
    ofstream output;
    output.open(PATH_TO_RESULTS + "recall_precision_" + to_string(videoNum) + ".csv", std::ios_base::app);
    output << threshold << "," << count[0] << "," << count[1] << "," << count[2] << "," << count[3] << endl;
    output.close();
    hp.close();

}

// Precondtion: Once prompted the user must give a valid video number to run as input for the video, or
//              the computer must have access to a camera.
// Postcondition:   Continually shows the next frame of the Air Hockey game until the game is over.
void run() {
    const char YES = 'y';
    const char NO = 'n';

    cout << "Would you like to load a pre-recorded video? (Y)es or (N)o" << endl;
    char response;
    cin >> response;

    int videoNum = -1;
    if (response == YES) {
        cout << "What video number would you like to load?" << endl;
        cin >> videoNum;
    } else
        cout << "Video and Template images are recorded under video number 4" << endl;

    AirHockeyEngine game(videoNum);
    game.createFrame();
    while(!game.gameOver()) {
        game.showFrame();
    }
}

// Precondition:    Arguments passed in are not used.
// Postcondition:   Depending on the task chosen, will execute the chosen task.
int main(int argc, char* argv[]) {
    cout << "What would you like to do: (P)lay Air Hockey, (R)ecord video, (A)nnotate video, (C)alculate results" << endl;
    char response;
    cin >> response;

    const char RECORD = 'r';
    const char ANNOTATE = 'a';
    const char CALCULATE = 'c';
    const char PLAY = 'p';

    if (response == PLAY) {
        run();
    }

    if (response == RECORD) {
        recordVideo();
    }

    if (response == CALCULATE) {
        calculateVideo();
    }

    if (response == ANNOTATE) {
        int num;
        cout << "What video number would you like to annotate?" << endl;
        cin >> num;
        HandProcessor hp(PATH_TO_VIDEO + "airhockey_test_" + to_string(num) + ".mov", num);
        hp.annotateLoadedVideo();
    }
}
