#include <cmath>
#include "boss.h"

BulletSnake::BulletSnake(BossSnake *master):
    Snake(master->mGameBoardWidth, master->mGameBoardHeight, 0, master->thisgame), 
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
    Snake(master->mGameBoardWidth, master->mGameBoardHeight, 0, master->thisgame), 
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
    return (1.0 - (T * T) / 2500.0) * (this->mGameBoardWidth + this->mLength);
}
void BulletSnake::moveForward() {
    this->posX = BulletSnake::nextX();
    this->innerClock++;
}
void BulletSnake::materialization() {
    int trueX = int(this->posX),
        trueY = int(this->posY);
    
    this->mSnake.clear();
    for (int i = 0; i < this->mLength; i++) {
        if (trueX + i >= 1 && trueX + i < this->mGameBoardWidth - 1 && 
            trueY >= 1 && trueY < this->mGameBoardHeight - 1)
                this->mSnake.push_back(SnakeBody(trueX + i, trueY));
    }
}

SnakeBody BossSnake::getHead() const {
    return this->mSnake[this->mSnake.size() - this->mHalfWidth - 1];
}
SniperSnake::SniperSnake(BossSnake* master, SnakeBody target, double angle) :
    Snake(master->mGameBoardWidth, master->mGameBoardHeight, 0, master->thisgame), 
    mMaster(master),
    innerClock(0)
{
    this->mSnake.clear();
    SnakeBody source = master->getHead();
    SnakeBody delta(target.getX() - source.getX(),
                    target.getY() - source.getY());
    double dX = delta.getX() / sqrt(delta.getX() * delta.getX() + delta.getY() * delta.getY());
    double dY = delta.getY() / sqrt(delta.getX() * delta.getX() + delta.getY() * delta.getY());
    angle = angle / 180 * 3.1415926;
    this->dirX = dX * cos(angle) - dY * sin(angle);
    this->dirY = dX * sin(angle) + dY * cos(angle);
    this->posX = source.getX() - this->dirX * this->mVelocity;
    this->posY = source.getY() - this->dirX * this->mVelocity;
}
void SniperSnake::moveForward() {
    this->posX += this->dirX * this->mVelocity;
    this->posY += this->dirY * this->mVelocity;
    this->innerClock++;
}
void SniperSnake::materialization() {
    this->mSnake.clear();
    double curX = this->posX;
    double curY = this->posY;
    for (int i = 0; i < this->mLength; i++) {
        int tX = curX,
            tY = curY;
        if (tX >= 1 && tX < this->mGameBoardWidth - 1 && 
            tY >= 1 && tY < this->mGameBoardHeight - 1)
                this->mSnake.push_back(SnakeBody(tX, tY));
        curX += this->dirX;
        curY += this->dirY;
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
    this->mSniper.clear();
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

void BossSnake::summonSniper(SnakeBody target, double angle) {
    this->mSniper.push_back(new SniperSnake(this, target, angle));
}
void BossSnake::allSniperForward() {
    std::vector<SniperSnake*>::iterator i = this->mSniper.begin();
    while (i != this->mSniper.end()) {
        if ((*i)->innerClock >= 50) {
            delete *i;
            i = this->mSniper.erase(i);
        }
        else
            i++;
    }
    for (auto &i : this->mSniper) {
        i->moveForward();
        i->materialization();
    }
}

void BossSnake::clearBullet()
{
    for (auto &i : this->mBullet)
        delete i;
    this->mBullet.clear();
    for (auto &i : this->mSniper)
        delete i;
    this->mSniper.clear();
}
BossSnake::~BossSnake()
{
    this->clearBullet();
}
int BossSnake::getHealth() const {
    return this->mHealth;
}
void BossSnake::setHealth(int newh) {
    this->mHealth = newh;
}