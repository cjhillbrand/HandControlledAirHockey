//
// Created by CJ Hillbrand on 2019-11-17.
//


// Abstract class used for the player objects.
// The method that must be implemented by concrete classes are the getMove method
// This class also implements some basic getters and setters.
#ifndef PLAYER_H
#define PLAYER_H
class Player {
public:
    // Constructors for the Player class
    Player(int x, int y) : x(x), y(y) {};
    Player() {
        velocityX = 0;
        velocityY = 0;
    };
    // Destructor
    virtual ~Player() {};

    // Stores x y coordinates.
    struct Movement {
        int x;
        int y;
    };

    // Given a char, what is the movement that will be preformed?
    virtual Movement getMove(char) = 0;

    // Returns current x coordinate
    int getX() {return x;}

    // Returns current y cordinate
    int getY() {return y;}

    // Sets currents x coordinate
    void setX(int val) {x = val;}

    // Sets current y coordinate
    void setY(int val) {y = val;}

    // Sets the frame in which the player is in
    void setFrame(const int width, const int height) {
        FRAME_WIDTH = width;
        FRAME_HEIGHT = height;
    }
    
    // Returns the specific type of player
    int getType() {return type;}

    // Returns the velocity of the player in x
    int getVelocityX() {return velocityX;}

    // Returns the velocity of the player in y
    int getVelocityY() {return velocityY;}

    // We can set the pucks dimension in the computer player since
    // were not making an AI to play air hockey.
    int puckX;
    int puckY;
    int x;      // Current x position
    int y;      // Current y position
    int velocityX;  // Current x velocity
    int velocityY;  // Current y velocity
    int type;       // Type of player
    int FRAME_WIDTH;    // Frame width
    int FRAME_HEIGHT;   // Frame height

};



#endif //PLAYER_H
