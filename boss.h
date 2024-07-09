#include <vector>
#include "snake.h"

class Game;

class BossSnake : public Snake
{
    friend class Game;
public:
    BossSnake(int gameBoardWidth, int gameBoardHeight, Game* game);
    int getLength() const;
    void moveForward();
private:
};