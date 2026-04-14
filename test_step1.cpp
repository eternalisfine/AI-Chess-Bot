#include "Board.h"
#include <iostream>

using namespace std;

int main() {
    Board board;
    board.initialize();

    cout << "=== Chess Bot - Step 1 Test ===\n";
    board.display();

    cout << "FEN: " << board.toFEN(Color::WHITE) << "\n\n";

    // Test move: e2 → e4
    Move m = Move::fromString("e2e4");
    cout << "Applying move: " << m.toString() << "\n";
    board.applyMove(m);
    board.display();

    cout << "FEN after move: " << board.toFEN(Color::BLACK) << "\n";
    return 0;
}