#include <string>
#include <iostream>
#include <cmath> 

// For terminal delay
#include <chrono>
#include <thread>

#include "game.h"

#define COLOR_36C48E 8

#define FOOD_COLOR 1
#define SNAKE_COLOR 2
#define DEFAULT_COLOR 3
#define ENEMY_SNAKE_COLOR 4

void Game::initializeGame()
{
    // allocate memory for a new snake
	this->mPtrSnake.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength, this));
    for (auto &s : this->mPtrEnemySnake)
        s.reset(nullptr);
    this->mBossSnake.reset(nullptr);

    /* 
     * initialize the game points as zero
     * create a food at random place
     * make the snake aware of the food
     * other initializations
     */
    this->mPoints = 0;
    this->mDifficulty = 0;
    this->mFood.clear();
    for (int i = 0; i < this->mFoodCount; i++)
        this->createRandomFood();
    this->mPtrSnake->senseFood(this->mFood);
    this->adjustDifficulty();
    this->animationTick = -1;
    // ...
}
/* 
 * create a food at random places
 * make sure that the food doesn't overlap with the snake.
 */
void Game::createRandomFood()
{
    if (this->mDifficulty <= 5) {
        int x, y;
        bool flag = true;
        while (flag) {
            x = 1 + rand() % (this->mGameBoardWidth - 2),
            y = 1 + rand() % (this->mGameBoardHeight - 2);
            if (this->mPtrSnake->isPartOfSnake(x, y)) continue;
            for (auto &s : this->mPtrEnemySnake)
                if (s && s->isPartOfSnake(x, y)) continue;
            flag = false;
            for (const Food &i : this->mFood)
                if (i.getPos() == SnakeBody(x, y)) {
                    flag = true; break;
                }
        }
        /*
        ? When whole screen is fully occupied by snake
        */
        this->mFood.push_back(Food(SnakeBody(x, y)));
    }
    else {
        int x, y;
        bool flag = true;
        while (flag) {
            flag = false;
            x = 2 + rand() % (this->mGameBoardWidth - 3),
            y = 2 + rand() % (this->mGameBoardHeight - 3); // not on the corner
            for (int dx = -1; dx <= 1; dx++)
            for (int dy = -1; dy <= 1; dy++) {
                if (this->mPtrSnake->isPartOfSnake(x + dx, y + dy)) 
                    { flag = true; break; }
                if (this->mBossSnake->isPartOfSnake(x + dx, y + dy))
                    { flag = true; break; }
            }
        }
        this->mFood.push_back(Food(SnakeBody(x, y), 2));
    }
}

bool mysteriousSwitchX = false;
bool mysteriousSwitchZ = false;
void Game::controlSnake() const
{
    int key;
    key = getch();
    switch(key)
    {
        case 'W':
        case 'w':
        case KEY_UP:
        {
			this->mPtrSnake->changeDirection(Direction::Up);
            break;
        }
        case 'S':
        case 's':
        case KEY_DOWN:
        {
			this->mPtrSnake->changeDirection(Direction::Down);
            break;
        }
        case 'A':
        case 'a':
        case KEY_LEFT:
        {
			this->mPtrSnake->changeDirection(Direction::Left);
            break;
        }
        case 'D':
        case 'd':
        case KEY_RIGHT:
        {
			this->mPtrSnake->changeDirection(Direction::Right);
            break;
        }
        case 'x':
        {
            mysteriousSwitchX = true;
            break;
        }
        case 'z':
        {
            mysteriousSwitchZ ^= 1;
            break;
        }
        default:
        {
            break;
        }
    }
}

void EnemySnake::initializeSnake()
{
    int centerX, centerY;
    bool flag = true;
    while (flag) {
        centerX = 1 + rand() % (this->mGameBoardWidth - 2),
        centerY = 1 + rand() % (this->mGameBoardHeight - 2);
        if (this->thisgame->mPtrSnake->isPartOfSnake(centerX, centerY)) continue;
        for (auto &s : this->thisgame->mPtrEnemySnake)
            if (s && s->isPartOfSnake(centerX, centerY)) continue;
        flag = false;
        for (Food i : this->mFood)
            if (i.getPos() == SnakeBody(centerX, centerY))
                { flag = true; break; }
    }
    
    this->mSnake.push_back(SnakeBody(centerX, centerY));
    this->mDirection = Direction(rand() % 4);
}
/*
All game process logic is here.
*/
void Game::adjustDifficulty()
{
    if (this->mPoints >= 2) this->mDifficulty = 1;
    if (this->mPoints >= 5) this->mDifficulty = 2;
    if (this->mPoints >= 10) this->mDifficulty = 3;
    if (this->mPoints >= 15) this->mDifficulty = 4;
    if (this->mPoints >= 23) this->mDifficulty = 5;
    if (this->mPoints >= 36) this->mDifficulty = 6;
    this->mDelay = this->mBaseDelay;
    for (int i = 0; i < this->mNumEnemySnake; i++)
        if (this->mDifficulty >= 1 + i && this->mDifficulty < 6 && !this->mPtrEnemySnake[i]) {
            this->mPtrEnemySnake[i].reset(new EnemySnake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength, this));
            this->mPtrEnemySnake[i]->senseFood(this->mFood);
        }
    if (this->mDifficulty == 6 && this->animationTick == -1) {
        for (auto &i : this->mPtrEnemySnake)
            i.reset(nullptr);
        this->mFood.clear();
        this->allSnakeSenseFood();
        this->animationTick = 0;
    }
    if (this->mDifficulty == 6 && this->animationTick == 50) {
        this->mBossSnake.reset(new BossSnake(this->mGameBoardWidth, this->mGameBoardHeight, this));
    }
    if (this->mDifficulty == 6 && this->animationTick == 100) {
        this->createRandomFood();
        this->allSnakeSenseFood();
    }
}

bool Snake::checkCollision()
{
    if (this->hitWall()) return true;
    if (this->hitSnake(this)) return true;
    for (auto &s : this->thisgame->mPtrEnemySnake)
        if (s && this->hitSnake(s.get()))
            return true;
    if (this->hitSnake(this->thisgame->mPtrSnake.get()))
        return true;
    if (this->thisgame->mBossSnake) {
        if (this->hitSnake(this->thisgame->mBossSnake.get()))
            return true;
        BossSnake *p = dynamic_cast<BossSnake*>(this->thisgame->mBossSnake.get());
        for (auto &s : p->mBullet)
            if (this->hitSnake(s))
                return true;
    }
    return false;
}

bool BulletSnake::checkCollision()
{
    Snake *p = this->thisgame->mPtrSnake.get();
    int sX = int(this->posX),
        sY = int(this->posY);
    int tX = int(this->nextX());
    bool hitBody = false;
    bool hitHead = false;
    for (int x = sX; x >= tX; x--) {
        if (p->isPartOfSnake(x, sY))
            hitBody = true;
        if (p->mSnake[0] == SnakeBody(x, sY))
            hitHead = true;
    }
    return hitBody && !hitHead;
}

void Game::allSnakeSenseFood() {
    this->mPtrSnake->senseFood(this->mFood);
    for (auto &s : this->mPtrEnemySnake) if (s)
        s->senseFood(this->mFood);
}
/*
pair<bool, int>
- bool : eat success
- int : food type
*/
std::pair<bool, int> Game::eatFood(const std::unique_ptr<Snake> &snake) {
    for (auto i = this->mFood.begin(); i != this->mFood.end(); i++)
        if (touchFoodSingle(snake.get(), *i)) {
            int res = i->getFoodType();
            this->mFood.erase(i);
            return std::make_pair(true, res);
        }
    return std::make_pair(false, 0);
}

BossSnake* Game::getBoss() {
    return dynamic_cast<BossSnake*>(this->mBossSnake.get());
}

void Game::runGame()
{
    bool moveSuccess;
    int key;
    while (true)
    {
        /* TODO 
         * this is the main control loop of the game.
         * it keeps running a while loop, and does the following things:
         * 	1. process your keyboard input
         * 	2. clear the window
         * 	3. move the current snake forward
         * 	4. check if the snake has eaten the food after movement
         * 	5. check if the snake dies after the movement
         * 	6. make corresponding steps for the ``if conditions'' in 3 and 4.
         *   7. render the position of the food and snake in the new frame of window. 
         *   8. update other game states and refresh the window
         */
        this->controlSnake();
        this->createGameBoard();

        // Snake Move
        bool collision = this->mPtrSnake->checkCollision();
        if (collision)
            break; // die
        auto eatRes = this->eatFood(this->mPtrSnake);
        bool newFood = eatRes.first && eatRes.second != 0;
        if (!mysteriousSwitchZ) // backdoor
            this->mPtrSnake->moveForward(!(eatRes.first || mysteriousSwitchX));
        if (eatRes.first) {
            this->mPoints += 1;
        }
        if (newFood) {
            this->createRandomFood();
            this->allSnakeSenseFood();
        }
        if (eatRes.first && eatRes.second == 2) {
            BossSnake *p = this->getBoss();
            p->setHealth(p->getHealth() - 10);
            this->renderInformationBoard_warning();
        }
        if (mysteriousSwitchX == true) { // backdoor
            this->mPoints += 1;
            mysteriousSwitchX = false;
        }

        // Enemy Snake Control
        for (auto &s : this->mPtrEnemySnake) if (s)
            s->EnemySnakeAI();
        // Enemy Snake Move
        for (auto &s : this->mPtrEnemySnake) if (s) {
            bool enemycollision = s->checkCollision();
            if (enemycollision) {
                for (auto i : s->mSnake)
                    if (rand() % 5 != 0)
                        this->mFood.push_back(Food(i, 0));
                this->allSnakeSenseFood();
                s.reset(nullptr);
                continue;
            }
            auto enemyeatRes = this->eatFood(s);
            bool enemynewFood = enemyeatRes.first && (enemyeatRes.second != 0);
            s->moveForward(!(enemyeatRes.first || s->mSnake.size() < s->mInitialSnakeLength));
            if (enemynewFood) {
                this->createRandomFood();
                this->allSnakeSenseFood();
            }
        }

        // Boss Onstage
        if (this->animationTick > 50 && this->animationTick <= 100 && this->animationTick % 3 == 0) {
            this->getBoss()->onstageAnimation();
        }
        // Boss Logic
        if (this->mBossSnake && this->animationTick > 100) {
            BossSnake *p = this->getBoss();
            if (animationTick % 4 == 0)
                p->summonBullet();
            p->allBulletForward();
        }

		this->renderSnake(this->mPtrSnake, SNAKE_COLOR);
        for (auto &s : this->mPtrEnemySnake)
            this->renderSnake(s, ENEMY_SNAKE_COLOR);
        this->renderSnake(this->mBossSnake, DEFAULT_COLOR);
        if (this->mBossSnake) {
            this->renderBulletSnake(this->getBoss(), DEFAULT_COLOR);
        }
        this->renderFood();

        box(this->mWindows[1], 0, 0);
        this->renderGameBoard();

        this->adjustDifficulty();
        this->renderPoints();
        this->renderDifficulty();

        if (this->animationTick >= 0)
            this->renderInformationBoard_warning(),
            this->animationTick++;
        refresh();

		std::this_thread::sleep_for(std::chrono::milliseconds(this->mDelay));
    }
}

void Game::startGame()
{
    refresh();
    bool choice;
    while (true)
    {
        this->readLeaderBoard();
        this->renderBoards();
        this->initializeGame();
        this->runGame();
        this->updateLeaderBoard();
        this->writeLeaderBoard();
        choice = this->renderRestartMenu();
        if (choice == false)
        {
            break;
        }
    }
}