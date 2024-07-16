#include <cmath>
#include "boss.h"

BulletSnake::BulletSnake(BossSnake *master):
    Snake(master->mGameBoardWidth, master->mGameBoardHeight, master->mInitialBulletLength, master->thisgame), 
    mMaster(master),
    innerClock(0),
    posX(0),
    posY(0)
{
    this->mSnake.clear();
    while (true) {
        this->posY = 1 + rand() % (this->mGameBoardHeight - 2);
        if (this->posY >= this->mGameBoardHeight / 2 - this->mMaster->mHalfWidth &&
            this->posY <= this->mGameBoardHeight / 2 + this->mMaster->mHalfWidth )
            continue;
        break;
    }
    this->posY += 0.1; // avoid float error
}

BulletSnake::BulletSnake(BossSnake *master, int y):
    Snake(master->mGameBoardWidth, master->mGameBoardHeight, master->mInitialBulletLength, master->thisgame), 
    mMaster(master),
    innerClock(0),
    posX(0),
    posY(0)
{
    this->mSnake.clear();
    this->posY = y;
    this->posY += 0.1;
}

double BulletSnake::nextX() {
    double T = this->innerClock + 1;
    return (1.0 - (T * T) / 2500.0) * (this->mGameBoardWidth + this->mInitialSnakeLength);
}
void BulletSnake::moveForward() {
    this->posX = BulletSnake::nextX();
    this->innerClock++;
}
void BulletSnake::materialization() {
    int trueX = int(this->posX),
        trueY = int(this->posY);
    
    this->mSnake.clear();
    for (int i = 0; i < this->mInitialSnakeLength; i++) {
        if (trueX + i >= 1 && trueX + i < this->mGameBoardWidth - 1 && 
            trueY >= 1 && trueY < this->mGameBoardHeight - 1)
                this->mSnake.push_back(SnakeBody(trueX + i, trueY));
    }
}

BossSnake::BossSnake(int gameBoardWidth, int gameBoardHeight, Game* game):
    Snake(gameBoardWidth, gameBoardHeight, 0, game),
    mHealth(this->mMaxHealth),
    mAttackClock(0)
{
    this->mSnake.clear();
    for (int i = -this->mHalfWidth; i <= this->mHalfWidth; i++)
        this->mSnake.push_back(SnakeBody(this->mGameBoardWidth - 2, this->mGameBoardHeight / 2 + i));
    this->mBullet.clear();
    // this->mHealth = this->mMaxHealth;
}

int BossSnake::getLength() const {
    return this->mSnake.size() / 7;
}
void BossSnake::onstageAnimation() {
    int beforeLength = this->getLength();
    for (int i = -this->mHalfWidth; i <= this->mHalfWidth; i++)
        this->mSnake.push_back(SnakeBody(this->mGameBoardWidth - beforeLength - 2, this->mGameBoardHeight / 2 + i));
}
void BossSnake::backstageAnimation() {
    for (int i = -this->mHalfWidth; i <= this->mHalfWidth; i++)
        this->mSnake.pop_back();
}

void BossSnake::summonBullet() {
    int j = 1 + rand() % 6;
    if (j <= 3 || this->mHealth > this->mMaxHealth / 2)
    {
        this->mBullet.push_back(new BulletSnake(this));
    }
    else if (j <= 5)
    {
        int y;
        while (true) {
            y = 2 + rand() % (this->mGameBoardHeight - 3);
            if (y >= this->mGameBoardHeight / 2 - this->mHalfWidth - 1 &&
                y <= this->mGameBoardHeight / 2 + this->mHalfWidth + 1)
                continue;
            break;
        }
        this->mBullet.push_back(new BulletSnake(this, y));
        this->mBullet.push_back(new BulletSnake(this, y - 1));
    }
    else
    {
        int y;
        while (true){
            y = 3 + rand() % (this->mGameBoardHeight - 4);
            if (y >= this->mGameBoardHeight / 2 - this->mHalfWidth - 2 &&
                y <= this->mGameBoardHeight / 2 + this->mHalfWidth + 2)
                continue;
            break;
        }
        this->mBullet.push_back(new BulletSnake(this, y));
        this->mBullet.push_back(new BulletSnake(this, y - 1));
        this->mBullet.push_back(new BulletSnake(this, y - 2));
    }
}
void BossSnake::allBulletForward() {
    std::vector<BulletSnake*>::iterator i = this->mBullet.begin();
    while (i != this->mBullet.end()) {
        if ((*i)->innerClock >= 50 || (*i)->checkCollision()) {
            delete *i;
            i = this->mBullet.erase(i);
        }
        else
            i++;
    }
    for (auto &i : this->mBullet) {
        i->moveForward();
        i->materialization();
    }
}

void BossSnake::clearBullet()
{
    for (auto &i : this->mBullet)
        delete i;
    this->mBullet.clear();
}
BossSnake::~BossSnake()
{
    for (auto &i : this->mBullet)
        delete i;
    this->mBullet.clear();
}
int BossSnake::getHealth() const {
    return this->mHealth;
}
void BossSnake::setHealth(int newh) {
    this->mHealth = newh;
}