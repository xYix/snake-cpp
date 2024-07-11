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
};

class BossSnake : public Snake {
    friend class Game;
    friend class Snake;
    friend class BulletSnake;
public:
    BossSnake(int gameBoardWidth, int gameBoardHeight, Game* game);
    int getLength() const;
    void onstageAnimation();
    void summonBullet();
    void allBulletForward();
    void attack();
    int getHealth() const;
    void setHealth(int newh);
    ~BossSnake();
private:
    std::vector<BulletSnake*> mBullet;
    const int mHalfWidth = 3;
    const int mInitialBulletLength = 8;
    const int mMaxHealth = 100;
    int mHealth;
};