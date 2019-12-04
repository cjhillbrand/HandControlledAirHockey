//
// Created by CJ Hillbrand on 2019-11-08.
//

#ifndef AIRHOCKEYENGINE_H
#define AIRHOCKEYENGINE_H
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
#include "Players/Player.h"
#include "Players/ComputerPlayer.h"
#include "Players/CameraPlayer.h"
#include "Players/KeyboardPlayer.h"
#include <vector>

using namespace cv;
using namespace std;

class AirHockeyEngine {
public:
    AirHockeyEngine(const int, const int);
    AirHockeyEngine(const int);
    AirHockeyEngine();
    ~AirHockeyEngine();
    bool createFrame();
    Player::Movement makeMove(const int);
    bool showFrame();
    bool gameOver();
    struct Puck {
        int x, y, velocityX, velocityY;
    };

private:
    Mat_<uchar> frame;
    int size;
    int scoreOne;
    int scoreTwo;
    Player *p1;
    Player *p2;
    int moveCount;
    vector<char> moves;
    VideoWriter recorder;
    bool gameover;

    Puck puck;

    void fillFrame();
    void updateState();
    void deleteOldState();
    void updatePuck(Player::Movement&, Player::Movement&, Player::Movement&, Player::Movement&);
    void checkPaddlePuckCollision(int, int, int, int, int, int, int);
    void resetAndUpdateScore(const int);
    void resetState(const int);
    void forceQuit();

    const static int DEFAULT_SIZE = 480;
    const int PADDLE_DIM = 50 * size / 480;
    const int PUCK_DIM = 10 * size / 480;
    const double DAMPENING = 0.922;
    const double INIT_VELOCITY = 3.0;
    const int MIDDLE_WIDTH = 5;
    const int GOAL_WIDTH = size / 6;
    const int GOAL_HEIGHT = size / 3;
    const Scalar WHITE = Scalar(240, 240, 240);
    const Scalar BLACK = Scalar(0, 0, 0);
    const int KEYBOARD_PLAYER = 1;
    const int COMPUTER_PLAYER = 2;
    const int CAMERA_PLAYER = 3;
    const int EXIT_COND = DEFAULT_SIZE * -2;

};

#endif //AIRHOCKEYENGINE_H
