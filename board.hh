#ifndef BOARD_HH
#define BOARD_HH

#include </usr/include/SDL2/SDL.h>
#include </usr/include/SDL2/SDL_image.h>

#include "utils.hh"

class MyApp;

class Board {
public:
    //  Creates a board with the pieces in the initial position. An instance of MyApp is needed to print the board.
    Board(std::shared_ptr<MyApp> a);

    ~Board();

    //  Sets the board to the initial position.
    void setDefaulValues();

    //  Gets the pieceColor of the player that will move next.
    PieceColor getMoveTurn();

    //  Gets the result of the game.
    BoardResult getBoardResult();

    //  Makes a move in the board, updating all bitmaps and variables accordingly.
    void movePiece(PieceMove& move);

    //  Returns all the legal moves for the current player's turn.
    const std::set<PieceMove>& getCurrentLegalMoves(); //FIX: maybe return a const reference

    //  Prints the board through the app passed in the constructor.
    void printBoardApp();

    //  Prints the result of the game.
    void printResult();

private:
    std::shared_ptr<MyApp> app; //The app that will print the board

    PieceColor moveTurn; //The color of the player that will move next
    unsigned int moveCounter; //The number of moves that have been made
    std::set<PieceMove> legalMoves; //The set of legal moves for the current player

    //  Log of the boardState
    std::map<BoardState, int> boardStateLog; //FIX: maybe implement it differently, it causes delays when duplicating the Board object.
    bool threefoldRepetition; //True if the same board state is repeated three times, false otherwise.

    //  Board result
    BoardResult boardResult; //The result of the game, if it is still ongoing, it will be NONE.


    //BITMAPS INFORMATION: The board is represented by bitmaps, each bit represents a square in the board. From the white player's view, the MSB (most significant bit) is a-1, and the LSB is h-8.

    //  All pieces
    uint64_t allPieces;
    uint64_t enPassant; //If a pawn moves two squares forward and the opponent can capture it through en passant, this bitmap will have the information of the square where the pawn can be captured.
    uint64_t castleBitmap; //Has the information of where the king can castle, if the king or the rook moves, the bitmap is updated.
    
    //  (white/black)TargetedSquares: Squares targeted by the opponent's pieces
    //  (white/black)PinnedSquares: Squares that are pinned by the opponent's pieces

    //  White pieces
    uint64_t whitePawn, whiteBishop, whiteKnight, whiteRook, whiteQueen, whiteKing;
    uint64_t whitePieces, whiteTargetedSquares, whitePinnedSquares;
    
    //  Black pieces
    uint64_t blackPawn, blackBishop, blackKnight, blackRook, blackQueen, blackKing;
    uint64_t blackPieces, blackTargetedSquares, blackPinnedSquares;


    //LEGAL MOVES CALCULATION related functions

    //  Updates the legalMoves set with all possible moves for the current player's turn. It also updtes the opponent's targetedSquares and pinnedSquares bitmaps.
    void calculateLegalMoves();

    //  Fills the pieceMoves set with all moves of all pieces, including those that may put the king in check.
    void getAllPiecesMoves(std::set<PieceMove>& pieceMoves);

    //  Adds all legal moves of the piece represented by 'bit' to the pieceMoves set.
    void getPieceMoves(uint64_t& bit, std::set<PieceMove>& pieceMoves);

    //  Updates the targetedSquares and pinned bitmap of the opponent of the color passed as argument.
    void updateTargetedSquares(PieceColor opponentColor);

    //  Updates the targetedSquares and pinned bitmap. Those cells that are targeted by the piece represented by 'bit' are updated.
    void updatePieceTargetedSquares(uint64_t bit);

    //  Updates the enPassant bitmap if the move involves a pawn moving two squares forward.
    void updateEnPassant(PieceMove& move);

    //  Updates the castleBitmap based on the move. If the move disables castling, the bitmap is updated accordingly.
    void updateCastle(PieceMove& move);

    //  Checks if the current player (moveTurn) is in check. If so, eliminates moves that do not free the king from check.
    void manageCheck(std::set<PieceMove> &pieceMoves);

    //  Eliminates moves from pieceMoves that would leave the king in check after the move.
    void eliminatePinnedCheckMoves(std::set<PieceMove> &pieceMoves);

    //  Gets all legal moves of the piece represented by 'bit' and adds them to the pieceLegalMoves set.
    void getWhitePawnMoves(uint64_t bit, std::set<PieceMove>& pieceLegalMoves);
    void getBlackPawnMoves(uint64_t bit, std::set<PieceMove>& pieceLegalMoves);
    void getBishopMoves(uint64_t bit, std::set<PieceMove>& pieceLegalMoves);
    void getKnightMoves(uint64_t bit, std::set<PieceMove>& pieceLegalMoves);
    void getRookMoves(uint64_t bit, std::set<PieceMove>& pieceLegalMoves);
    void getQueenMoves(uint64_t bit, std::set<PieceMove>& pieceLegalMoves);
    void getKingMoves(uint64_t bit, std::set<PieceMove>& pieceLegalMoves);
    void promoteWhitePawn(std::set<PieceMove>& pieceMoves, PieceMove& move);
    void promoteBlackPawn(std::set<PieceMove>& pieceMoves, PieceMove& move);

    //  For the piece in located in the bit, it will update the targetedSquares bitmap of the opponent. Also the pinnedSquares bitmap will be updated.
    void targetedByWhitePawn(uint64_t bit);
    void targetedByBlackPawn(uint64_t bit);
    void targetedByBishop(uint64_t bit);
    void targetedByKnight(uint64_t bit);
    void targetedByRook(uint64_t bit);
    void targetedByQueen(uint64_t bit);
    void targetedByKing(uint64_t bit);


    //MAKING A MOVE related functions
    
    //  Makes the move in the board, updating all bitmaps and variables accordingly.
    void makeAMove(const PieceMove& move);

    //  Removes the bit from the targetBitMap.
    void removePiece(uint64_t& targetBitMap, uint64_t bit);

    //  Adds the bit to the targetBitMap.
    void addPiece(uint64_t& targetBitMap, PieceColor col, uint64_t bit);

    //  Detects if a castle move is being done, if so, it will move the rook.
    void manageCastleMove(uint64_t fromPieceBitmap, const PieceMove& move);

    //  Adds to boardStateLog the current board state.
    void registerState();


    //BITMAPS related functions
    
    //  Given a bit in a uint64_t, it where it is located through (i, j) coordinates
    std::pair<uint16_t, uint16_t> bitToij(uint64_t bit) const;

    //  Convert the (i, j) coordinates to a bit in a uint64_t
    void ijToBit(int i, int j, uint64_t& bit) const;

    //  Returns a pointer to the bitmap of the pieceType. 'nullptr' if the pieceType is NONE.
    uint64_t* pieceTypeToBitmap(PieceType pt);

    //  Returns a pointer to the bitmap of the piece located in the bit.
    uint64_t* bitToPieceBitmap(uint64_t bit);

    //  Returns the pieceType of the piece located in the bit.
    PieceType bitToPieceType(uint64_t bit) const;

    //  Given an 8x8 matrix, it fills it with the pieceType of each cell, if there is no piece, pieceType = NONE.
    //  The returned matrix will be filled from the white player's view. PieceMatrix[0][0] = a-7, PieceMatrix[7][7] = h-0
    void bitBoardToMatrix(PieceMatrix& b) const;


    //EXTRA BOARD INFORMATION BITMAPS

    //  Board information bitmaps, static constexpr because they are constant and will be used in the whole program.
    static constexpr uint64_t A_FILE = 0x8080808080808080;
    static constexpr uint64_t B_FILE = 0x4040404040404040;
    static constexpr uint64_t C_FILE = 0x2020202020202020;
    static constexpr uint64_t D_FILE = 0x1010101010101010;
    static constexpr uint64_t E_FILE = 0x0808080808080808;
    static constexpr uint64_t F_FILE = 0x0404040404040404;
    static constexpr uint64_t G_FILE = 0x0202020202020202;
    static constexpr uint64_t H_FILE = 0x0101010101010101;

    static constexpr uint64_t RANK_1 = 0xff00000000000000;
    static constexpr uint64_t RANK_2 = 0x00ff000000000000;
    static constexpr uint64_t RANK_3 = 0x0000ff0000000000;
    static constexpr uint64_t RANK_4 = 0x000000ff00000000;
    static constexpr uint64_t RANK_5 = 0x00000000ff000000;
    static constexpr uint64_t RANK_6 = 0x0000000000ff0000;
    static constexpr uint64_t RANK_7 = 0x000000000000ff00;
    static constexpr uint64_t RANK_8 = 0x00000000000000ff;
};

#endif

/* BOARD:
  _________________________
  |8|                     |
  |7|                     |
  |6|                     |
  |5|                     |
  |4|                     |
  |3|                     |
  |2|                     |
  |1|_____________________|
  |_a__b__c__d__e__f__g__h|
*/