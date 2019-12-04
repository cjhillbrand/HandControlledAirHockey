//
// Created by CJ Hillbrand on 2019-11-08.
//

// Header file for the Air Hockey Engine class
// Please refer to the .cpp file for full function 
// definitions.

#ifndef AIRHOCKEYENGINE_H
#define AIRHOCKEYENGINE_H
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
#include <string>
#include "Players/Player.h"
#include "Players/ComputerPlayer.h"
#include "Players/CameraPlayer.h"
#include "Players/KeyboardPlayer.h"
#include <vector>

using namespace cv;
using namespace std;

class AirHockeyEngine {
public:
    // Constructors
    AirHockeyEngine(const int, const int);
    AirHockeyEngine(const int);
    AirHockeyEngine();

    //Destructor
    ~AirHockeyEngine();

    // Creates the frame for the game display
    bool createFrame();
    
    // Asks a player to make a move
    Player::Movement makeMove(const int);

    // Shows the next frame
    bool showFrame();

    // Returns whether or not the game is over
    bool gameOver();

    // The struct that holds information about the puck
    struct Puck {
        int x, y;
        double velocityX, velocityY;
    };

private:
    Mat_<uchar> frame;  // Frame that holds the state of the game
    int size;           // Hold the size of the frame
    int scoreOne;       // Score of player one
    int scoreTwo;       // Score of player two
    Player *p1;         // Pointer to player one
    Player *p2;         // Pointer to player two
    int moveCount;      // Number of moves that have been made
    vector<char> moves; // Holds keyboard input for the moves that need to be handled
    VideoWriter recorder;   // Records the current game
    bool gameover;      // True if the game is finished

    Puck puck;          // holds the state of the puck

    void fillFrame();   // Draws static components of frame
    void updateState(); // Updates the state of the game
    void deleteOldState();  // Deletes the state of the game
    // Updates state of puck
    void updatePuck(Player::Movement&, Player::Movement&, Player::Movement&, Player::Movement&);
    // Checks if the paddle and puck have collided
    void checkPaddlePuckCollision(int, int, int, int, int, int, int);
    void resetState(const int); // Resets the state of the game except the scores
    void forceQuit();           // Quits the game

    const static int DEFAULT_SIZE = 480;    // Default size of game
    const int PADDLE_DIM = 50 * size / 480; // Dimensions of paddles
    const int PUCK_DIM = 10 * size / 480;   // Dimensions of puck
    const double DAMPENING = 0.922;         // Dampening of the puck Velocity
    const double INIT_VELOCITY = 3.0;       // Initial velocity of the puck
    const int MIDDLE_WIDTH = 5;             // Width of the mid-line
    const int GOAL_WIDTH = size / 6;        // Widht of the goals
    const int GOAL_HEIGHT = size / 3;       // Height of the goals
    const Scalar WHITE = Scalar(240, 240, 240); // The color white
    const Scalar BLACK = Scalar(0, 0, 0);       // The color black
    const int KEYBOARD_PLAYER = 1;          // Constant used to identify keyboard players
    const int COMPUTER_PLAYER = 2;          // Constant used to identify Computer players
    const int CAMERA_PLAYER = 3;            // Constant used to identify Camera Players
    const int EXIT_COND = DEFAULT_SIZE * -2;    // Exit condition

    // Path to the video, must be changed to specifics of machine.
    const string PATH_TO_VIDEO = "C:\\Users\\cj\\source\\repos\\HandControlledAirHockey\\HandProcessing\\videos\\";

};

#endif //AIRHOCKEYENGINE_H
