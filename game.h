#ifndef GAME_H
#define GAME_H

#include <ncurses.h>
#include <string>
#include <vector>
#include <memory>

#include "snake.h"


class Game
{
public:
    Game();
    ~Game();
    
		void createInformationBoard();
    void renderInformationBoard() const;

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
    
		void renderBoards() const;
    
		void initializeGame();
    void runGame();
    void renderPoints() const;
    void renderDifficulty() const;
    
		void createRandomFood();
    void renderFood() const;
    void renderSnake(const std::unique_ptr<Snake> &snake, int clr) const;
    void controlSnake() const;
    
		void startGame();
    bool renderRestartMenu() const;
    void adjustDifficulty();

    bool eatFood(const std::unique_ptr<Snake> &snake);
    
    std::unique_ptr<Snake> mPtrSnake;
    std::unique_ptr<Snake> mPtrEnemySnake = nullptr;
private:
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
    int mFoodCount = 3;
    int mDelay;
    const std::string mRecordBoardFilePath = "record.dat";
    std::vector<int> mLeaderBoard;
    const int mNumLeaders = 3;
};

#endif
