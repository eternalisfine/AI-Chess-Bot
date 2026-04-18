#include "GameEngine.h"
#include "Player.h"
#include <memory>

int main() {
    // Human vs Human to test game loop (Bot comes in Step 4)
    auto white = std::make_shared<HumanPlayer>(Color::WHITE, "Player");
    auto black = std::make_shared<HumanPlayer>(Color::BLACK, "Player2");

    GameEngine game(white, black);
    game.run();

    return 0;
}