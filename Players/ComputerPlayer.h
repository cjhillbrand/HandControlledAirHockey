//
// Created by CJ Hillbrand on 2019-11-17.
//

#ifndef COMPUTERPLAYER_H
#define COMPUTERPLAYER_H
#include <iostream>
#include "Player.h"
#include <stdlib.h>
using namespace std;

class ComputerPlayer : public Player {
public:
    ComputerPlayer() : Player() {
        type = 2;
    };
    ComputerPlayer(int x, int y) : Player(x, y) {}

    ComputerPlayer(int playerNum) : Player(), playerNum(playerNum) {}

    Movement getMove(char move) {
        // if the puck is on the other side of the field
        // Player 1
        Movement m;
        if (puckX > FRAME_WIDTH / 2 && playerNum == 1) {
            m = {.x = (FRAME_WIDTH / 4 - x) / DAMPENING, .y = (FRAME_HEIGHT / 2 - y) / DAMPENING};
        }
        // Player 2
        else if (puckX < FRAME_WIDTH / 2 && playerNum == 2) {
            m = {.x = (FRAME_WIDTH * 3 / 4 - x) / DAMPENING, .y = (FRAME_HEIGHT / 2 - y) / DAMPENING};
        } else {
            m = {.x = (puckX - x) / DAMPENING, .y = (puckY - y) / DAMPENING};
        }
        x += m.x;
        y += m.y;
        return m;
    }
    using Player::setX;
    using Player::setY;
    using Player::getX;
    using Player::getY;
    using Player::setFrame;
private:
    int playerNum;

    const int DAMPENING = 15;
};


#endif //FINALPROJECT487_COMPUTERPLAYER_H
