#include "OllamaClient.h"
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

// ─────────────────────────────────────────
//  libcurl write callback
// ─────────────────────────────────────────

static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total = size * nmemb;
    output->append((char*)contents, total);
    return total;
}

// ─────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────

OllamaClient::OllamaClient(const std::string& model, const std::string& host)
    : host(host), model(model) {}

// ─────────────────────────────────────────
//  Send HTTP POST to Ollama
// ─────────────────────────────────────────

std::string OllamaClient::sendRequest(const std::string& prompt) {
    CURL* curl = curl_easy_init();
    if (!curl) throw std::runtime_error("Failed to init curl");

    std::string url = host + "/api/generate";
    std::string response;

    // Escape prompt for JSON (basic escaping)
    std::string escaped;
    for (char c : prompt) {
        if      (c == '"')  escaped += "\\\"";
        else if (c == '\\') escaped += "\\\\";
        else if (c == '\n') escaped += "\\n";
        else if (c == '\r') escaped += "\\r";
        else if (c == '\t') escaped += "\\t";
        else escaped += c;
    }

    // Build JSON body — stream:false so we get one complete response
    std::string body = "{\"model\":\"" + model + "\","
                        "\"prompt\":\"" + escaped + "\","
                        "\"stream\":false,"
                        "\"options\":{\"temperature\":0.2}}";

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL,            url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,     body.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER,     headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,      &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,        60L);  // 60s timeout

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
        throw std::runtime_error(std::string("curl error: ") + curl_easy_strerror(res));

    return response;
}

// ─────────────────────────────────────────
//  Extract move from LLM response
//  Ollama returns: {"response":"e7e5","done":true,...}
// ─────────────────────────────────────────

std::string OllamaClient::extractMove(const std::string& response) {
    // Pull out "response":"..." field
    std::string key = "\"response\":\"";
    size_t start = response.find(key);
    if (start == std::string::npos) return "";
    start += key.size();

    size_t end = response.find("\"", start);
    if (end == std::string::npos) return "";

    std::string text = response.substr(start, end - start);

    // Trim whitespace / newlines
    text.erase(0, text.find_first_not_of(" \t\n\r"));
    text.erase(text.find_last_not_of(" \t\n\r") + 1);

    // Scan for a 4-char move pattern like e7e5 anywhere in the text
    for (size_t i = 0; i + 3 < text.size(); i++) {
        if (std::isalpha(text[i])   && text[i]   >= 'a' && text[i]   <= 'h' &&
            std::isdigit(text[i+1]) && text[i+1] >= '1' && text[i+1] <= '8' &&
            std::isalpha(text[i+2]) && text[i+2] >= 'a' && text[i+2] <= 'h' &&
            std::isdigit(text[i+3]) && text[i+3] >= '1' && text[i+3] <= '8') {
            return text.substr(i, 4);
        }
    }

    return "";  // couldn't find a move
}

// ─────────────────────────────────────────
//  Build Prompt + Get Move
// ─────────────────────────────────────────

std::string OllamaClient::getMove(const std::string& fen,
                                   const std::vector<std::string>& legalMoves,
                                   const std::string& colorStr) {
    // Build legal moves string
    std::string movesStr;
    for (size_t i = 0; i < legalMoves.size(); i++) {
        movesStr += legalMoves[i];
        if (i + 1 < legalMoves.size()) movesStr += ", ";
    }

    // Prompt engineered for small 1B models:
    // - Give FEN
    // - Give exact list of legal moves (prevents hallucination)
    // - Ask for ONE move only
    std::string prompt =
        "You are a chess engine playing as " + colorStr + ".\n"
        "Current board position (FEN): " + fen + "\n"
        "Legal moves you can make: " + movesStr + "\n\n"
        "Pick the best move from the legal moves list above.\n"
        "Reply with ONLY the move in format like e7e5. Nothing else.";

    std::cout << "  [Bot thinking...]\n";

    try {
        std::string response = sendRequest(prompt);
        std::string move = extractMove(response);
        return move;
    } catch (const std::exception& e) {
        std::cerr << "  [Ollama error: " << e.what() << "]\n";
        return "";
    }
}