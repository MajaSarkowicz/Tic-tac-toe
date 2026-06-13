#include "AI.hpp"
#include <cmath>
#include <algorithm>
#include <string>

AI::AI(Player aiPlayer, Player humanPlayer) : ai_player(aiPlayer), human_player(humanPlayer) {}

std::string AI::boardHash(const Board& board) const {
    std::string hash;
    for (int r = 0; r < board.size; ++r)
        for (int c = 0; c < board.size; ++c)
            hash += static_cast<char>(board.grid[r][c]);
    return hash;
}

bool AI::timeExpired() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    return elapsed > TIME_LIMIT_MS;
}

int AI::calculateMaxDepth(const Board& board) const {
    int emptyCells = board.getAvailableMoves().size();
    if (emptyCells > 20) return 3;
    if (emptyCells > 16) return 4;
    if (emptyCells > 10) return 5;
    return 6;
}

int AI::getMoveScore(const Board& board, const Move& move) const {
    int centerX = board.size / 2;
    int centerY = board.size / 2;
    
    // Preferuj ruchy bliżej środka
    int distFromCenter = std::abs(move.row - centerX) + std::abs(move.col - centerY);
    int centerScore = 100 - distFromCenter;
    
    // Preferuj ruchy blisko istniejących ruchów
    int proximityScore = 0;
    for (int r = std::max(0, move.row - 2); r <= std::min(board.size - 1, move.row + 2); ++r) {
        for (int c = std::max(0, move.col - 2); c <= std::min(board.size - 1, move.col + 2); ++c) {
            if (board.grid[r][c] != Player::EMPTY) {
                proximityScore += 50;
            }
        }
    }
    
    return centerScore + proximityScore;
}

std::vector<Move> AI::orderMoves(const Board& board, const std::vector<Move>& moves) const {
    std::vector<std::pair<Move, int>> scoredMoves;
    
    for (const auto& move : moves) {
        scoredMoves.push_back({move, getMoveScore(board, move)});
    }
    
    std::sort(scoredMoves.begin(), scoredMoves.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<Move> orderedMoves;
    for (const auto& sm : scoredMoves) {
        orderedMoves.push_back(sm.first);
    }
    return orderedMoves;
}


int AI::evaluateLine(const std::vector<Player>& line) const {
    int score = 0;
    int aiCount = 0, humanCount = 0, emptyCount = 0;
    
    for (Player p : line) {
        if (p == ai_player) aiCount++;
        else if (p == human_player) humanCount++;
        else emptyCount++;
    }
    
    // Tylko oceniaj linie z szansą na wygraną
    if (aiCount > 0 && humanCount > 0) return 0;
    
    int lineSize = static_cast<int>(line.size());
    if (aiCount > 0 && humanCount == 0) {
        if (aiCount == lineSize) score += 50000;
        else if (aiCount == lineSize - 1 && emptyCount > 0) score += 5000;
        else if (aiCount >= 2) score += 10 * aiCount;
        else score += aiCount;
    } else if (humanCount > 0 && aiCount == 0) {
        if (humanCount == lineSize) score -= 50000;
        else if (humanCount == lineSize - 1 && emptyCount > 0) score -= 5000;
        else if (humanCount >= 2) score -= 10 * humanCount;
        else score -= humanCount;
    }
    return score;
}

int AI::evaluateBoard(const Board& board) const {
    int score = 0;
    auto lines = board.getAllLines();
    
    for (const auto& line : lines) {
        score += evaluateLine(line);
    }
    return score;
}


int AI::minimax(Board& board_copy, int depth, int alpha, int beta, bool isMaximizing) {
    Player winner = board_copy.checkWin();
    if (winner == ai_player) return 10000 + depth;
    if (winner == human_player) return -10000 - depth;
    if (board_copy.isFull() || depth == 0) return evaluateBoard(board_copy);
    
    if (timeExpired()) return evaluateBoard(board_copy);
    
    std::string hash = boardHash(board_copy);
    auto it = transpositionTable.find(hash);
    if (it != transpositionTable.end() && it->second.depth >= depth) {
        return it->second.score;
    }

    auto moves = orderMoves(board_copy, board_copy.getAvailableMoves());

    if (isMaximizing) {
        int maxEval = std::numeric_limits<int>::min();
        for (const auto& move : moves) {
            board_copy.grid[move.row][move.col] = ai_player;
            int eval = minimax(board_copy, depth - 1, alpha, beta, false);
            board_copy.grid[move.row][move.col] = Player::EMPTY;
            
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        transpositionTable[hash] = {maxEval, depth, 0};
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (const auto& move : moves) {
            board_copy.grid[move.row][move.col] = human_player;
            int eval = minimax(board_copy, depth - 1, alpha, beta, true);
            board_copy.grid[move.row][move.col] = Player::EMPTY;
            
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        transpositionTable[hash] = {minEval, depth, 0};
        return minEval;
    }
}

Move AI::getBestMove(const Board& board) {
    startTime = std::chrono::high_resolution_clock::now();
    transpositionTable.clear();
    
    int bestVal = std::numeric_limits<int>::min();
    Move bestMove = {-1, -1};
    int depth = calculateMaxDepth(board);
    
    auto moves = orderMoves(board, board.getAvailableMoves());

    for (const auto& move : moves) {
        if (timeExpired()) break;
        
        Board board_copy = board;
        board_copy.makeMove(move.row, move.col, ai_player);
        int moveVal = minimax(board_copy, depth - 1, 
                             std::numeric_limits<int>::min(), 
                             std::numeric_limits<int>::max(), 
                             false);
        
        if (moveVal > bestVal) {
            bestMove = move;
            bestVal = moveVal;
        }
    }
    
    return bestMove;
}