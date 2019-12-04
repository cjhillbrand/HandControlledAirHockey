//
// Created by CJ Hillbrand on 2019-11-18.
//

// This player extends the abstract player class.

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

    // Constructors
    KeyboardPlayer() : Player() {
        type = 1;
    };
    KeyboardPlayer(int player) {
        playerNum = player;
        moves = -1;
        type = 1;
    };
    KeyboardPlayer(int x, int y) : Player(x, y) {}

    // Given a char. Returns the move that it will make.
    // If this player is on the left side of the field, it will only handle 'wasd' keys
    // If this player in on the right side of the field it will only handle 'ijkl; keys
    Movement getMove(char move) {
        Movement m = {0, 0};
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

    // Methods inherited by the Player class.
    using Player::setX;
    using Player::setY;
    using Player::getX;
    using Player::getY;
    using Player::getType;
    using Player::getVelocityX;
    using Player::getVelocityY;
    using Player::setFrame;

private:
    int playerNum;      // Denotes if we are on the left or right side
    const int delta = 15;   // The rate of change for one key press
    int moves;          // How many moves have been made

};



#endif //KEYBOARDPLAYER_H
