//
// Created by CJ Hillbrand on 2019-11-18.
//

#ifndef KEYBOARDPLAYER_H
#define KEYBOARDPLAYER_H
#include <iostream>
#include "Player.h"
#include <vector>
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

class KeyboardPlayer : public Player {
public:
    KeyboardPlayer() : Player() {
        type = 1;
    };
    KeyboardPlayer(int player) {
        playerNum = player;
        moves = -1;
        type = 1;
    };
    KeyboardPlayer(int x, int y) : Player(x, y) {}

    Movement getMove(char move) {
        Movement m = { .x = 0, .y = 0};
        moves++;
        if (moves == 0 || move == -1) {
            velocityY = 0;
            velocityX = 0;
            return m;
        }
        if (playerNum == 1) {
            switch (move) {
                case 'w':
                    m.y = -delta;
                    break;
                case 's':
                    m.y = delta;
                    break;
                case 'a':
                    m.x = -delta;
                    break;
                case 'd':
                    m.x = delta;
                    break;
            }
        }
        if (playerNum == 2) {
            switch (move) {
                case 'i':
                    m.y = -delta;
                    break;
                case 'k':
                    m.y = delta;
                    break;
                case 'j':
                    m.x = -delta;
                    break;
                case 'l':
                    m.x = delta;
            }
        }
        velocityX = m.x;
        velocityY = m.y;
        return m;
    }
    using Player::setX;
    using Player::setY;
    using Player::getX;
    using Player::getY;
    using Player::getType;
    using Player::getVelocityX;
    using Player::getVelocityY;
    using Player::setFrame;

private:
    int playerNum;
    const int delta = 15;
    int moves;

};



#endif //KEYBOARDPLAYER_H
