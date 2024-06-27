#ifndef SNAKE_H
#define SNAKE_H

#include <vector>

enum class Direction
{
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3,
};

class SnakeBody
{
public:
    SnakeBody();
    SnakeBody(int x, int y);
    int getX() const;
    int getY() const;
    const bool operator == (const SnakeBody& snakeBody);
private:
    int mX;
    int mY;
};

class Food
{
public:
    Food (SnakeBody Pos);
    SnakeBody getPos() const;
private:
    SnakeBody mPos;
};

// Snake class should have no depency on the GUI library
class Snake
{
public:
    //Snake();
    Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength);
    // Set random seed
    void setRandomSeed();
    // Initialize snake
    void initializeSnake();
    // Checking API for generating random food
    bool isPartOfSnake(int x, int y);
    void senseFood(const std::vector<Food> &food);
    bool touchFood();
    // Check if the snake is dead
    bool hitWall();
    bool hitSelf();
    bool checkCollision();

    bool changeDirection(Direction newDirection);
    std::vector<SnakeBody>& getSnake();
    int getLength();
    SnakeBody createNewHead();
    void moveFoward(bool killTail);

private:
    const int mGameBoardWidth;
    const int mGameBoardHeight;
    // Snake information
    const int mInitialSnakeLength;
    Direction mDirection;
    std::vector<Food> mFood;
    std::vector<SnakeBody> mSnake;
};

#endif
