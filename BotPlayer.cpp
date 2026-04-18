#include "BotPlayer.h"
#include <iostream>

Move BotPlayer::getMove(const Board& board, MoveValidator& validator) {
    // Get all legal moves for bot
    auto legalMoves = validator.getAllLegalMoves(board, color);

    // Convert to strings for prompt
    std::vector<std::string> moveStrs;
    for (auto& m : legalMoves)
        moveStrs.push_back(m.toString());

    // Get FEN for current position
    std::string fen = board.toFEN(color);
    std::string colorStr = (color == Color::WHITE) ? "White" : "Black";

    // Try up to maxRetries times
    for (int attempt = 1; attempt <= maxRetries; attempt++) {
        std::string moveStr = client.getMove(fen, moveStrs, colorStr);

        if (moveStr.empty()) {
            std::cout << "  [Bot returned empty response, attempt " << attempt << "/" << maxRetries << "]\n";
            continue;
        }

        // Try to parse and validate the move
        try {
            Move move = Move::fromString(moveStr);

            if (validator.isMoveLegal(board, move, color)) {
                return move;  // good move
            } else {
                std::cout << "  [Bot suggested illegal move: " << moveStr
                          << ", attempt " << attempt << "/" << maxRetries << "]\n";
            }
        } catch (...) {
            std::cout << "  [Bot returned invalid format: " << moveStr
                      << ", attempt " << attempt << "/" << maxRetries << "]\n";
        }
    }

    // Fallback: pick first legal move if LLM keeps failing
    std::cout << "  [Bot falling back to first legal move]\n";
    return legalMoves[0];
}