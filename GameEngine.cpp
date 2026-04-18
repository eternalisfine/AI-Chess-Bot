#include "GameEngine.h"
#include <iostream>

// ─────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────

GameEngine::GameEngine(std::shared_ptr<Player> white, std::shared_ptr<Player> black)
    : whitePlayer(white), blackPlayer(black),
      currentTurn(Color::WHITE), moveNumber(1) {
    board.initialize();
}

// ─────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────

void GameEngine::switchTurn() {
    currentTurn = (currentTurn == Color::WHITE) ? Color::BLACK : Color::WHITE;
}

std::shared_ptr<Player> GameEngine::getCurrentPlayer() {
    return (currentTurn == Color::WHITE) ? whitePlayer : blackPlayer;
}

void GameEngine::printStatus() {
    std::string turnStr = (currentTurn == Color::WHITE) ? "White" : "Black";

    if (validator.isInCheck(board, currentTurn))
        std::cout << "  *** " << turnStr << " is in CHECK! ***\n";

    std::cout << "  Move " << moveNumber << " — " << turnStr << "'s turn\n";
    std::cout << "  (type move as e.g. e2e4, or 'quit' to exit)\n";
}

// ─────────────────────────────────────────
//  Main Game Loop
// ─────────────────────────────────────────

void GameEngine::run() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════╗\n";
    std::cout << "║       CHESS BOT  v1.0        ║\n";
    std::cout << "║   You vs Local LLM (Ollama)  ║\n";
    std::cout << "╚══════════════════════════════╝\n\n";

    while (true) {
        board.display();
        printStatus();

        // ── Check game over conditions ──
        if (validator.isCheckmate(board, currentTurn)) {
            Color winner = (currentTurn == Color::WHITE) ? Color::BLACK : Color::WHITE;
            std::string winnerName = (winner == Color::WHITE)
                ? whitePlayer->getName() : blackPlayer->getName();
            std::cout << "\n  ♔  CHECKMATE! " << winnerName << " wins!\n\n";
            break;
        }

        if (validator.isStalemate(board, currentTurn)) {
            std::cout << "\n  ½  STALEMATE! It's a draw.\n\n";
            break;
        }

        // ── Get move from current player ──
        auto player = getCurrentPlayer();
        std::cout << "\n";
        Move move = player->getMove(board, validator);

        // ── Apply move ──
        board.applyMove(move);
        std::cout << "  → " << player->getName()
                  << " played: " << move.toString() << "\n\n";

        // ── Pawn promotion (auto-promote to Queen) ──
        auto piece = board.getPiece(move.to);
        if (piece && piece->getName() == "Pawn") {
            int promoteRow = (piece->getColor() == Color::WHITE) ? 0 : 7;
            if (move.to.row == promoteRow) {
                board.setPiece(move.to, std::make_shared<Queen>(piece->getColor()));
                std::cout << "  ★  Pawn promoted to Queen!\n\n";
            }
        }

        // ── Advance move counter after black plays ──
        if (currentTurn == Color::BLACK) moveNumber++;

        switchTurn();
    }
}