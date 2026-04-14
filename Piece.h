#pragma once
#include <string>

enum class Color { WHITE, BLACK, NONE };

// ─────────────────────────────────────────
//  Abstract Base Class
// ─────────────────────────────────────────
class Piece {
    protected:
        Color color;
        char symbol;  // uppercase = white, lowercase = black

    public:
        Piece(Color c, char sym) : color(c), symbol(sym) {}
    
        virtual ~Piece() = default;

        Color getColor() const { return color; }
        char getSymbol() const { return symbol; }

        // Each piece knows its own name
        virtual std::string getName() const = 0;

        // Returns single char for board display
        char display() const { return symbol; }
};

// ─────────────────────────────────────────
//  Concrete Piece Subclasses
// ─────────────────────────────────────────
class King : public Piece {
    public:
        King(Color c) : Piece(c, c == Color::WHITE ? 'K' : 'k') {}
        std::string getName() const override { return "King"; }
};

class Queen : public Piece {
    public:
        Queen(Color c) : Piece(c, c == Color::WHITE ? 'Q' : 'q') {}
        std::string getName() const override { return "Queen"; }
};

class Rook : public Piece {
    public:
        Rook(Color c) : Piece(c, c == Color::WHITE ? 'R' : 'r') {}
        std::string getName() const override { return "Rook"; }
};

class Bishop : public Piece {
    public:
        Bishop(Color c) : Piece(c, c == Color::WHITE ? 'B' : 'b') {}
        std::string getName() const override { return "Bishop"; }
};

class Knight : public Piece {
    public:
        Knight(Color c) : Piece(c, c == Color::WHITE ? 'N' : 'n') {}
        std::string getName() const override { return "Knight"; }
};

class Pawn : public Piece {
    public:
        Pawn(Color c) : Piece(c, c == Color::WHITE ? 'P' : 'p') {}
        std::string getName() const override { return "Pawn"; }
};