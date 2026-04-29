#include "Board.h"
#include "MoveValidator.h"
#include "OllamaClient.h"
#include "BotPlayer.h"
#include "Player.h"
#include <iostream>
#include <string>
#include <sstream>

// ─────────────────────────────────────────
// chess_engine: reads commands from stdin, writes JSON to stdout
// Commands:
//   INIT
//   LEGAL <from>        → lists legal moves from a square
//   MOVE <from><to>     → apply human move, returns new state
//   BOTMOVE <model>     → get + apply bot move
//   STATE               → return current board state
// ─────────────────────────────────────────

Board board;
MoveValidator validator;
Color currentTurn = Color::WHITE;
std::string gameStatus = "playing"; // playing, checkmate, stalemate

std::string boardToJSON() {
    std::string json = "{";
    json += "\"fen\":\"" + board.toFEN(currentTurn) + "\",";
    json += "\"turn\":\"" + std::string(currentTurn == Color::WHITE ? "white" : "black") + "\",";
    json += "\"status\":\"" + gameStatus + "\",";

    // Board grid as array
    json += "\"board\":[";
    for (int r = 0; r < 8; r++) {
        json += "[";
        for (int c = 0; c < 8; c++) {
            auto p = board.getPiece(r, c);
            if (!p) {
                json += "null";
            } else {
                json += "{\"symbol\":\"";
                json += p->display();
                json += "\",\"color\":\"";
                json += (p->getColor() == Color::WHITE ? "white" : "black");
                json += "\",\"name\":\"" + p->getName() + "\"}";
            }
            if (c < 7) json += ",";
        }
        json += "]";
        if (r < 7) json += ",";
    }
    json += "]";
    json += "}";
    return json;
}

void updateGameStatus() {
    if (validator.isCheckmate(board, currentTurn))
        gameStatus = "checkmate";
    else if (validator.isStalemate(board, currentTurn))
        gameStatus = "stalemate";
    else if (validator.isInCheck(board, currentTurn))
        gameStatus = "check";
    else
        gameStatus = "playing";
}

int main() {
    board.initialize();
    updateGameStatus();

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        if (line == "INIT") {
            board.initialize();
            currentTurn = Color::WHITE;
            gameStatus = "playing";
            std::cout << boardToJSON() << std::endl; // Use std::endl to flush the buffer
        }
        else if (line == "STATE") {
            std::cout << boardToJSON() << "\n";
            std::cout.flush();
        }
        else if (line.substr(0, 5) == "LEGAL") {
            std::string sq = line.substr(6);
            try {
                // Handle potential trailing spaces or case issues
                if (sq.size() > 2) sq = sq.substr(0, 2); 
                
                Position pos = Position::fromAlgebraic(sq);
                auto moves = validator.getLegalMoves(board, pos);
                
                // CRITICAL: Must be wrapped in {"legalMoves": [...]}
                // Change this block in your LEGAL command handler:
                std::cout << "{\"legalMoves\": ["; 
                for (size_t i = 0; i < moves.size(); i++) {
                // moves[i].toString() gives "e2e4" instead of just "e4"
                std::cout << "\"" << moves[i].toString() << "\""; 
                if (i + 1 < moves.size()) std::cout << ",";
                }
                std::cout << "]}" << std::endl;
            } catch (...) {
                // Return empty object on error to prevent JS from crashing
                std::cout << "{\"legalMoves\": []}" << std::endl;
            }
        }
        else if (line.substr(0, 4) == "MOVE") {
            std::string mv = line.substr(5);
            try {
                Move move = Move::fromString(mv);
                if (validator.isMoveLegal(board, move, currentTurn)) {
                    board.applyMove(move);

                    // Pawn promotion
                    auto piece = board.getPiece(move.to);
                    if (piece && piece->getName() == "Pawn") {
                        int promoteRow = (piece->getColor() == Color::WHITE) ? 0 : 7;
                        if (move.to.row == promoteRow)
                            board.setPiece(move.to, std::make_shared<Queen>(piece->getColor()));
                    }

                    currentTurn = (currentTurn == Color::WHITE) ? Color::BLACK : Color::WHITE;
                    updateGameStatus();
                    std::cout << boardToJSON() << "\n";
                } else {
                    std::cout << "{\"error\":\"illegal move\"}\n";
                }
            } catch (...) {
                std::cout << "{\"error\":\"invalid move format\"}\n";
            }
            std::cout.flush();
        }
        else if (line.substr(0, 7) == "BOTMOVE") {
            std::string model = line.size() > 8 ? line.substr(8) : "llama3.2:1b";
            auto legalMoves = validator.getAllLegalMoves(board, currentTurn);
            if (legalMoves.empty()) {
                std::cout << "{\"error\":\"no legal moves\"}\n";
                std::cout.flush();
                continue;
            }

            std::vector<std::string> moveStrs;
            for (auto& m : legalMoves) moveStrs.push_back(m.toString());

            OllamaClient client(model);
            std::string colorStr = (currentTurn == Color::WHITE) ? "White" : "Black";
            std::string fen = board.toFEN(currentTurn);

            Move chosenMove = legalMoves[0]; // fallback
            std::string moveStr = client.getMove(fen, moveStrs, colorStr);

            if (!moveStr.empty()) {
                try {
                    Move m = Move::fromString(moveStr);
                    if (validator.isMoveLegal(board, m, currentTurn))
                        chosenMove = m;
                } catch (...) {}
            }

            board.applyMove(chosenMove);

            // Pawn promotion
            auto piece = board.getPiece(chosenMove.to);
            if (piece && piece->getName() == "Pawn") {
                int promoteRow = (piece->getColor() == Color::WHITE) ? 0 : 7;
                if (chosenMove.to.row == promoteRow)
                    board.setPiece(chosenMove.to, std::make_shared<Queen>(piece->getColor()));
            }

            currentTurn = (currentTurn == Color::WHITE) ? Color::BLACK : Color::WHITE;
            updateGameStatus();

            // Add bot's move to response
            std::string resp = boardToJSON();
            // inject botMove field
            resp.insert(1, "\"botMove\":\"" + chosenMove.toString() + "\",");
            std::cout << resp << "\n";
            std::cout.flush();
        }
    }
    return 0;
}