// For terminal delay
#include <chrono>
#include <thread>

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

    // Initialize Enemy Snake
    this->mPtrEnemySnake.resize(mNumEnemySnake);
    for (auto &s : this->mPtrEnemySnake)
        s.reset(nullptr);
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
    mvwprintw(this->mWindows[0], 2, 1, "");
    mvwprintw(this->mWindows[0], 3, 1, "");
    mvwprintw(this->mWindows[0], 4, 1, "ver 0.0.0");
    wrefresh(this->mWindows[0]);
}
void Game::renderInformationBoard_warning()
{
    // if (this->animationTick == 0 || this->animationTick == 50) {
    this->mWindows[0] = newwin(this->mInformationHeight, this->mScreenWidth, 0, 0);
    box(this->mWindows[0], 0, 0);
    // }
    if (this->animationTick >= 0 && this->animationTick < 50) {
        if (this->animationTick % 10 < 5)
            wattrset(this->mWindows[0], COLOR_PAIR(FOOD_COLOR));
        else 
            wattrset(this->mWindows[0], COLOR_PAIR(DEFAULT_COLOR));
        mvwprintw(this->mWindows[0], 1, this->mScreenWidth / 2 - 25, "---------------------------------------------------");
        mvwprintw(this->mWindows[0], 2, this->mScreenWidth / 2 - 25, "---------- WARNING -- WARNING -- WARNING ----------");
        mvwprintw(this->mWindows[0], 3, this->mScreenWidth / 2 - 25, "---------------------------------------------------");
        wrefresh(this->mWindows[0]);
        wattrset(this->mWindows[0], COLOR_PAIR(DEFAULT_COLOR));
    }
    if (this->animationTick >= 50) {
        wattrset(this->mWindows[0], COLOR_PAIR(FOOD_COLOR));
        mvwprintw(this->mWindows[0], 1, this->mScreenWidth / 2 - 25, "----------------------- BOSS ----------------------");
        mvwprintw(this->mWindows[0], 2, this->mScreenWidth / 2 - 25, "------------- Longrraz, Snake of Chaos ------------");
        mvwprintw(this->mWindows[0], 3, this->mScreenWidth / 2 - 25, "---------------------------------------------------");
        BossSnake *p = this->getBoss();
        for (int i = 1; i <= (this->mScreenWidth - 10) * p->getHealth() / p->mMaxHealth; i++)
            mvwprintw(this->mWindows[0], 4, i, "*");
        wrefresh(this->mWindows[0]);
        wattrset(this->mWindows[0], COLOR_PAIR(DEFAULT_COLOR));
    }
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

void Game::renderFood() const
{   
    wattrset(this->mWindows[1], COLOR_PAIR(FOOD_COLOR));
    for (const Food &i : this->mFood) {
        switch (i.getFoodType()) {
            case 0:
                mvwaddch(this->mWindows[1], i.getPos().getY(), i.getPos().getX(), this->mFakeFoodSymbol);
                break;
            case 1:
                mvwaddch(this->mWindows[1], i.getPos().getY(), i.getPos().getX(), this->mFoodSymbol);
                break;
            case 2:
                for (int dx = -1; dx <= 1; dx++)
                for (int dy = -1; dy <= 1; dy++)
                    mvwaddch(this->mWindows[1], i.getPos().getY() + dy, i.getPos().getX() + dx, this->mFoodSymbol);
                break;
        }
    }
    wrefresh(this->mWindows[1]);
    wattrset(this->mWindows[1], COLOR_PAIR(DEFAULT_COLOR));
}

void Game::renderSnake(const std::unique_ptr<Snake> &snake, int clr) const
{
    if (!snake) return;
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
void Game::renderBulletSnake(const BossSnake* master, int clr) const {
    
    wattrset(this->mWindows[1], COLOR_PAIR(clr));
    for (auto &s : master->mBullet) {
        int snakeLength = s->getLength();
        std::vector<SnakeBody>& snakebody = s->getSnake();
        for (int i = 0; i < snakeLength; i ++)
        {
            mvwaddch(this->mWindows[1], snakebody[i].getY(), snakebody[i].getX(), this->mSnakeSymbol);
        }
    }
    wrefresh(this->mWindows[1]);
    wattrset(this->mWindows[1], COLOR_PAIR(DEFAULT_COLOR));
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