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
    mHealth(this->mMaxHealth)
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

void BossSnake::summonBullet() {
    this->mBullet.push_back(new BulletSnake(this));
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

BossSnake::~BossSnake()
{
    for (auto &i : this->mBullet)
        delete i;
    this->mBullet.clear();
}