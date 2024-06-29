#include <string>
#include <iostream>
#include <cmath> 

// For terminal delay
#include <chrono>
#include <thread>

#include <fstream>
#include <algorithm> 

#include "game.h"

#define COLOR_36C48E 8

#define FOOD_COLOR 1
#define SNAKE_COLOR 2
#define DEFAULT_COLOR 3
#define ENEMY_SNAKE_COLOR 4

Game::Game()
{
    // set random seed
    std::srand(std::time(nullptr));
    // Separate the screen to three windows
    this->mWindows.resize(3);
    initscr();
    if (has_colors() == false)
    {
        printf("No color support!\n");
        exit(0);
    }
    start_color();
    init_color(COLOR_36C48E, 211, 640, 555);
    init_pair(FOOD_COLOR, COLOR_RED, COLOR_BLACK);
    init_pair(SNAKE_COLOR, COLOR_36C48E, COLOR_BLACK);
    init_pair(DEFAULT_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(ENEMY_SNAKE_COLOR, COLOR_BLUE, COLOR_BLACK);
    // If there wasn't any key pressed don't wait for keypress
    nodelay(stdscr, true);
    // Turn on keypad control
    keypad(stdscr, true);
    // No echo for the key pressed
    noecho();
    // No cursor show
    curs_set(0);
    // Get screen and board parameters
    getmaxyx(stdscr, this->mScreenHeight, this->mScreenWidth);
    this->mGameBoardWidth = this->mScreenWidth - this->mInstructionWidth;
    this->mGameBoardHeight = this->mScreenHeight - this->mInformationHeight;

    this->createInformationBoard();
    this->createGameBoard();
    this->createInstructionBoard();

    // Initialize the leader board to be all zeros
    this->mLeaderBoard.assign(this->mNumLeaders, 0);
}

Game::~Game()
{
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        delwin(this->mWindows[i]);
    }
    endwin();
}

void Game::createInformationBoard()
{
    int startY = 0;
    int startX = 0;
    this->mWindows[0] = newwin(this->mInformationHeight, this->mScreenWidth, startY, startX);
}

void Game::renderInformationBoard() const
{
    mvwprintw(this->mWindows[0], 1, 1, "Welcome to The Snake Game!");
    mvwprintw(this->mWindows[0], 2, 1, "This is a mock version.");
    mvwprintw(this->mWindows[0], 3, 1, "Please fill in the blanks to make it work properly!!");
    mvwprintw(this->mWindows[0], 4, 1, "Implemented using C++ and libncurses library.");
    wrefresh(this->mWindows[0]);
}

void Game::createGameBoard()
{
    int startY = this->mInformationHeight;
    int startX = 0;
    this->mWindows[1] = newwin(this->mScreenHeight - this->mInformationHeight, this->mScreenWidth - this->mInstructionWidth, startY, startX);
}

void Game::renderGameBoard() const
{
    wrefresh(this->mWindows[1]);
}

void Game::createInstructionBoard()
{
    int startY = this->mInformationHeight;
    int startX = this->mScreenWidth - this->mInstructionWidth;
    this->mWindows[2] = newwin(this->mScreenHeight - this->mInformationHeight, this->mInstructionWidth, startY, startX);
}

void Game::renderInstructionBoard() const
{
    mvwprintw(this->mWindows[2], 1, 1, "Manual");

    mvwprintw(this->mWindows[2], 3, 1, "Up: W");
    mvwprintw(this->mWindows[2], 4, 1, "Down: S");
    mvwprintw(this->mWindows[2], 5, 1, "Left: A");
    mvwprintw(this->mWindows[2], 6, 1, "Right: D");

    mvwprintw(this->mWindows[2], 8, 1, "Difficulty");
    mvwprintw(this->mWindows[2], 11, 1, "Points");

    wrefresh(this->mWindows[2]);
}


void Game::renderLeaderBoard() const
{
    // If there is not too much space, skip rendering the leader board 
    if (this->mScreenHeight - this->mInformationHeight - 14 - 2 < 3 * 2)
    {
        return;
    }
    mvwprintw(this->mWindows[2], 14, 1, "Leader Board");
    std::string pointString;
    std::string rank;
    for (int i = 0; i < std::min(this->mNumLeaders, this->mScreenHeight - this->mInformationHeight - 14 - 2); i ++)
    {
        pointString = std::to_string(this->mLeaderBoard[i]);
        rank = "#" + std::to_string(i + 1) + ":";
        mvwprintw(this->mWindows[2], 14 + (i + 1), 1, rank.c_str());
        mvwprintw(this->mWindows[2], 14 + (i + 1), 5, pointString.c_str());
    }
    wrefresh(this->mWindows[2]);
}

bool Game::renderRestartMenu() const
{
    WINDOW * menu;
    int width = this->mGameBoardWidth * 0.5;
    int height = this->mGameBoardHeight * 0.5;
    int startX = this->mGameBoardWidth * 0.25;
    int startY = this->mGameBoardHeight * 0.25 + this->mInformationHeight;

    menu = newwin(height, width, startY, startX);
    box(menu, 0, 0);
    std::vector<std::string> menuItems = {"Restart", "Quit"};

    int index = 0;
    int offset = 4;
    mvwprintw(menu, 1, 1, "Your Final Score:");
    std::string pointString = std::to_string(this->mPoints);
    mvwprintw(menu, 2, 1, pointString.c_str());
    wattron(menu, A_STANDOUT);
    mvwprintw(menu, 0 + offset, 1, menuItems[0].c_str());
    wattroff(menu, A_STANDOUT);
    mvwprintw(menu, 1 + offset, 1, menuItems[1].c_str());

    wrefresh(menu);

    int key;
    while (true)
    {
        key = getch();
        switch(key)
        {
            case 'W':
            case 'w':
            case KEY_UP:
            {
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index --;
                index = (index < 0) ? menuItems.size() - 1 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
            case 'S':
            case 's':
            case KEY_DOWN:
            {
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index ++;
                index = (index > menuItems.size() - 1) ? 0 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
        }
        wrefresh(menu);
        if (key == ' ' || key == 10)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    delwin(menu);

    if (index == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
    
}

void Game::renderPoints() const
{
    std::string pointString = std::to_string(this->mPoints);
    mvwprintw(this->mWindows[2], 12, 1, pointString.c_str());
    wrefresh(this->mWindows[2]);
}

void Game::renderDifficulty() const
{
    std::string difficultyString = std::to_string(this->mDifficulty);
    mvwprintw(this->mWindows[2], 9, 1, difficultyString.c_str());
    wrefresh(this->mWindows[2]);
}

void Game::initializeGame()
{
    // allocate memory for a new snake
	this->mPtrSnake.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength, this));
    if (this->mPtrEnemySnake) {
        this->mPtrEnemySnake.reset(nullptr);
    }

    /* 
     * initialize the game points as zero
     * create a food at randome place
     * make the snake aware of the food
     * other initializations
     */
    this->mPoints = 0;
    this->mFood.clear();
    for (int i = 0; i < this->mFoodCount; i++)
        this->createRandomFood();
    this->mPtrSnake->senseFood(this->mFood);
    this->adjustDifficulty();
    // ...
}

void Game::createRandomFood()
{
    /* 
     * create a food at random places
     * make sure that the food doesn't overlap with the snake.
     */
    int x, y;
    while (true) {
        x = 1 + rand() % (this->mGameBoardWidth - 2),
        y = 1 + rand() % (this->mGameBoardHeight - 2);
        if (this->mPtrSnake->isPartOfSnake(x, y)) continue;
        for (const Food &i : this->mFood)
            if (i.getPos() == SnakeBody(x, y))
                continue;
        break;
    }
    /*
    ? When whole screen is fully occupied by snake
    */
    this->mFood.push_back(SnakeBody(x, y));
}

void Game::renderFood() const
{   
    wattrset(this->mWindows[1], COLOR_PAIR(FOOD_COLOR));
    for (const Food &i : this->mFood)
        mvwaddch(this->mWindows[1], i.getPos().getY(), i.getPos().getX(), i.isRealFood() ? this->mFoodSymbol : this->mFakeFoodSymbol);
    wrefresh(this->mWindows[1]);
    wattrset(this->mWindows[1], COLOR_PAIR(DEFAULT_COLOR));
}

void Game::renderSnake(const std::unique_ptr<Snake> &snake, int clr) const
{
    wattrset(this->mWindows[1], COLOR_PAIR(clr));
    int snakeLength = snake->getLength();
    std::vector<SnakeBody>& snakebody = snake->getSnake();
    for (int i = 0; i < snakeLength; i ++)
    {
        mvwaddch(this->mWindows[1], snakebody[i].getY(), snakebody[i].getX(), this->mSnakeSymbol);
    }
    wrefresh(this->mWindows[1]);
    wattrset(this->mWindows[1], COLOR_PAIR(DEFAULT_COLOR));
}

bool mysteriousSwitch = false;
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
            mysteriousSwitch = true;
            break;
        }
        default:
        {
            break;
        }
    }
}

void Game::renderBoards() const
{
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        werase(this->mWindows[i]);
    }
    this->renderInformationBoard();
    this->renderGameBoard();
    this->renderInstructionBoard();
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        box(this->mWindows[i], 0, 0);
        wrefresh(this->mWindows[i]);
    }
    this->renderLeaderBoard();
}

void EnemySnake::initializeSnake()
{
    int centerX = 1 + rand() % (this->mGameBoardWidth - 2),
        centerY = 1 + rand() % (this->mGameBoardHeight - 2);
    
    this->mSnake.push_back(SnakeBody(centerX, centerY));
    this->mDirection = Direction(rand() % 4);
}
void Game::adjustDifficulty()
{
    this->mDifficulty = this->mPoints / 5;
    this->mDelay = this->mBaseDelay * pow(0.75, this->mDifficulty);
    if (this->mDifficulty >= 2 && !this->mPtrEnemySnake) {
	    this->mPtrEnemySnake.reset(new EnemySnake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength, this));
        this->mPtrEnemySnake->senseFood(this->mFood);
    }
}

bool Snake::checkCollision()
{
    if (this->hitWall()) return true;
    if (this->hitSnake(this)) return true;
    if (this->thisgame->mPtrEnemySnake && this->hitSnake(this->thisgame->mPtrEnemySnake.get()))
        return true;
    return false;
}

bool EnemySnake::checkCollision()
{
    if (this->hitWall()) return true;
    if (this->hitSnake(this)) return true;
    if (this->hitSnake(this->thisgame->mPtrSnake.get()))
        return true;
    return false;

}

bool Game::eatFood(const std::unique_ptr<Snake> &snake) {
    // true if new food need to be spawned
    for (auto i = this->mFood.begin(); i != this->mFood.end(); i++)
        if (i->getPos() == snake->createNewHead()) {
            bool res = i->isRealFood();
            this->mFood.erase(i);
            return res;
        }
    return false;
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
        bool touchFood = this->mPtrSnake->touchFood();
        bool eatFood = touchFood && this->eatFood(this->mPtrSnake);
        this->mPtrSnake->moveFoward(!(touchFood || mysteriousSwitch));
        if (touchFood) {
            this->mPoints += 1;
        }
        if (eatFood) {
            this->createRandomFood();
            this->mPtrSnake->senseFood(this->mFood);
            if (this->mPtrEnemySnake)
                this->mPtrEnemySnake->senseFood(this->mFood);
        }
        if (mysteriousSwitch == true) {// backdoor
            this->mPoints += 1;
            mysteriousSwitch = false;
        }

        // Enemy Snake Control
        if (this->mPtrEnemySnake) {
            this->mPtrEnemySnake->EnemySnakeAI();
        }
        // Enemy Snake Move
        if (this->mPtrEnemySnake) {
            bool enemycollision = this->mPtrEnemySnake->checkCollision();
            if (enemycollision) {
                for (auto i : this->mPtrEnemySnake->mSnake)
                    if (rand() % 5 != 0)
                        this->mFood.push_back(Food(i, false));
                this->mPtrSnake->senseFood(this->mFood);
                this->mPtrEnemySnake.reset(nullptr);
            }
            else {
                bool enemytouchFood = this->mPtrEnemySnake->touchFood();
                bool enemyeatFood = enemytouchFood && this->eatFood(this->mPtrEnemySnake);
                this->mPtrEnemySnake->moveFoward(!(enemytouchFood || this->mPtrEnemySnake->mSnake.size() < this->mPtrEnemySnake->mInitialSnakeLength));
                if (enemyeatFood) {
                    this->createRandomFood();
                    this->mPtrSnake->senseFood(this->mFood);
                    this->mPtrEnemySnake->senseFood(this->mFood);
                }
            }
        }

		this->renderSnake(this->mPtrSnake, SNAKE_COLOR);
        if (this->mPtrEnemySnake)
            this->renderSnake(this->mPtrEnemySnake, ENEMY_SNAKE_COLOR);
        this->renderFood();

        box(this->mWindows[1], 0, 0);
        this->renderGameBoard();

        this->adjustDifficulty();
        this->renderPoints();
        this->renderDifficulty();
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

// https://en.cppreference.com/w/cpp/io/basic_fstream
bool Game::readLeaderBoard()
{
    std::fstream fhand(this->mRecordBoardFilePath, fhand.binary | fhand.in);
    if (!fhand.is_open())
    {
        return false;
    }
    int temp;
    int i = 0;
    while ((!fhand.eof()) && (i < mNumLeaders))
    {
        fhand.read(reinterpret_cast<char*>(&temp), sizeof(temp));
        this->mLeaderBoard[i] = temp;
        i ++;
    }
    fhand.close();
    return true;
}

bool Game::updateLeaderBoard()
{
    bool updated = false;
    int newScore = this->mPoints;
    for (int i = 0; i < this->mNumLeaders; i ++)
    {
        if (this->mLeaderBoard[i] >= this->mPoints)
        {
            continue;
        }
        int oldScore = this->mLeaderBoard[i];
        this->mLeaderBoard[i] = newScore;
        newScore = oldScore;
        updated = true;
    }
    return updated;
}

bool Game::writeLeaderBoard()
{
    // trunc: clear the data file
    std::fstream fhand(this->mRecordBoardFilePath, fhand.binary | fhand.trunc | fhand.out);
    if (!fhand.is_open())
    {
        return false;
    }
    for (int i = 0; i < this->mNumLeaders; i ++)
    {
        fhand.write(reinterpret_cast<char*>(&this->mLeaderBoard[i]), sizeof(this->mLeaderBoard[i]));;
    }
    fhand.close();
    return true;
}








