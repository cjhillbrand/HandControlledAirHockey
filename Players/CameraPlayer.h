//
// Created by CJ Hillbrand on 2019-11-17.
//

// The camera player extends the player class. It creates a hand processing object
// to calculate the position of the hand in each frame, but calculates the deltas itself.
// The deltas are what it will return to be its final movement

#ifndef CAMERAPLAYER_H
#define CAMERAPLAYER_H

#include "Player.h"
#include "../HandProcessing/HandProcessor.h"
#include <iostream>
using namespace std;

class CameraPlayer : public Player {
public:
    // Constructor to read in live video. Defaults to writing to video 4
    CameraPlayer() : Player(), hp(HandProcessor(4)) {
        type = 3;
        prevCoord = {-1, -1};

    };
    // Constructor to read in from a premade file.
    CameraPlayer(const string file, const int videoNum) : Player(), hp(HandProcessor(file, videoNum)) {
        type = 3;
        prevCoord = {-1, -1};
    }

    // Returns the delta between the previous position and the next position.
    // If the video is done it will return a special move signifying the end of the video.
    Movement getMove(char move) {
        //cout << "Returning new CameraMove" << endl;
        moves++;
        Point nextCoordinates = hp.getCurrentCoord();
        if (nextCoordinates.x == EXIT_COND_INT && nextCoordinates.y == EXIT_COND_INT) {
            return EXIT_COND;
        }
        if (prevCoord.x == -1 && prevCoord.y == -1) {
            prevCoord.x = nextCoordinates.x;
            prevCoord.y = nextCoordinates.y;
        }
        int deltaX = -(nextCoordinates.x - prevCoord.x);
        int deltaY = nextCoordinates.y - prevCoord.y;
        if (deltaX > 50 || deltaX < -50) {
            deltaX = deltaX / 10;
        }
        if (deltaY > 50 || deltaY < -50) {
            deltaY = deltaY / 10;
        }
        nextMove = {nextMove.x + deltaX, nextMove.y + deltaY};

        prevCoord.x = nextCoordinates.x;
        prevCoord.y = nextCoordinates.y;
        if (moves % 2 == 0) {
            Movement temp = {nextMove.x * 2, nextMove.y * 2};
            nextMove = {0, 0};
            return temp;
        } else
            return NO_CHANGE;
    }

    // Methods inherited from Player
    using Player::setX;
    using Player::setY;
    using Player::getX;
    using Player::getY;
    using Player::setFrame;

private:
    HandProcessor hp;   // Hand proccessor
    Movement prevCoord; // Previous hand position
    Movement nextMove;  // Next hand position 
    int moves;          // Number of moves made
    const int EXIT_COND_INT = -2;   // Special exit condition
    const Movement NO_CHANGE = {0, 0};  // Constant for no change
    const Movement EXIT_COND = {-960, -960};    // Constant for exit condition


};


#endif //FINALPROJECT487_CAMERAPLAYER_H
