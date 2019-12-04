//
// Created by CJ Hillbrand on 2019-11-08.
//

#include "AirHockeyEngine.h"
AirHockeyEngine::AirHockeyEngine(const int size, const int videoNum) : frame(Mat_<uchar>(size, size * 2)), size(size),
    p1(new ComputerPlayer(1)) {
    p2 = videoNum == -1 ? new CameraPlayer() :
            new CameraPlayer("./HandProcessing/videos/airhockey_test_" + to_string(videoNum) + ".mov", videoNum);
    p1 -> setX(size / 4);
    p1 -> setY(size / 2 - PADDLE_DIM / 2);
    p2 -> setX(size * 7 / 4 - PADDLE_DIM / 2);
    p2 -> setY(size / 2 - PADDLE_DIM / 2);
    p1 -> setFrame(size * 2, size);
    p2 -> setFrame(size * 2, size);
    puck = {
        .x = p1 -> getX() + PADDLE_DIM * 2,
        .y = p1 -> getY() + PADDLE_DIM / 2 - PUCK_DIM / 2
    };
    recorder.open("./LastGame.mov",
                  VideoWriter::fourcc('m', 'p', '4', 'v'), 20,
                  Size(frame.cols, frame.rows), false);
    gameover = false;
    scoreOne = 0;
    scoreTwo = 0;
    fillFrame();
    updateState();
}

AirHockeyEngine::AirHockeyEngine() : AirHockeyEngine(DEFAULT_SIZE, -1) {}

AirHockeyEngine::AirHockeyEngine(const int videoNum) : AirHockeyEngine(DEFAULT_SIZE, videoNum) {}

AirHockeyEngine::~AirHockeyEngine() {
    delete p1;
    delete p2;
}

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

void AirHockeyEngine::forceQuit() {
    recorder.release();
    gameover = true;
}

void AirHockeyEngine::resetState(const int scored) {
    p1 -> setX(size / 4);
    p1 -> setY(size / 2 - PADDLE_DIM / 2);
    p2 -> setX(size * 7 / 4 - PADDLE_DIM / 2);
    p2 -> setY(size / 2 - PADDLE_DIM / 2);
    Player *p = scored == 1 ? p1 : p2;
    int offset = scored == 1 ? PADDLE_DIM * 2 : -PADDLE_DIM * 2;
    puck = {
            .x = p -> getX() + offset,
            .y = p -> getY() + PADDLE_DIM / 2 - PUCK_DIM / 2,
            .velocityX = 0,
            .velocityY = 0
    };
    p1 -> velocityX = 0;
    p1 -> velocityY = 0;
    p2 -> velocityX = 0;
    p2 -> velocityY = 0;
}

bool AirHockeyEngine::createFrame() {
    namedWindow("Air Hockey", WINDOW_NORMAL);
    //waitKey(5);
    return true;
}

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
    Player::Movement returnVal = { .x = oldX, .y = oldY};
    return returnVal;
}

void AirHockeyEngine::updatePuck(Player::Movement& oldPaddleOne, Player::Movement& oldPaddleTwo,
        Player::Movement& newPaddleOne, Player::Movement& newPaddleTwo) {
    //Update puck state
    int newPuckX = puck.x + puck.velocityX;
    int newPuckY = puck.y + puck.velocityY;

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
    if (moveCount % 5 == 0) {
        puck.velocityX = puck.velocityX * DAMPENING;
        puck.velocityY = puck.velocityY * DAMPENING;
    }

}

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

void AirHockeyEngine::updateState() {
    Rect paddleOne(p1 -> getX(), p1 -> getY(), PADDLE_DIM, PADDLE_DIM);
    Rect paddleTwo(p2 -> getX(), p2 -> getY(), PADDLE_DIM, PADDLE_DIM);
    rectangle(frame, paddleOne, WHITE);
    rectangle(frame, paddleTwo, WHITE);

    Rect puckRect(puck.x, puck.y, PUCK_DIM, PUCK_DIM);
    rectangle(frame, puckRect, WHITE, FILLED);
}

bool AirHockeyEngine::showFrame() {
    Player::Movement oldPaddleOne = { .x = p1 -> getX(), .y = p1 -> getY()};
    Player::Movement newPaddleOne = { .x = p1 -> getX(), .y = p1 -> getY()};
    Player::Movement oldPaddleTwo = { .x = p2 -> getX(), .y = p2 -> getY()};
    Player::Movement newPaddleTwo = { .x = p2 -> getX(), .y = p2 -> getY()};

    moveCount++;
    char move = waitKey(5);
    if (move == 'q') {
        forceQuit();
    }

    if ((move == 'a' || move == 's' || move == 'd' || move == 'w') && p1->getType() == KEYBOARD_PLAYER) {
        moves.push_back(move);
        newPaddleOne = makeMove(1);
    } else if ((move == 'i' || move == 'j' || move == 'k' || move == 'l') && p2->getType() == KEYBOARD_PLAYER) {
        moves.push_back(move);
        newPaddleTwo = makeMove(2);
    }
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


