#pragma once
#include "Board.h"
#include <vector>

class MoveValidator {
private:
    // Raw moves (ignoring check)
    std::vector<Move> getPawnMoves(const Board& board, const Position& pos);
    std::vector<Move> getRookMoves(const Board& board, const Position& pos);
    std::vector<Move> getBishopMoves(const Board& board, const Position& pos);
    std::vector<Move> getQueenMoves(const Board& board, const Position& pos);
    std::vector<Move> getKnightMoves(const Board& board, const Position& pos);
    std::vector<Move> getKingMoves(const Board& board, const Position& pos);

    // Sliding piece helper (rook/bishop/queen)
    std::vector<Move> getSlidingMoves(const Board& board, const Position& pos,
                                      const std::vector<std::pair<int,int>>& directions);

    // Find king position for a given color
    Position findKing(const Board& board, Color color);

public:
    // Is a square attacked by any piece of `attackerColor`?
    bool isSquareAttacked(const Board& board, const Position& pos, Color attackerColor);

    // Is the given color's king currently in check?
    bool isInCheck(const Board& board, Color color);

    // Get all legal moves for a piece at pos (filters out moves leaving king in check)
    std::vector<Move> getLegalMoves(const Board& board, const Position& pos);

    // Get all legal moves for a color
    std::vector<Move> getAllLegalMoves(const Board& board, Color color);

    // Is a specific move legal?
    bool isMoveLegal(const Board& board, const Move& move, Color color);

    // Is it checkmate?
    bool isCheckmate(const Board& board, Color color);

    // Is it stalemate?
    bool isStalemate(const Board& board, Color color);
};