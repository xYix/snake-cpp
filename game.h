#ifndef GAME_H
#define GAME_H

#include <ncurses.h>
#include <string>
#include <vector>
#include <memory>

#include "snake.h"
#include "boss.h"

class Game
{
    friend class Snake;
    friend class EnemySnake;
    friend class BulletSnake;
    friend class BossSnake;
public:
    Game();
    ~Game();
    
	void createInformationBoard();
    void renderInformationBoard() const;
    void renderInformationBoard_warning();
    void renderInformationBoard_win() const;

    void createGameBoard();
    void renderGameBoard() const;
    
	void createInstructionBoard();
    void renderInstructionBoard() const;
		
	void loadLeadBoard();
    void updateLeadBoard();
    bool readLeaderBoard();
    bool updateLeaderBoard();
    bool writeLeaderBoard();
    void renderLeaderBoard() const;
    bool renderRestartMenu() const;

	void renderBoards() const;

    void renderFood() const;
    void renderSnake(const std::unique_ptr<Snake> &snake, int clr) const;
    void renderBulletSnake(const BossSnake* master, int clr) const;
    void renderSniperSnake(const BossSnake* master, int clr) const;

    void renderPoints() const;
    void renderDifficulty() const;
    
	void initializeGame();
    void runGame();
    
	void createRandomFood();
    void controlSnake();
    
	void startGame();
    void adjustDifficulty();

    void allSnakeSenseFood();
    std::pair<bool, int> eatFood(const std::unique_ptr<Snake> &snake);
    BossSnake* getBoss();
private:
    std::unique_ptr<Snake> mPtrSnake;
    std::vector<std::unique_ptr<Snake> > mPtrEnemySnake;
    std::unique_ptr<Snake> mBossSnake;
    // We need to have two windows
    // One is for game introduction
    // One is for game mWindows
    int mScreenWidth;
    int mScreenHeight;
    int mGameBoardWidth;
    int mGameBoardHeight;
    const int mInformationHeight = 6;
    const int mInstructionWidth = 18;
    std::vector<WINDOW *> mWindows;
    // Snake information
    const int mInitialSnakeLength = 2;
    const char mSnakeSymbol = '@';
    // Food information
    std::vector<Food> mFood;
    const char mFoodSymbol = '#';
    const char mFakeFoodSymbol = 'a';
    int mPoints = 0;
    int mDifficulty = 0;
    int mBaseDelay = 100;
    int mFoodCount = 5;
    int mDelay;
    int mDelayedLength = 0;
    const std::string mRecordBoardFilePath = "record.dat";
    std::vector<int> mLeaderBoard;
    const int mNumLeaders = 3;
    const int mNumEnemySnake = 5;
    
    // animation effect
    int animationClock = -1;
};

#endif
