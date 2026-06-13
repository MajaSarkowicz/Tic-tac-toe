#pragma once
#include "Board.hpp"
#include <unordered_map>
#include <string>
#include <chrono>
#include <limits>

struct TranspositionEntry {
    int score;
    int depth;
    uint8_t flag; // 0 = exact, 1 = lower bound, 2 = upper bound
};

class AI {
public:
    AI(Player aiPlayer, Player humanPlayer);
    Move getBestMove(const Board& board);

private:
    Player ai_player, human_player;
    std::unordered_map<std::string, TranspositionEntry> transpositionTable;
    std::chrono::high_resolution_clock::time_point startTime;
    static constexpr int TIME_LIMIT_MS = 3000;
    
    int calculateMaxDepth(const Board& board) const;
    std::string boardHash(const Board& board) const;
    int evaluateLine(const std::vector<Player>& line) const;
    int evaluateBoard(const Board& board) const;
    int minimax(Board& board_copy, int depth, int alpha, int beta, bool isMaximizing);
    bool timeExpired() const;
    std::vector<Move> orderMoves(const Board& board, const std::vector<Move>& moves) const;
    int getMoveScore(const Board& board, const Move& move) const;
};