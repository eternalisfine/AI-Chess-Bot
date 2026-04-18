#pragma once
#include "Board.h"
#include "MoveValidator.h"
#include "Player.h"
#include <memory>

// ─────────────────────────────────────────
//  GameEngine — controls the game loop
// ─────────────────────────────────────────
class GameEngine {
private:
    Board board;
    MoveValidator validator;
    std::shared_ptr<Player> whitePlayer;
    std::shared_ptr<Player> blackPlayer;
    Color currentTurn;
    int moveNumber;

    // Print game status (check, whose turn etc.)
    void printStatus();

    // Switch turn
    void switchTurn();

    // Get current player
    std::shared_ptr<Player> getCurrentPlayer();

public:
    GameEngine(std::shared_ptr<Player> white, std::shared_ptr<Player> black);

    // Start and run the game loop
    void run();
};