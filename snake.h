#ifndef SNAKE_H
#define SNAKE_H

#include <vector>

class Game;

enum class Direction
{
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3,
};
Direction directionLeft(Direction dir);
Direction directionRight(Direction dir);

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
    Food (SnakeBody Pos, bool flag);
    SnakeBody getPos() const;
    bool isRealFood() const;
private:
    SnakeBody mPos;
    bool RealFood;
};

// Snake class should have no depency on the GUI library
class Snake
{
    friend class Game;
    friend class EnemySnake;
public:
    //Snake();
    Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength, Game* game);
    // Initialize snake
    virtual void initializeSnake();
    // Checking API for generating random food
    bool isPartOfSnake(int x, int y);
    void senseFood(const std::vector<Food> &food);
    bool touchFood();
    // Check if the snake is dead
    bool hitWall();
    bool hitSnake(Snake *othersnake);
    bool checkCollision();

    bool changeDirection(Direction newDirection);
    std::vector<SnakeBody>& getSnake();
    int getLength();
    SnakeBody createNewHead();
    void moveFoward(bool killTail);
    
    virtual void EnemySnakeAI();
private:
    Game *thisgame;

    // i hate private
    const int mGameBoardWidth;
    const int mGameBoardHeight;
    const int mInitialSnakeLength;
    // Snake information
    Direction mDirection;
    std::vector<Food> mFood;
    std::vector<SnakeBody> mSnake;
};

class EnemySnake : public Snake {
    friend class Game;
    friend class Snake;
public:
    void initializeSnake();
    EnemySnake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength, Game* game);
    // bool checkCollision();
    bool findFoodLine(Direction dir);
    bool findFoodHalfPlane(Direction dir);
    void EnemySnakeAI();
    void SnakeAI_Greed(Direction F, Direction L, Direction R);
    void SnakeAI_NoHitSelf(Direction F, Direction L, Direction R);
private:
};

#endif
