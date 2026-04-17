#include "Board.h"
#include "MoveValidator.h"
#include <iostream>

int main() {
    Board board;
    board.initialize();
    MoveValidator validator;

    std::cout << "=== Chess Bot - Step 2 Test ===\n";
    board.display();

    // Test 1: legal moves for white pawn at e2
    std::cout << "Legal moves for pawn at e2:\n";
    auto moves = validator.getLegalMoves(board, Position::fromAlgebraic("e2"));
    for (auto& m : moves)
        std::cout << "  " << m.toString() << "\n";

    // Test 2: legal moves for white knight at b1
    std::cout << "\nLegal moves for knight at b1:\n";
    moves = validator.getLegalMoves(board, Position::fromAlgebraic("b1"));
    for (auto& m : moves)
        std::cout << "  " << m.toString() << "\n";

    // Test 3: total legal moves for white at start
    auto allMoves = validator.getAllLegalMoves(board, Color::WHITE);
    std::cout << "\nTotal legal moves for White at start: " << allMoves.size() << " (should be 20)\n";

    // Test 4: check detection — not in check at start
    std::cout << "White in check at start: " << (validator.isInCheck(board, Color::WHITE) ? "YES" : "NO") << "\n";

    // Test 5: apply e2e4 and check black's legal moves
    board.applyMove(Move::fromString("e2e4"));
    board.applyMove(Move::fromString("e7e5"));
    board.display();

    auto blackMoves = validator.getAllLegalMoves(board, Color::BLACK);
    std::cout << "Total legal moves for Black after 1.e4 e5: " << blackMoves.size() << "\n";

    return 0;
}