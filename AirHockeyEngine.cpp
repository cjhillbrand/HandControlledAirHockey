//
// Created by CJ Hillbrand on 2019-11-08.
//

// The object that controls the state of the air hockey game. To play one must create this object
// and then continually prompt the object to show the next frame with showFrame() the object should 
// be released when the game is over, by asking gameOver() if the game is over.

#include "AirHockeyEngine.h"

// Precondition: A int greater than 0 to construct the Matrix that will display the game. Also a video number that determines
//              which video file number the game will be recorded to.
// Postcondtion: Constructs the Air Hockey game. Everything will be initialed to a beginning state.
AirHockeyEngine::AirHockeyEngine(const int size, const int videoNum) : frame(Mat_<uchar>::zeros(size, size * 2)), size(size),
    p1(new ComputerPlayer(1)) {
    p2 = videoNum == -1 ? new CameraPlayer() :
            new CameraPlayer(PATH_TO_VIDEO + "airhockey_test_" + to_string(videoNum) + ".mov", videoNum);
    p1 -> setX(size / 4);
    p1 -> setY(size / 2 - PADDLE_DIM / 2);
    p2 -> setX(size * 7 / 4 - PADDLE_DIM / 2);
    p2 -> setY(size / 2 - PADDLE_DIM / 2);
    p1 -> setFrame(size * 2, size);
    p2 -> setFrame(size * 2, size);
    puck.x = p1 -> getX() + PADDLE_DIM * 2;
    puck.y = p1->getY() + PADDLE_DIM / 2 - PUCK_DIM / 2;
 
    recorder.open(PATH_TO_VIDEO + "LastGame.mov",
                  VideoWriter::fourcc('m', 'p', '4', 'v'), 20,
                  Size(frame.cols, frame.rows), false);
    gameover = false;
    scoreOne = 0;
    scoreTwo = 0;
    fillFrame();
    updateState();
}

// Preconditon: N/A
// Postcondition: Empty constructor, constructs the Air Hockey Engine with a default size, and
//                using the video fed in from a live camera.
AirHockeyEngine::AirHockeyEngine() : AirHockeyEngine(DEFAULT_SIZE, -1) {}

// Precondition: A video number that is valid and exists.
// Postcondition: Creates an Air Hockey Engine with default size and with the feed coming from the video
//                 supplied in from the .mov file.
AirHockeyEngine::AirHockeyEngine(const int videoNum) : AirHockeyEngine(DEFAULT_SIZE, videoNum) {}

// Destructor for the Air Hockey Engine. Destroys the two player pointers.
AirHockeyEngine::~AirHockeyEngine() {
    delete p1;
    delete p2;
}

// Precondition: N/A
// Postcondition: Returns if the condition to end the game is satsified. If so close properly and state to 
//                gameover.
bool AirHockeyEngine::gameOver() {
    if(scoreTwo == 10 || scoreOne == 10) {
        scoreOne = 0;
        scoreTwo = 0;
        recorder.release();
        gameover = true;
        return gameover;
    }
    return gameover;
}

// Precondition: N/A
// Postcondition: Ends the game.
void AirHockeyEngine::forceQuit() {
    recorder.release();
    gameover = true;
}

// Precondition: Gives a value of 1 or 2 two signify who the last person was to score.
// Postcondition: Resets the state to the beginning state except for one condition:
//                1. The puck will be placed on the side of who just scored.
void AirHockeyEngine::resetState(const int scored) {
    p1 -> setX(size / 4);
    p1 -> setY(size / 2 - PADDLE_DIM / 2);
    p2 -> setX(size * 7 / 4 - PADDLE_DIM / 2);
    p2 -> setY(size / 2 - PADDLE_DIM / 2);
    Player *p = scored == 1 ? p1 : p2;
    int offset = scored == 1 ? PADDLE_DIM * 2 : -PADDLE_DIM * 2;
    puck.x = p->getX() + offset;
    puck.y = p->getY() + PADDLE_DIM / 2 - PUCK_DIM / 2;
    puck.velocityX = 0;
    puck.velocityY = 0;

    p1 -> velocityX = 0;
    p1 -> velocityY = 0;
    p2 -> velocityX = 0;
    p2 -> velocityY = 0;
}

// Preconditon: N/A
// Postcondition: Creates the window in which to display the game. Returns true
//                to denote the successful creation.
bool AirHockeyEngine::createFrame() {
    namedWindow("Air Hockey", WINDOW_NORMAL);
    //waitKey(5);
    return true;
}

// Precondtion: N/A
// Postcondition: Draws all of the static objects on the field.
void AirHockeyEngine::fillFrame() {
    // Middle Line
    line(frame, Point(frame.cols / 2, frame.rows), Point(frame.cols / 2, 0), WHITE, MIDDLE_WIDTH * 2);

    // Goal One
    Rect goalOne(0, frame.rows / 3, GOAL_WIDTH, GOAL_HEIGHT);
    rectangle(frame, goalOne, WHITE, 4);

    // Goal Two
    Rect goalTwo(frame.cols - frame.cols / 12, frame.rows / 3, GOAL_WIDTH, GOAL_HEIGHT);
    rectangle(frame, goalTwo, WHITE, 4);

    // Initialize scores
    putText(frame, to_string(scoreOne), Point(0, size - 10), FONT_HERSHEY_SIMPLEX, 2, WHITE, 4);
    putText(frame, to_string(scoreTwo), Point(size * 2 - 40, size - 10), FONT_HERSHEY_SIMPLEX, 2, WHITE, 4);
}


// Preconditon: Gives a number 1 or 2. 
// Postcondition: Prompts the player to return a movement. The player is then updated
//                to the new state
Player::Movement AirHockeyEngine::makeMove(const int player) {
    Player *temp = player == 1 ? p1 : p2;
    temp -> puckX = puck.x;
    temp -> puckY = puck.y;
    deleteOldState();
    char move = -1;
    if (!moves.empty()) {
        move = moves.back();
        moves.pop_back();
    }
    const Player::Movement m = temp -> getMove(move);

    if (m.x == EXIT_COND)
        forceQuit();

    const int x = m.x;
    const int y = m.y;
    // Check to make sure not moving out of bounds.
    int newX = temp -> getX() + x;
    int newY = temp -> getY() + y;
    int oldX = temp -> getX();
    int oldY = temp -> getY();

    if (newX < 0)
        newX = 0;
    if (newX + PADDLE_DIM >= size * 2)
        newX = size * 2 - PADDLE_DIM;
    if (newY < 0)
        newY = 0;
    if (newY + PADDLE_DIM + MIDDLE_WIDTH >= size)
        newY = size - PADDLE_DIM - MIDDLE_WIDTH;
    if (newX + PADDLE_DIM + MIDDLE_WIDTH >= size && player == 1)
        newX = size - PADDLE_DIM - MIDDLE_WIDTH;
    if (newX < size + MIDDLE_WIDTH && player == 2)
        newX = size + MIDDLE_WIDTH;

    temp -> setX(newX);
    temp -> setY(newY);

    updateState();
    fillFrame();
    Player::Movement returnVal = { oldX, oldY };

    return returnVal;
}

// Preconditon: Gives four structs of Movement which signifies the nth move of each player and the nth + 1 move.
// Postcondition: Updates the puck movement relative to the player positions.
void AirHockeyEngine::updatePuck(Player::Movement& oldPaddleOne, Player::Movement& oldPaddleTwo,
        Player::Movement& newPaddleOne, Player::Movement& newPaddleTwo) {
    //Update puck state
    int newPuckX = static_cast<int>(puck.x + puck.velocityX);
    int newPuckY = static_cast<int>(puck.y + puck.velocityY);

    // Must check if the paddle hit the puck
    checkPaddlePuckCollision(1, puck.x, puck.y, oldPaddleOne.x, oldPaddleOne.y, newPuckX, newPuckY);
    checkPaddlePuckCollision(2, puck.x, puck.y, oldPaddleTwo.x, oldPaddleTwo.y, newPuckX, newPuckY);

    // Need to check if hit wall.
    if (newPuckX <= 0) {
        newPuckX = 0;
        puck.velocityX = -puck.velocityX;
        if (newPuckY < GOAL_HEIGHT * 2 && newPuckY > GOAL_HEIGHT) {
            scoreTwo++;
            resetState(2);
            newPuckX = puck.x;
            newPuckY = puck.y;
        }
    }
    if (newPuckX + PUCK_DIM>= size * 2) {
        newPuckX = size * 2 - PUCK_DIM;
        puck.velocityX = -puck.velocityX;
        if (newPuckY < GOAL_HEIGHT * 2 && newPuckY > GOAL_HEIGHT) {
            scoreOne++;
            resetState(1);
            newPuckX = puck.x;
            newPuckY = puck.y;
        }
    }
    if (newPuckY <= 0) {
        newPuckY = 0;
        puck.velocityY = -puck.velocityY;
    }
    if (newPuckY + PUCK_DIM >= size) {
        newPuckY = size - PUCK_DIM;
        puck.velocityY = -puck.velocityY;
    }
    puck.x = newPuckX;
    puck.y = newPuckY;

    // Every 5 moves move the puck relative to its velocity.
    if (moveCount % 5 == 0) {
        puck.velocityX = puck.velocityX * DAMPENING;
        puck.velocityY = puck.velocityY * DAMPENING;
    }

}

// Precondition: Receive which player to check the collison for, the old puck position and the new alleged puck position
//               and the old position of the paddle
// Postcondition: Moves the puck such that if there was a collision it will move in the opposite direction.
void AirHockeyEngine::checkPaddlePuckCollision(int paddle, int puckX, int puckY, int oldPaddleX, int oldPaddleY, int newPuckX, int newPuckY) {
    Player* p = paddle == 1 ? p1 : p2;
    int newPaddleX = p -> getX();
    int newPaddleY = p -> getY();

    // Case 1 Left hit
    if (puckX + PUCK_DIM <= oldPaddleX && newPuckX + PUCK_DIM >= newPaddleX &&
        (newPuckY - PUCK_DIM <= newPaddleY + PADDLE_DIM && newPuckY + PUCK_DIM * 2 >= newPaddleY)) {
        puck.velocityX = -puck.velocityX + (newPaddleX - oldPaddleX) * INIT_VELOCITY;
        puck.velocityY = (newPaddleY - oldPaddleY) + puck.velocityY;
    }

    // Case 2 Right hit
    if (oldPaddleX + PADDLE_DIM <= puckX && newPaddleX + PADDLE_DIM >= newPuckX &&
        (newPuckY - PUCK_DIM <= newPaddleY + PADDLE_DIM && newPuckY + PUCK_DIM * 2 >= newPaddleY)) {
        puck.velocityX = -puck.velocityX + (newPaddleX - oldPaddleX) * INIT_VELOCITY;
        puck.velocityY = (newPaddleY - oldPaddleY) + puck.velocityY;
    }

    // Case 3 Above hit
    if (puckY + PUCK_DIM <= oldPaddleY && newPuckY + PUCK_DIM >= newPaddleY &&
        (newPuckX <= newPaddleX + PADDLE_DIM && newPuckX + PUCK_DIM >= newPaddleX)) {
        puck.velocityY = -puck.velocityY + (newPaddleY - oldPaddleY) * INIT_VELOCITY;
        puck.velocityX = (newPaddleX - oldPaddleX) + puck.velocityX;
    }

    // Case 4 Below Hit
    if (puckY >= oldPaddleY + PADDLE_DIM && newPuckY <= newPaddleY + PADDLE_DIM &&
        (newPuckX <= newPaddleX + PADDLE_DIM && newPuckX + PUCK_DIM >= newPaddleX)) {
        puck.velocityY = -puck.velocityY + (newPaddleY - oldPaddleY) * INIT_VELOCITY;
        puck.velocityX = (newPaddleX - oldPaddleX) + puck.velocityX;
    }
    p -> velocityX = 0;
    p -> velocityY = 0;
}

// Precondition: N/A
// Postcondition: deletes the state of the paddles and pucks
void AirHockeyEngine::deleteOldState() {
    Rect paddleOne(p1 -> getX(), p1 -> getY(), PADDLE_DIM, PADDLE_DIM);
    Rect paddleTwo(p2 -> getX(), p2 -> getY(), PADDLE_DIM, PADDLE_DIM);
    rectangle(frame, paddleOne, BLACK);
    rectangle(frame, paddleTwo, BLACK);

    Rect puckRect(puck.x, puck.y, PUCK_DIM, PUCK_DIM);
    rectangle(frame, puckRect, BLACK, FILLED);

    putText(frame, to_string(scoreOne), Point(0, size - 10), FONT_HERSHEY_SIMPLEX, 2, BLACK, 4);
    putText(frame, to_string(scoreTwo), Point(size * 2 - 40, size - 10), FONT_HERSHEY_SIMPLEX, 2, BLACK, 4);
}

// Preconditon: N/A
// Postconditon: Draws the paddles and pucks onto the frame.
void AirHockeyEngine::updateState() {
    Rect paddleOne(p1 -> getX(), p1 -> getY(), PADDLE_DIM, PADDLE_DIM);
    Rect paddleTwo(p2 -> getX(), p2 -> getY(), PADDLE_DIM, PADDLE_DIM);
    rectangle(frame, paddleOne, WHITE);
    rectangle(frame, paddleTwo, WHITE);

    Rect puckRect(puck.x, puck.y, PUCK_DIM, PUCK_DIM);
    rectangle(frame, puckRect, WHITE, FILLED);
}

// Precondition: N/A 
// Postcondition: Prompts both players for a move. Deletes the old state and then calculates the new
//                state of the game.
bool AirHockeyEngine::showFrame() {
    Player::Movement oldPaddleOne;
    Player::Movement newPaddleOne;
    Player::Movement oldPaddleTwo;
    Player::Movement newPaddleTwo;
    oldPaddleOne.x = p1->getX();
    oldPaddleOne.y = p1->getY();
    newPaddleOne.x = p1->getX();
    newPaddleOne.y = p1->getY();
    oldPaddleTwo.x = p2->getX();
    oldPaddleTwo.y = p2->getY();
    newPaddleTwo.x = p2->getX();
    newPaddleTwo.y = p2->getY();
    moveCount++;
    char move = waitKey(5);

    // Check if player wants to force quit
    if (move == 'q') {
        forceQuit();
    }

    // If there is a keyboard player we need to handle their keyboard input
    if ((move == 'a' || move == 's' || move == 'd' || move == 'w') && p1->getType() == KEYBOARD_PLAYER) {
        moves.push_back(move);
        newPaddleOne = makeMove(1);
    } else if ((move == 'i' || move == 'j' || move == 'k' || move == 'l') && p2->getType() == KEYBOARD_PLAYER) {
        moves.push_back(move);
        newPaddleTwo = makeMove(2);
    }

    // If theyre not a keyboard player we need to still prompt for a move
    if (p1->getType() != KEYBOARD_PLAYER)
        newPaddleOne = makeMove(1);
    if (p2->getType() != KEYBOARD_PLAYER)
        newPaddleTwo = makeMove(2);


    imshow("Air Hockey", frame);
    deleteOldState();
    updatePuck(oldPaddleOne, oldPaddleTwo, newPaddleOne, newPaddleTwo);
    updateState();
    fillFrame();
    recorder.write(frame);

    return true;
}


