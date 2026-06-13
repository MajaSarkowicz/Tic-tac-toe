#include "GameApp.hpp"
#include <imgui.h>

GameApp::GameApp() {}

void GameApp::render() {
    if (!gameStarted) {
        renderMenu();
    } else {
        renderGame();
    }
}

void GameApp::renderMenu() {
    ImGui::Begin("Konfiguracja Gry");
    ImGui::SliderInt("Rozmiar planszy", &boardSize, 3, 10);
    if (winLength > boardSize) winLength = boardSize;
    ImGui::SliderInt("Dlugosc do wygranej", &winLength, 2, boardSize);
    ImGui::Checkbox("Graj przeciwko SI", &vsAI);
    if (vsAI) {
        ImGui::Checkbox("SI wykonuje pierwszy ruch", &aiGoesFirst);
    }

    if (ImGui::Button("Start Gry", ImVec2(120, 40))) {
        board = std::make_unique<Board>(boardSize, winLength);
        if (vsAI) {
            ai = std::make_unique<AI>(aiGoesFirst ? Player::X : Player::O, aiGoesFirst ? Player::O : Player::X);
        }
        currentPlayer = Player::X;
        winner = Player::EMPTY;
        isDraw = false;
        gameStarted = true;
        
        if (vsAI && aiGoesFirst) handleComputerMove();
    }
    ImGui::End();
}

void GameApp::handleComputerMove() {
    if (winner == Player::EMPTY && !isDraw) {
        Move bestMove = ai->getBestMove(*board);
        board->makeMove(bestMove.row, bestMove.col, currentPlayer);
        
        winner = board->checkWin();
        if (winner == Player::EMPTY && board->isFull()) isDraw = true;
        
        currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
    }
}

void GameApp::renderGame() {
    ImGui::Begin("Kolko i Krzyzyk", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    if (winner != Player::EMPTY) {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Gracz %s Wygrywa!", winner == Player::X ? "X" : "O");
    } else if (isDraw) {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Remis!");
    } else {
        ImGui::Text("Tura gracza: %s", currentPlayer == Player::X ? "X" : "O");
    }

    for (int r = 0; r < board->size; ++r) {
        for (int c = 0; c < board->size; ++c) {
            ImGui::PushID(r * board->size + c);
            const char* label = " ";
            if (board->grid[r][c] == Player::X) label = "X";
            if (board->grid[r][c] == Player::O) label = "O";

            if (ImGui::Button(label, ImVec2(50, 50)) && winner == Player::EMPTY && !isDraw && board->grid[r][c] == Player::EMPTY) {
                board->makeMove(r, c, currentPlayer);
                winner = board->checkWin();
                if (winner == Player::EMPTY && board->isFull()) isDraw = true;
                
                currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;

                if (vsAI && winner == Player::EMPTY && !isDraw) {
                    handleComputerMove();
                }
            }
            ImGui::PopID();
            if (c < board->size - 1) ImGui::SameLine();
        }
    }

    if (ImGui::Button("Powrot do Menu", ImVec2(150, 30))) {
        gameStarted = false;
    }
    ImGui::End();
}