#include "Board.h"
#include <iostream>
#include <stdexcept>

// ─────────────────────────────────────────
//  Position helpers
// ─────────────────────────────────────────

Position Position::fromAlgebraic(const std::string& s) {
    // "e2" → col=4, row=6
    if (s.size() < 2) throw std::invalid_argument("Invalid position: " + s);
    int col = s[0] - 'a';          // 'a'=0 ... 'h'=7
    int row = 8 - (s[1] - '0');    // '1'=row7 ... '8'=row0
    return {row, col};
}

std::string Position::toAlgebraic() const {
    std::string s;
    s += (char)('a' + col);
    s += (char)('0' + (8 - row));
    return s;
}

// ─────────────────────────────────────────
//  Move helpers
// ─────────────────────────────────────────

Move Move::fromString(const std::string& s) {
    // "e2e4" → from=e2, to=e4
    if (s.size() < 4) throw std::invalid_argument("Invalid move: " + s);
    return { Position::fromAlgebraic(s.substr(0, 2)),
             Position::fromAlgebraic(s.substr(2, 2)) };
}

std::string Move::toString() const {
    return from.toAlgebraic() + to.toAlgebraic();
}

// ─────────────────────────────────────────
//  Board constructor
// ─────────────────────────────────────────

Board::Board() {
    // Fill grid with nullptrs
    for (auto& row : grid)
        row.fill(nullptr);
}

void Board::placePiece(int row, int col, std::shared_ptr<Piece> piece) {
    grid[row][col] = piece;
}

// ─────────────────────────────────────────
//  Initialize standard starting position
// ─────────────────────────────────────────

void Board::initialize() {
    // Clear board
    for (auto& row : grid) row.fill(nullptr);

    // ── Black pieces (row 0 = rank 8) ──
    placePiece(0, 0, std::make_shared<Rook>(Color::BLACK));
    placePiece(0, 1, std::make_shared<Knight>(Color::BLACK));
    placePiece(0, 2, std::make_shared<Bishop>(Color::BLACK));
    placePiece(0, 3, std::make_shared<Queen>(Color::BLACK));
    placePiece(0, 4, std::make_shared<King>(Color::BLACK));
    placePiece(0, 5, std::make_shared<Bishop>(Color::BLACK));
    placePiece(0, 6, std::make_shared<Knight>(Color::BLACK));
    placePiece(0, 7, std::make_shared<Rook>(Color::BLACK));
    for (int c = 0; c < 8; c++)
        placePiece(1, c, std::make_shared<Pawn>(Color::BLACK));

    // ── White pieces (row 7 = rank 1) ──
    placePiece(7, 0, std::make_shared<Rook>(Color::WHITE));
    placePiece(7, 1, std::make_shared<Knight>(Color::WHITE));
    placePiece(7, 2, std::make_shared<Bishop>(Color::WHITE));
    placePiece(7, 3, std::make_shared<Queen>(Color::WHITE));
    placePiece(7, 4, std::make_shared<King>(Color::WHITE));
    placePiece(7, 5, std::make_shared<Bishop>(Color::WHITE));
    placePiece(7, 6, std::make_shared<Knight>(Color::WHITE));
    placePiece(7, 7, std::make_shared<Rook>(Color::WHITE));
    for (int c = 0; c < 8; c++)
        placePiece(6, c, std::make_shared<Pawn>(Color::WHITE));
}

// ─────────────────────────────────────────
//  Getters / Setters
// ─────────────────────────────────────────

std::shared_ptr<Piece> Board::getPiece(const Position& pos) const {
    return grid[pos.row][pos.col];
}

std::shared_ptr<Piece> Board::getPiece(int row, int col) const {
    return grid[row][col];
}

void Board::setPiece(const Position& pos, std::shared_ptr<Piece> piece) {
    grid[pos.row][pos.col] = piece;
}

void Board::removePiece(const Position& pos) {
    grid[pos.row][pos.col] = nullptr;
}

bool Board::isEmpty(const Position& pos) const {
    return grid[pos.row][pos.col] == nullptr;
}

bool Board::hasEnemy(const Position& pos, Color myColor) const {
    auto piece = grid[pos.row][pos.col];
    if (!piece) return false;
    return piece->getColor() != myColor;
}

// ─────────────────────────────────────────
//  Apply Move
// ─────────────────────────────────────────

void Board::applyMove(const Move& move) {
    auto piece = getPiece(move.from);
    setPiece(move.to, piece);
    removePiece(move.from);
}

// ─────────────────────────────────────────
//  FEN Generator (sends board state to LLM)
// ─────────────────────────────────────────

std::string Board::toFEN(Color activeColor) const {
    std::string fen = "";

    for (int r = 0; r < 8; r++) {
        int empty = 0;
        for (int c = 0; c < 8; c++) {
            auto piece = grid[r][c];
            if (!piece) {
                empty++;
            } else {
                if (empty > 0) { fen += std::to_string(empty); empty = 0; }
                fen += piece->display();
            }
        }
        if (empty > 0) fen += std::to_string(empty);
        if (r < 7) fen += '/';
    }

    fen += (activeColor == Color::WHITE) ? " w" : " b";
    fen += " - - 0 1";  // simplified: no castling/en-passant tracking
    return fen;
}

// ─────────────────────────────────────────
//  ASCII Board Display
// ─────────────────────────────────────────

void Board::display() const {
    std::cout << "\n    a  b  c  d  e  f  g  h\n";
    std::cout << "  +------------------------+\n";

    for (int r = 0; r < 8; r++) {
        int rank = 8 - r;
        std::cout << rank << " | ";
        for (int c = 0; c < 8; c++) {
            auto piece = grid[r][c];
            // Checkerboard shading with dots and spaces
            if (!piece) {
                std::cout << ((r + c) % 2 == 0 ? " . " : "   ");
            } else {
                std::cout << " " << piece->display() << " ";
            }
        }
        std::cout << "| " << rank << "\n";
    }

    std::cout << "  +------------------------+\n";
    std::cout << "    a  b  c  d  e  f  g  h\n\n";
}