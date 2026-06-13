#pragma once
#include "Board.hpp"
#include "AI.hpp"
#include <memory>

class GameApp {
public:
    GameApp();
    void render();

private:
    int boardSize = 3;
    int winLength = 3;
    bool vsAI = true;
    bool aiGoesFirst = false;
    
    bool gameStarted = false;
    Player currentPlayer = Player::X;
    Player winner = Player::EMPTY;
    bool isDraw = false;

    std::unique_ptr<Board> board;
    std::unique_ptr<AI> ai;

    void renderMenu();
    void renderGame();
    void handleComputerMove();
};