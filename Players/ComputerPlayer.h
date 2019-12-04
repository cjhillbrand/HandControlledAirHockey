//
// Created by CJ Hillbrand on 2019-11-17.
//

// Computer player, which extends the player class. This player behaves very deterministically
// It will close the gap between itself and the puck if the puck is on its side. If the puck is
// on the other side of the midline it will move towards a starting position.

#ifndef COMPUTERPLAYER_H
#define COMPUTERPLAYER_H
#include <iostream>
#include "Player.h"
#include <stdlib.h>
using namespace std;

class ComputerPlayer : public Player {
public:
    // Constructors
    ComputerPlayer() : Player() {
        type = 2;
    };
    ComputerPlayer(int x, int y) : Player(x, y) {}

    ComputerPlayer(int playerNum) : Player(), playerNum(playerNum) {}

    // Given a char, it will return the move it will make. It will attempt to hit the
    // puck if it is on its side of the field. If not it will return to the starting pos.
    Movement getMove(char move) {
        // if the puck is on the other side of the field
        // Player 1
        Movement m;
        if (puckX > FRAME_WIDTH / 2 && playerNum == 1) {
            m.x = (FRAME_WIDTH / 4 - x) / DAMPENING;
            m.y = (FRAME_HEIGHT / 2 - y) / DAMPENING;
        }
        // Player 2
        else if (puckX < FRAME_WIDTH / 2 && playerNum == 2) {
            m.x = (FRAME_WIDTH * 3 / 4 - x) / DAMPENING;
            m.y = (FRAME_HEIGHT / 2 - y) / DAMPENING;
        } else {
            m.x = (puckX - x) / DAMPENING;
            m.y = (puckY - y) / DAMPENING;
        }
        x += m.x;
        y += m.y;
        return m;
    }

    // Methods inherited from Player
    using Player::setX;
    using Player::setY;
    using Player::getX;
    using Player::getY;
    using Player::setFrame;
private:
    int playerNum;              // Denotes which side of the field we are on

    const int DAMPENING = 15;   // Slows the movement of the paddle so that it is able to
                                // to be defeated.
};


#endif //FINALPROJECT487_COMPUTERPLAYER_H
