#pragma once
#include <string>
#include <vector>

// ─────────────────────────────────────────
//  OllamaClient — talks to local Ollama API
// ─────────────────────────────────────────
class OllamaClient {
private:
    std::string host;   // default: http://localhost:11434
    std::string model;  // e.g. "llama3.2:1b"

    // Send raw POST request, return response body
    std::string sendRequest(const std::string& prompt);

    // Parse the move string out of LLM response
    std::string extractMove(const std::string& response);

public:
    OllamaClient(const std::string& model = "llama3.2:1b",
                 const std::string& host  = "http://localhost:11434");

    // Build prompt and get a move from LLM
    // legalMoves: list of all legal moves in current position
    // Returns move string like "e7e5", or "" on failure
    std::string getMove(const std::string& fen,
                        const std::vector<std::string>& legalMoves,
                        const std::string& colorStr);
};