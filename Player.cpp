#include "Player.h"
#include <iostream>

Move HumanPlayer::getMove(const Board& board, MoveValidator& validator) {
    while (true) {
        std::cout << name << " (" << (color == Color::WHITE ? "White" : "Black") << ") > ";
        std::string input;
        std::cin >> input;

        // Allow quit
        if (input == "quit" || input == "exit") {
            std::cout << "Thanks for playing!\n";
            exit(0);
        }

        // Basic format check
        if (input.size() < 4) {
            std::cout << "  Invalid format. Use e.g. e2e4\n";
            continue;
        }

        // Parse move
        Move move;
        try {
            move = Move::fromString(input);
        } catch (...) {
            std::cout << "  Couldn't parse move. Use e.g. e2e4\n";
            continue;
        }

        // Check there's a piece at from
        auto piece = board.getPiece(move.from);
        if (!piece) {
            std::cout << "  No piece at " << move.from.toAlgebraic() << "\n";
            continue;
        }

        // Check it's your piece
        if (piece->getColor() != color) {
            std::cout << "  That's not your piece!\n";
            continue;
        }

        // Validate move
        if (!validator.isMoveLegal(board, move, color)) {
            std::cout << "  Illegal move. Try again.\n";
            continue;
        }

        return move;
    }
}