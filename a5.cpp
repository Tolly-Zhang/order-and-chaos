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
 */
enum Cell {
    E, ///< Empty cell
    O, ///< Cell occupied by player O
    X  ///< Cell occupied by player X
};

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
    Move(size_t r, size_t c, Cell s)
        : row(max(r, size_t(0))), //
          col(max(c, size_t(0))), //
          symbol(s) {
        assert(r >= 0 && c >= 0);
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

        /**
         * @brief Constructs Wins with the given board size, initializing all winning
         * conditions to true (indicating that they are all possible at the start of the
         * game).
         */
        Wins(const size_t n)
            : rows(max(n, size_t(1)), true), //
              cols(max(n, size_t(1)), true) {
            assert(n > 0);
        }

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

    vector<vector<Cell>> board{};
    Wins o_wins;
    Wins x_wins;
};

/**
 * @brief Represents a player in the game, which can be either a human or a computer.
 * Each player has a name and a type (ORDER or CHAOS).
 */
class Player {
  private:
    const string name;
    enum PlayerType { ORDER, CHAOS };
    const PlayerType type;

  public:
    Player(const string& name, PlayerType type) : name(name), type(type) {}

    /**
     * @brief Gets the player's move based on the current game board state. This is a pure
     * virtual function that must be implemented by derived classes (Human and Computer).
     * @param game_board The current game board.
     * @return The move chosen by the player.
     */
    virtual Move get_move(GameBoard game_board) const = 0;
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

    void turn(Player& player) {
        Move move = player.get_move(game_board);
    }

    bool end() {}

  private:
    enum GameState {};

    GameBoard game_board;
    GameState game_state;
    Player* player1;
    Player* player2;
};

int main() {} // main
