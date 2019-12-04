//
// Created by CJ Hillbrand on 2019-11-17.
//

#ifndef CAMERAPLAYER_H
#define CAMERAPLAYER_H

#include "Player.h"
#include "../HandProcessing/HandProcessor.h"
#include <iostream>
using namespace std;

class CameraPlayer : public Player {
public:
    CameraPlayer() : Player(), hp(HandProcessor(4)) {
        type = 3;
        prevCoord = { .x = -1, .y = -1};

    };
    CameraPlayer(const string file, const int videoNum) : Player(), hp(HandProcessor(file, videoNum)) {
        type = 3;
        prevCoord = { .x = -1, .y = -1};
    }
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
        nextMove = { .x = nextMove.x + deltaX, .y = nextMove.y + deltaY};

        prevCoord.x = nextCoordinates.x;
        prevCoord.y = nextCoordinates.y;
        if (moves % 2 == 0) {
            Movement temp = { .x = nextMove.x * 2, .y = nextMove.y * 2 };
            nextMove = { .x = 0, .y = 0 };
            return temp;
        } else
            return NO_CHANGE;
    }
    using Player::setX;
    using Player::setY;
    using Player::getX;
    using Player::getY;
    using Player::setFrame;

private:
    HandProcessor hp;
    Movement prevCoord;
    Movement nextMove;
    int moves;
    const int EXIT_COND_INT = -2;
    const Movement NO_CHANGE = {.x = 0, .y = 0};
    const Movement EXIT_COND = {.x = -960, .y = -960};


};


#endif //FINALPROJECT487_CAMERAPLAYER_H
