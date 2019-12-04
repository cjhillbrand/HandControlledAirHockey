//
// Created by CJ Hillbrand on 2019-11-17.
//

#ifndef PLAYER_H
#define PLAYER_H
class Player {
public:
    Player(int x, int y) : x(x), y(y) {};
    Player() {
        velocityX = 0;
        velocityY = 0;
    };
    virtual ~Player() {};
    struct Movement {
        int x;
        int y;
    };
    virtual Movement getMove(char) = 0;
    int getX() {return x;}
    int getY() {return y;}
    void setX(int val) {x = val;}
    void setY(int val) {y = val;}
    void setFrame(const int width, const int height) {
        FRAME_WIDTH = width;
        FRAME_HEIGHT = height;
    }
    int getType() {return type;}
    int getVelocityX() {return velocityX;}
    int getVelocityY() {return velocityY;}
    // We can set the pucks dimension in the computer player since
    // were not making an AI to play air hockey.
    int puckX;
    int puckY;
    int x;
    int y;
    int velocityX;
    int velocityY;
    int type;
    int FRAME_WIDTH;
    int FRAME_HEIGHT;

};



#endif //PLAYER_H
