#include "MoveValidator.h"
#include <algorithm>

// ─────────────────────────────────────────
//  Sliding pieces (Rook, Bishop, Queen)
// ─────────────────────────────────────────

std::vector<Move> MoveValidator::getSlidingMoves(const Board& board, const Position& pos,
                                                  const std::vector<std::pair<int,int>>& directions) {
    std::vector<Move> moves;
    auto piece = board.getPiece(pos);
    if (!piece) return moves;

    for (auto& [dr, dc] : directions) {
        Position cur = {pos.row + dr, pos.col + dc};
        while (cur.isValid()) {
            if (board.isEmpty(cur)) {
                moves.push_back({pos, cur});
            } else {
                if (board.hasEnemy(cur, piece->getColor()))
                    moves.push_back({pos, cur});  // capture
                break;  // blocked
            }
            cur.row += dr;
            cur.col += dc;
        }
    }
    return moves;
}

// ─────────────────────────────────────────
//  Pawn Moves
// ─────────────────────────────────────────

std::vector<Move> MoveValidator::getPawnMoves(const Board& board, const Position& pos) {
    std::vector<Move> moves;
    auto piece = board.getPiece(pos);
    if (!piece) return moves;

    Color color = piece->getColor();
    int dir = (color == Color::WHITE) ? -1 : 1;   // white moves up (row-), black moves down (row+)
    int startRow = (color == Color::WHITE) ? 6 : 1;

    // One step forward
    Position oneStep = {pos.row + dir, pos.col};
    if (oneStep.isValid() && board.isEmpty(oneStep)) {
        moves.push_back({pos, oneStep});

        // Two steps from starting row
        Position twoStep = {pos.row + 2 * dir, pos.col};
        if (pos.row == startRow && board.isEmpty(twoStep))
            moves.push_back({pos, twoStep});
    }

    // Diagonal captures
    for (int dc : {-1, 1}) {
        Position cap = {pos.row + dir, pos.col + dc};
        if (cap.isValid() && board.hasEnemy(cap, color))
            moves.push_back({pos, cap});
    }

    return moves;
}

// ─────────────────────────────────────────
//  Rook Moves
// ─────────────────────────────────────────

std::vector<Move> MoveValidator::getRookMoves(const Board& board, const Position& pos) {
    return getSlidingMoves(board, pos, {{-1,0},{1,0},{0,-1},{0,1}});
}

// ─────────────────────────────────────────
//  Bishop Moves
// ─────────────────────────────────────────

std::vector<Move> MoveValidator::getBishopMoves(const Board& board, const Position& pos) {
    return getSlidingMoves(board, pos, {{-1,-1},{-1,1},{1,-1},{1,1}});
}

// ─────────────────────────────────────────
//  Queen Moves (rook + bishop combined)
// ─────────────────────────────────────────

std::vector<Move> MoveValidator::getQueenMoves(const Board& board, const Position& pos) {
    return getSlidingMoves(board, pos, {{-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1}});
}

// ─────────────────────────────────────────
//  Knight Moves
// ─────────────────────────────────────────

std::vector<Move> MoveValidator::getKnightMoves(const Board& board, const Position& pos) {
    std::vector<Move> moves;
    auto piece = board.getPiece(pos);
    if (!piece) return moves;

    std::vector<std::pair<int,int>> jumps = {
        {-2,-1},{-2,1},{-1,-2},{-1,2},
        {1,-2},{1,2},{2,-1},{2,1}
    };

    for (auto& [dr, dc] : jumps) {
        Position dest = {pos.row + dr, pos.col + dc};
        if (dest.isValid() && (board.isEmpty(dest) || board.hasEnemy(dest, piece->getColor())))
            moves.push_back({pos, dest});
    }
    return moves;
}

// ─────────────────────────────────────────
//  King Moves
// ─────────────────────────────────────────

std::vector<Move> MoveValidator::getKingMoves(const Board& board, const Position& pos) {
    std::vector<Move> moves;
    auto piece = board.getPiece(pos);
    if (!piece) return moves;

    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            Position dest = {pos.row + dr, pos.col + dc};
            if (dest.isValid() && (board.isEmpty(dest) || board.hasEnemy(dest, piece->getColor())))
                moves.push_back({pos, dest});
        }
    }
    return moves;
}

// ─────────────────────────────────────────
//  Find King
// ─────────────────────────────────────────

Position MoveValidator::findKing(const Board& board, Color color) {
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++) {
            auto p = board.getPiece(r, c);
            if (p && p->getColor() == color && p->getName() == "King")
                return {r, c};
        }
    return {-1, -1};  // should never happen in a valid game
}

// ─────────────────────────────────────────
//  Is Square Attacked?
// ─────────────────────────────────────────

bool MoveValidator::isSquareAttacked(const Board& board, const Position& pos, Color attackerColor) {
    // Check all squares for attacker pieces and see if they can reach pos
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            auto p = board.getPiece(r, c);
            if (!p || p->getColor() != attackerColor) continue;

            Position from = {r, c};
            std::vector<Move> rawMoves;
            std::string name = p->getName();

            if (name == "Pawn")   rawMoves = getPawnMoves(board, from);
            else if (name == "Rook")   rawMoves = getRookMoves(board, from);
            else if (name == "Bishop") rawMoves = getBishopMoves(board, from);
            else if (name == "Queen")  rawMoves = getQueenMoves(board, from);
            else if (name == "Knight") rawMoves = getKnightMoves(board, from);
            else if (name == "King")   rawMoves = getKingMoves(board, from);

            for (auto& m : rawMoves)
                if (m.to.row == pos.row && m.to.col == pos.col)
                    return true;
        }
    }
    return false;
}

// ─────────────────────────────────────────
//  Is In Check?
// ─────────────────────────────────────────

bool MoveValidator::isInCheck(const Board& board, Color color) {
    Position kingPos = findKing(board, color);
    if (kingPos.row == -1) return false;
    Color enemy = (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
    return isSquareAttacked(board, kingPos, enemy);
}

// ─────────────────────────────────────────
//  Get Legal Moves for one piece
// ─────────────────────────────────────────

std::vector<Move> MoveValidator::getLegalMoves(const Board& board, const Position& pos) {
    std::vector<Move> legal;
    auto piece = board.getPiece(pos);
    if (!piece) return legal;

    Color color = piece->getColor();
    std::string name = piece->getName();
    std::vector<Move> rawMoves;

    if (name == "Pawn")        rawMoves = getPawnMoves(board, pos);
    else if (name == "Rook")   rawMoves = getRookMoves(board, pos);
    else if (name == "Bishop") rawMoves = getBishopMoves(board, pos);
    else if (name == "Queen")  rawMoves = getQueenMoves(board, pos);
    else if (name == "Knight") rawMoves = getKnightMoves(board, pos);
    else if (name == "King")   rawMoves = getKingMoves(board, pos);

    // Filter: only keep moves that don't leave our king in check
    for (auto& move : rawMoves) {
        Board copy = board;  // copy the board
        copy.applyMove(move);
        if (!isInCheck(copy, color))
            legal.push_back(move);
    }
    return legal;
}

// ─────────────────────────────────────────
//  Get All Legal Moves for a color
// ─────────────────────────────────────────

std::vector<Move> MoveValidator::getAllLegalMoves(const Board& board, Color color) {
    std::vector<Move> all;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            auto p = board.getPiece(r, c);
            if (!p || p->getColor() != color) continue;
            auto moves = getLegalMoves(board, {r, c});
            all.insert(all.end(), moves.begin(), moves.end());
        }
    }
    return all;
}

// ─────────────────────────────────────────
//  Is a specific move legal?
// ─────────────────────────────────────────

bool MoveValidator::isMoveLegal(const Board& board, const Move& move, Color color) {
    auto piece = board.getPiece(move.from);
    if (!piece || piece->getColor() != color) return false;

    auto legal = getLegalMoves(board, move.from);
    for (auto& m : legal)
        if (m.from.row == move.from.row && m.from.col == move.from.col &&
            m.to.row == move.to.row && m.to.col == move.to.col)
            return true;
    return false;
}

// ─────────────────────────────────────────
//  Checkmate & Stalemate
// ─────────────────────────────────────────

bool MoveValidator::isCheckmate(const Board& board, Color color) {
    return isInCheck(board, color) && getAllLegalMoves(board, color).empty();
}

bool MoveValidator::isStalemate(const Board& board, Color color) {
    return !isInCheck(board, color) && getAllLegalMoves(board, color).empty();
}