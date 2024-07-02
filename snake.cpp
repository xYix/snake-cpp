#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "snake.h"

#include <vector>

class Game;

// Direction

Direction directionLeft(Direction dir) {
    if (dir == Direction::Up) return Direction::Left;
    if (dir == Direction::Down) return Direction::Right;
    if (dir == Direction::Left) return Direction::Down;
    if (dir == Direction::Right) return Direction::Up;
}
Direction directionRight(Direction dir) {
    if (dir == Direction::Up) return Direction::Right;
    if (dir == Direction::Down) return Direction::Left;
    if (dir == Direction::Left) return Direction::Up;
    if (dir == Direction::Right) return Direction::Down;
}

// Food

Food::Food(SnakeBody Pos) : mPos(Pos), RealFood(true) {};
Food::Food(SnakeBody Pos, bool flag) : mPos(Pos), RealFood(flag) {};
SnakeBody Food::getPos() const {
    return mPos;
}
bool Food::isRealFood() const { return RealFood; }


// SnakeBody

SnakeBody::SnakeBody()
{
}


SnakeBody::SnakeBody(int x, int y): mX(x), mY(y)
{
}

int SnakeBody::getX() const
{
    return mX;
}

int SnakeBody::getY() const
{
    return mY;
}

const bool SnakeBody::operator == (const SnakeBody& snakeBody)
{
    // overload the == operator for SnakeBody comparision.
    if (this->getX() == snakeBody.getX() && this->getY() == snakeBody.getY())
        return true;
    return false;
}

// Snake

Snake::Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength, Game* game):
    mGameBoardWidth(gameBoardWidth), mGameBoardHeight(gameBoardHeight), mInitialSnakeLength(initialSnakeLength), thisgame(game) {
    this->initializeSnake();
}

void Snake::initializeSnake()
{
    // Instead of using a random initialization algorithm
    // We always put the snake at the center of the game mWindows
    int centerX = this->mGameBoardWidth / 2;
    int centerY = this->mGameBoardHeight / 2;

    for (int i = 0; i < this->mInitialSnakeLength; i ++)
    {
        this->mSnake.push_back(SnakeBody(centerX, centerY + i));
    }
    this->mDirection = Direction::Up;
}

bool Snake::isPartOfSnake(int x, int y)
{
    // check if a given point with axis x, y is on the body of the snake.
    for (SnakeBody &i : this->mSnake)
        if (i == SnakeBody(x, y))
            return true;
    return false;
}

/*
 * Assumption:
 * Only the head would hit wall.
 */
bool Snake::hitWall()
{
	// check if the snake has hit the wall
    SnakeBody u = this->createNewHead();
    if (u.getX() < 1 || u.getX() >= this->mGameBoardWidth - 1) return true;
    if (u.getY() < 1 || u.getY() >= this->mGameBoardHeight - 1) return true;
    return false;
}

/*
 * The snake head is overlapping with othersnake's body
 */
bool Snake::hitSnake(Snake *othersnake)
{
	// check if the snake has hit itself.
    SnakeBody u = this->createNewHead();
    if (othersnake->isPartOfSnake(u.getX(), u.getY()))
        return true;
    return false;
}

/*
 * The snake head is touching food
 */
bool Snake::touchFood()
{
    SnakeBody newHead = this->createNewHead();
    for (const Food &i : this->mFood)
        if (newHead == i.getPos())
            return true;
    return false;
}

void Snake::senseFood(const std::vector<Food> &food)
{
    this->mFood = food;
}

std::vector<SnakeBody>& Snake::getSnake()
{
    return this->mSnake;
}

bool Snake::changeDirection(Direction newDirection)
{
    if (this->mDirection == Direction::Up && newDirection == Direction::Down) return false;
    if (this->mDirection == Direction::Down && newDirection == Direction::Up) return false;
    if (this->mDirection == Direction::Left && newDirection == Direction::Right) return false;
    if (this->mDirection == Direction::Right && newDirection == Direction::Left) return false;
    this->mDirection = newDirection;
    return true;
}


SnakeBody Snake::createNewHead()
{
    /* 
    * read the position of the current head
    * read the current heading direction 
    * add the new head according to the direction
    * return the new snake
    */
    SnakeBody curHead = this->mSnake[0];
    int curX = curHead.getX(), curY = curHead.getY();
    SnakeBody newHead;
    switch (this->mDirection) {
        case Direction::Up:
            newHead = SnakeBody(curX, curY - 1); break;
        case Direction::Down:
            newHead = SnakeBody(curX, curY + 1); break;
        case Direction::Left:
            newHead = SnakeBody(curX - 1, curY); break;
        case Direction::Right:
            newHead = SnakeBody(curX + 1, curY); break;
    }
    return newHead;
}

/*
 * If eat food, return true, otherwise return false
 */
void Snake::moveFoward(bool killTail)
{
    /* 
	 * move the snake forward. 
     */
    SnakeBody newHead = this->createNewHead();
    this->mSnake.insert(mSnake.begin(), newHead);
    if (killTail)
        this->mSnake.pop_back();
}

int Snake::getLength()
{
    return this->mSnake.size();
}

void Snake::EnemySnakeAI()
{
    // do nothing
}

// EnemySnake
EnemySnake::EnemySnake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength, Game *game): 
    Snake(gameBoardWidth, gameBoardHeight, initialSnakeLength, game) {
        this->mSnake.clear();
        this->initializeSnake();
    }

bool EnemySnake::findFoodLine(Direction dir) {
    SnakeBody curHead = this->mSnake[0];
    for (Food &i : this->mFood) {
        SnakeBody pos = i.getPos();
        switch (dir) {
            case Direction::Up:
                if (pos.getX() == curHead.getX() && pos.getY() < curHead.getY())
                    return true;
                break;
            case Direction::Down:
                if (pos.getX() == curHead.getX() && pos.getY() > curHead.getY())
                    return true;
                break;
            case Direction::Left:
                if (pos.getY() == curHead.getY() && pos.getX() < curHead.getX())
                    return true;
                break;
            case Direction::Right:
                if (pos.getY() == curHead.getY() && pos.getX() > curHead.getX())
                    return true;
                break;
        }
    }
    return false;
}
bool EnemySnake::findFoodHalfPlane(Direction dir) {
    SnakeBody curHead = this->mSnake[0];
    for (Food &i : this->mFood) {
        SnakeBody pos = i.getPos();
        switch (dir) {
            case Direction::Up:
                if (pos.getY() < curHead.getY())
                    return true;
                break;
            case Direction::Down:
                if (pos.getY() > curHead.getY())
                    return true;
                break;
            case Direction::Left:
                if (pos.getX() < curHead.getX())
                    return true;
                break;
            case Direction::Right:
                if (pos.getX() > curHead.getX())
                    return true;
                break;
        }
    }
    return false;
}
void EnemySnake::EnemySnakeAI() {
    Direction F = this->mDirection,
              L = directionLeft(this->mDirection),
              R = directionRight(this->mDirection);
    this->SnakeAI_Greed(F, L, R);
    this->SnakeAI_NoHitSelf(F, L, R);
}
void EnemySnake::SnakeAI_Greed(Direction F, Direction L, Direction R) {
    if (this->findFoodLine(this->mDirection)) return;
    if (this->findFoodLine(L)) {
        this->mDirection = L;
        return;
    }
    if (this->findFoodLine(R)) {
        this->mDirection = R;
        return;
    }
    if (!this->findFoodHalfPlane(this->mDirection))
        this->mDirection = L;
}
void EnemySnake::SnakeAI_NoHitSelf(Direction F, Direction L, Direction R) {
    if (this->hitSnake(this) || this->hitWall()) {
        this->mDirection = L;
        if (!this->hitSnake(this) && !this->hitWall()) return;
        this->mDirection = R;
        if (!this->hitSnake(this) && !this->hitWall()) return;
        this->mDirection = F; // gg
        return;
    }
}