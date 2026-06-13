#include "Board.hpp"

Board::Board(int boardSize, int winLen) : size(boardSize), winLength(winLen) {
    grid.resize(size, std::vector<Player>(size, Player::EMPTY));
}

bool Board::makeMove(int row, int col, Player player) {
    if (row >= 0 && row < size && col >= 0 && col < size && grid[row][col] == Player::EMPTY) {
        grid[row][col] = player;
        return true;
    }
    return false;
}

bool Board::isFull() const {
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (grid[r][c] == Player::EMPTY) return false;
    return true;
}

std::vector<Move> Board::getAvailableMoves() const {
    std::vector<Move> moves;
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (grid[r][c] == Player::EMPTY) moves.push_back({r, c});
    return moves;
}

std::vector<std::vector<Player>> Board::getAllLines() const {
    std::vector<std::vector<Player>> lines;
    
    // Wiersze i kolumny
    for (int i = 0; i < size; ++i) {
        std::vector<Player> rowLine, colLine;
        for (int j = 0; j < size; ++j) {
            rowLine.push_back(grid[i][j]);
            colLine.push_back(grid[j][i]);
        }
        lines.push_back(rowLine);
        lines.push_back(colLine);
    }
    
    // Wszystkie diagonale (główne i sub-diagonale)
    for (int startRow = 0; startRow < size; ++startRow) {
        std::vector<Player> diag;
        for (int i = 0; i < size - startRow; ++i) {
            diag.push_back(grid[startRow + i][i]);
        }
        if (diag.size() >= static_cast<size_t>(winLength)) lines.push_back(diag);
    }
    
    for (int startCol = 1; startCol < size; ++startCol) {
        std::vector<Player> diag;
        for (int i = 0; i < size - startCol; ++i) {
            diag.push_back(grid[i][startCol + i]);
        }
        if (diag.size() >= static_cast<size_t>(winLength)) lines.push_back(diag);
    }
    
    // Diagonale od prawego górnego rogu
    for (int startRow = 0; startRow < size; ++startRow) {
        std::vector<Player> diag;
        for (int i = 0; i < size - startRow; ++i) {
            diag.push_back(grid[startRow + i][size - 1 - i]);
        }
        if (diag.size() >= static_cast<size_t>(winLength)) lines.push_back(diag);
    }
    
    for (int startCol = size - 2; startCol >= 0; --startCol) {
        std::vector<Player> diag;
        for (int i = 0; i <= startCol; ++i) {
            diag.push_back(grid[i][startCol - i]);
        }
        if (diag.size() >= static_cast<size_t>(winLength)) lines.push_back(diag);
    }
    
    return lines;
}

Player Board::checkWin() const {
    auto lines = getAllLines();
    for (const auto& line : lines) {
        int countX = 0, countO = 0;
        for (Player p : line) {
            if (p == Player::X) { countX++; countO = 0; }
            else if (p == Player::O) { countO++; countX = 0; }
            else { countX = 0; countO = 0; }

            if (countX >= winLength) return Player::X;
            if (countO >= winLength) return Player::O;
        }
    }
    return Player::EMPTY;
}