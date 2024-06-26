#ifndef PLAYER_HH
#define PLAYER_HH

#include "utils.hh"

class MyApp;
class Board;

class Player {
public:
    Player() {}
    virtual ~Player() {}

    virtual bool canMove() = 0;
    virtual PieceMove getMove() = 0;

protected:
    std::shared_ptr<MyApp> app;
    std::shared_ptr<Board> board;
};

class HumanPlayer : public Player {
public:
    HumanPlayer(std::shared_ptr<MyApp> myApp);
    ~HumanPlayer() override {}

    bool canMove() override;
    PieceMove getMove() override;
};

class EngineRandom : public Player {
public:
    EngineRandom(std::shared_ptr<Board> myBoard) {
        board = myBoard;
    }
    ~EngineRandom() override {}

    bool canMove() override {
        return true;
    }
    PieceMove getMove() {
        std::set<PieceMove> s = board->getCurrentLegalMoves();
        int random = rand() % s.size();
        auto it = s.begin();
        std::advance(it, random);
        return *it;
    }
};

class TranspositionTable {
public:
    struct transTableEntry {
        uint64_t zobristHash;
        uint32_t score; //Maybe uint16_t
        uint8_t depth;
        uint8_t nodeType;
    };

    static constexpr uint8_t NODE_TYPE_EXACT = 0;
    static constexpr uint8_t NODE_TYPE_LOWERBOUND = 1;
    static constexpr uint8_t NODE_TYPE_UPPERBOUND = 2;
    static constexpr uint8_t NODE_TYPE_QUIESCENCE = 3;


    bool contains(uint64_t zobristHash) {
        return transpositionTableBuffer[zobristHash % TRANSPOSITION_TABLE_MASK].zobristHash == zobristHash;
    }

    void insert(uint64_t zobristHash, uint32_t score, uint8_t depth, uint8_t node) {
        transpositionTableBuffer[zobristHash % TRANSPOSITION_TABLE_MASK] = {zobristHash, score, depth, node};
    }

    const transTableEntry* getEntry(uint64_t zobristHash) {
        return &transpositionTableBuffer[zobristHash % TRANSPOSITION_TABLE_MASK];
    }

private:
    static constexpr int TRANSPOSITION_TABLE_SIZE = 1 << 20;
    static constexpr int TRANSPOSITION_TABLE_MASK = TRANSPOSITION_TABLE_SIZE - 1;
    transTableEntry transpositionTableBuffer[TRANSPOSITION_TABLE_SIZE];
};

class EngineV1 : public Player {
public:
    EngineV1(std::shared_ptr<Board> myBoard, std::chrono::milliseconds timeSpan);
    ~EngineV1() override {}

    bool canMove() override;
    PieceMove getMove() override;

private: 
    TranspositionTable transpositionTable;

    //Recursive function that searches for the best move. Depth is the current depth of the search, alfa and beta are the bounds of the search.
    //  Negamax algorithm with alpha-beta pruning. For more information, visit:
    //      Negamax: [https://www.chessprogramming.org/Negamax]
    //      Alpha Beta Pruning: [https://www.chessprogramming.org/Alpha-Beta]
    int search(int depth, int alfa, int beta);

    int quiescenceSearch(int alfa, int beta);

    void orderMoves(const std::set<PieceMove>& moves, std::list<PieceMove>& orderedMoves);

    int evaluate();

    int countMaterial(PieceColor myColor);

    static constexpr int MAX_DEPTH = 4;
    static constexpr int INF = INT_MAX;

    static constexpr int PAWN_VALUE = 100;
    static constexpr int KNIGHT_VALUE = 320;
    static constexpr int BISHOP_VALUE = 330;
    static constexpr int ROOK_VALUE = 500;
    static constexpr int QUEEN_VALUE = 900;
    static constexpr int KING_VALUE = 20000;

    std::chrono::milliseconds moveDelay;
};

#endif