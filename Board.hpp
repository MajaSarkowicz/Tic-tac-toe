#pragma once
#include <vector>

enum class Player { EMPTY = 0, X = 1, O = 2 };

struct Move {
    int row, col;
};

class Board {
public:
    int size;
    int winLength;
    std::vector<std::vector<Player>> grid;

    Board(int boardSize, int winLen);
    
    bool makeMove(int row, int col, Player player);
    bool isFull() const;
    Player checkWin() const;
    std::vector<Move> getAvailableMoves() const;
    
    // Zwraca wszystkie linie (wiersze, kolumny, przekątne) do analizy heurystycznej
    std::vector<std::vector<Player>> getAllLines() const;
};