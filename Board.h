#pragma once
#include "Piece.h"
#include <array>
#include <string>
#include <memory>

// A square position on the board
struct Position {
    int row;  // 0–7 (0 = rank 8, 7 = rank 1)
    int col;  // 0–7 (0 = file a, 7 = file h)

    bool isValid() const {
        return row >= 0 && row < 8 && col >= 0 && col < 8;
    }

    // Convert "e2" → Position{6, 4}
    static Position fromAlgebraic(const std::string& s);

    // Convert Position → "e2"
    std::string toAlgebraic() const;
};

// A move: from → to (simple format like "e2e4")
struct Move {
    Position from;
    Position to;

    static Move fromString(const std::string& s);
    std::string toString() const;
};

class Board {
private:
    // 8x8 grid of piece pointers (nullptr = empty square)
    std::array<std::array<std::shared_ptr<Piece>, 8>, 8> grid;

    void placePiece(int row, int col, std::shared_ptr<Piece> piece);

public:
    Board();

    // Setup standard starting position
    void initialize();

    // Get piece at position (nullptr if empty)
    std::shared_ptr<Piece> getPiece(const Position& pos) const;
    std::shared_ptr<Piece> getPiece(int row, int col) const;

    // Place / remove pieces
    void setPiece(const Position& pos, std::shared_ptr<Piece> piece);
    void removePiece(const Position& pos);

    // Apply a move (does NOT validate — MoveValidator handles that)
    void applyMove(const Move& move);

    // Generate FEN string (for sending to LLM)
    std::string toFEN(Color activeColor) const;

    // Print ASCII board to terminal
    void display() const;

    // Check if a square is empty
    bool isEmpty(const Position& pos) const;

    // Check if square has enemy piece relative to given color
    bool hasEnemy(const Position& pos, Color myColor) const;
};