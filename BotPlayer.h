#pragma once
#include "Player.h"
#include "OllamaClient.h"
#include <memory>

// ─────────────────────────────────────────
//  BotPlayer — uses Ollama LLM to pick moves
// ─────────────────────────────────────────
class BotPlayer : public Player {
private:
    OllamaClient client;
    int maxRetries;  // retries if LLM returns bad/illegal move

public:
    BotPlayer(Color c, const std::string& n,
              const std::string& model = "llama3.2:1b",
              int retries = 3)
        : Player(c, n), client(model), maxRetries(retries) {}

    Move getMove(const Board& board, MoveValidator& validator) override;
};