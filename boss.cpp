#include "boss.h"

BossSnake::BossSnake(int gameBoardWidth, int gameBoardHeight, Game* game):
    Snake(gameBoardWidth, gameBoardHeight, 0, game)
{
    this->mSnake.clear();
    for (int i = -3; i <= 3; i++)
        this->mSnake.push_back(SnakeBody(this->mGameBoardWidth - 2, this->mGameBoardHeight / 2 + i));
}

int BossSnake::getLength() const {
    return this->mSnake.size() / 7;
}
void BossSnake::moveForward() {
    int beforeLength = this->getLength();
    for (int i = -3; i <= 3; i++)
        this->mSnake.push_back(SnakeBody(this->mGameBoardWidth - beforeLength - 2, this->mGameBoardHeight / 2 + i));
}