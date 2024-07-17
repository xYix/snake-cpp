#include <vector>
#include "snake.h"

class Game;

class BossSnake;

class BulletSnake : public Snake {
    friend class BossSnake;
public:
    BulletSnake(BossSnake* master);
    BulletSnake(BossSnake *master, int y);
    bool checkCollision();
    double nextX();
    void moveForward();
    void materialization();
private:
    int innerClock;
    double posX, posY;
    BossSnake *mMaster;
    const int mLength = 8;
};

class SniperSnake : public Snake {
    friend class BossSnake;
public:
    SniperSnake(BossSnake* master, SnakeBody target, double angle = 0.0);
    // bool checkCollision();
    void moveForward();
    void materialization();
private:
    int innerClock;
    double posX, posY;
    double dirX, dirY;
    const double mVelocity = 3.0;
    const int mLength = 5;
    BossSnake *mMaster;
};

class BossSnake : public Snake {
    friend class Game;
    friend class Snake;
    friend class BulletSnake;
    friend class SniperSnake;
public:
    BossSnake(int gameBoardWidth, int gameBoardHeight, Game* game);
    int getLength() const;
    void onstageAnimation();
    void backstageAnimation();
    void summonBullet();
    void allBulletForward();
    void summonSniper(SnakeBody target, double angle = 0.0);
    void allSniperForward();
    void attack();
    int getHealth() const;
    void setHealth(int newh);
    void clearBullet();
    SnakeBody getHead() const;
    ~BossSnake();
private:
    std::vector<BulletSnake*> mBullet;
    std::vector<SniperSnake*> mSniper;
    const int mHalfWidth = 3;
    // const int mInitialBulletLength = 8;
    const int mMaxHealth = 100;
    int mHealth;
    int mAttackClock;
};