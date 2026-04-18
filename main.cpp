#include "GameEngine.h"
#include "Player.h"
#include "BotPlayer.h"
#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
    // Default model — can override via command line arg
    // Usage: ./chess_bot [model_name]
    // e.g.:  ./chess_bot llama3.2:1b
    //        ./chess_bot qwen2.5:1b
    std::string model = "llama3.2:1b";
    if (argc > 1) model = argv[1];

    std::cout << "\n  Using Ollama model: " << model << "\n";
    std::cout << "  Make sure Ollama is running: ollama serve\n";
    std::cout << "  And model is pulled:         ollama pull " << model << "\n\n";

    // You = White (Human), Bot = Black (LLM)
    auto human = std::make_shared<HumanPlayer>(Color::WHITE, "You");
    auto bot   = std::make_shared<BotPlayer>(Color::BLACK, "Bot", model);

    GameEngine game(human, bot);
    game.run();

    return 0;
}