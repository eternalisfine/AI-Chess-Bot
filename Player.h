#pragma once
#include "Board.h"
#include "MoveValidator.h"
#include <string>

// ─────────────────────────────────────────
//  Abstract Base Class: Player
// ─────────────────────────────────────────
class Player {
protected:
    Color color;
    std::string name;

public:
    Player(Color c, const std::string& n) : color(c), name(n) {}
    virtual ~Player() = default;

    Color getColor() const { return color; }
    std::string getName() const { return name; }

    // Each player type implements how they pick a move
    virtual Move getMove(const Board& board, MoveValidator& validator) = 0;
};

// ─────────────────────────────────────────
//  HumanPlayer — reads move from CLI
// ─────────────────────────────────────────
class HumanPlayer : public Player {
public:
    HumanPlayer(Color c, const std::string& n) : Player(c, n) {}

    Move getMove(const Board& board, MoveValidator& validator) override;
};