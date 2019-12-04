//
// Created by CJ Hillbrand on 2019-11-08.
//
#include "AirHockeyEngine.h"
#include "./HandProcessing/HandProcessor.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;
using namespace cv;

void fillAnnotations(vector<Point> &annotations, const int testNum) {
    ifstream file;
    file.open("./HandProcessing/annotations/annotations_" + to_string(testNum) + ".psv");
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

void recordVideo() {
    int num;
    cout << "What number video would you like to identify this with? " << endl;
    cin >> num;
    HandProcessor hp(num);
    Point next;
    const int NUM_FRAMES = 200;

    for (int i = 0; i < NUM_FRAMES; i++)
        next = hp.getCurrentCoord();

    hp.close();
}

void calculateVideo() {
    // Lets run some tests && record results
    int videoNum;
    double threshold;
    cout << "What video number would you like to test?" << endl;
    cin >> videoNum;
    cout << "What threshold would you like to test?" << endl;
    cin >> threshold;
    HandProcessor hp("./HandProcessing/videos/airhockey_test_" + to_string(videoNum) + ".mov", videoNum);
    hp.setCorrelationThreshold(threshold);
    const int THRESHOLD_WIDTH = 60;
    const int THRESHOLD_HEIGHT = 60;
    const int NUM_FRAMES = 200;

    vector<Point> annotations;
    fillAnnotations(annotations, videoNum);
    Point estimate, real;
    int count[] = {0, 0, 0, 0};
    for (int i = 0; i < NUM_FRAMES && i < annotations.size(); i++) {
        estimate = hp.getCurrentCoord();
        real = annotations[annotations.size() - i];
        cout << real << " " << estimate << endl;
        if (real.x == -1 && real.y == -1 && estimate.x == 0 && estimate.y == 0) {
            count[2]++;
        } else if (estimate.x == 0 && estimate.y == 0) {
            count[3]++;
        } else if (real.x == -1 && real.y == -1) {
            count[1]++;
        } else if (real.x + THRESHOLD_WIDTH >= estimate.x && real.x - THRESHOLD_WIDTH <= estimate.x &&
                   real.y + THRESHOLD_HEIGHT >= estimate.y && real.y - THRESHOLD_HEIGHT <= estimate.y) {
            count[0]++;
        } else {
            count[1]++;
        }

    }
    ofstream output;
    output.open("./HandProcessing/results/recall_precision_" + to_string(videoNum) + ".csv", std::ios_base::app);
    output << threshold << "," << count[0] << "," << count[1] << "," << count[2] << "," << count[3] << endl;
    output.close();
    hp.close();

}

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
        HandProcessor hp("./HandProcessing/videos/airhockey_test_" + to_string(num) + ".mov", num);
        hp.annotateLoadedVideo();
    }
}
