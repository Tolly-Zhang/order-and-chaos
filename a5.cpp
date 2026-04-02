// a5.cpp

/////////////////////////////////////////////////////////////////////////
//
// Student Info
// ------------
//
// Name : <put your full name here!>
// St.# : <put your full SFU student number here>
// Email: <put your SFU email address here>
//
//
// Statement of Originality
// ------------------------
//
// All the code and comments below are my own original work. For any non-
// original work, I have provided citations in the comments with enough
// detail so that someone can see the exact source and extent of the
// borrowed work.
//
// In addition, I have not shared this work with anyone else, and I have
// not seen solutions from other students, tutors, websites, books,
// etc.
//
/////////////////////////////////////////////////////////////////////////

//
// You can use any other C++17 standard #includes that you need. Do not use any
// other libraries or substantial pieces of code from the web or other sources.
// We want to see your own work.
//
#include <array>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;
/**
 * @brief Represents the state of a cell on the game board.
 * E: Empty cell
 * O: Cell occupied by player O
 * X: Cell occupied by player X
 */
enum Cell { E, O, X };

/**
 * @brief Represents a move made by a player, including the row, column, and symbol
 * placed.
 */
struct Move {
    size_t row;
    size_t col;
    Cell symbol;

    /**
     * @brief Constructs a Move with the given row, column, and symbol.
     * @param r The row index of the move.
     * @param c The column index of the move.
     * @param s The symbol to place (O or X).
     * @pre r > = 0 && c >= 0
     * @pre s must be either O or X.
     */
    Move(size_t r, size_t c, Cell s) : row(r), col(c), symbol(s) {
        assert(s == O || s == X);
    }
};

/**
 * @brief Represents the game board and tracks possible winning conditions for both
 * players.
 */
class GameBoard {
  public:
  private:
    /**
     * @brief Represents the possible winning conditions for a player, including rows,
     * columns, and diagonals. Each condition is represented as a boolean indicating
     * whether it is still possible for that player to win in that way.
     */
    struct Wins {
        vector<bool> rows;
        vector<bool> cols;
        bool diag1 = false;
        bool diag2 = false;

        Wins(const size_t n) : rows(n, true), cols(n, true) {}

        /**
         * @brief Checks if there are any winning conditions met for this symbol.
         * @return true if there is a win, false otherwise.
         */
        bool has_wins() const {
            for (bool win : rows) {
                if (win) return true;
            }
            for (bool win : cols) {
                if (win) return true;
            }
            return diag1 || diag2;
        }
    };

    /**
     * @brief Represents the possible winning conditions for both players (O and X).
     */
    struct SymbolWins {
        Wins o_wins;
        Wins x_wins;

        /**
         * @brief Constructs SymbolWins with the given board size.
         * @param n The size of the board (number of rows/columns).
         * @pre n > 0
         */
        SymbolWins(const size_t n) : o_wins(n), x_wins(n) {}
    };

    vector<vector<Cell>> board{};
    Wins possible_wins;
};

/**
 * @brief Represents a player in the game, which can be either a human or a computer.
 * Each player has a name and a type (ORDER or CHAOS).
 */
class Player {
  private:
    enum PlayerType { ORDER, CHAOS };
    const string name;
    const PlayerType type;

  public:
    Player(const string& name, PlayerType type) : name(name), type(type) {}
};

/**
 * @brief Represents a human player in the game.
 */
class Human : public Player {
  public:
  private:
};

/**
 * @brief Represents a computer player in the game.
 */
class Computer : public Player {
  public:
  private:
};

/**
 * @brief Represents the main game logic, including the game loop, and player turns.
 */
class Game {
  public:
    void play() {
        bool repeat = true;
        while (repeat) {
            start();
            // ... game loop
            repeat = end();
        }
    }
    void start() {
        string instructions = R"(
            Welcome to Order and Chaos!
            In this game, two players take turns placing Os and Xs onto the board.
            Each turn, both players can choose whether to place an O or and X.
            Order wins if they can place 5 Xs or Os in a row. Chaos wins if they can prevent this.
        )";
        cout << instructions;
    }

    void turn(Player& player) {}

    Move get_move(Player& player) {}

    bool end() {}

  private:
    enum GameState {};

    GameBoard game_board;
    GameState game_state;
    Player* player1;
    Player* player2;
};

int main() {} // main
