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
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/**
 * @brief Helper validation utilities for board size and index bounds.
 */
struct Validate {
    inline static size_t vector_size;

    /**
     * @brief Validates and converts a board size.
     * @param n Proposed board size.
     * @return n as size_t.
     * @pre n >= 1.
     */
    static size_t size(int n) {
        assert(n >= 1);
        return n;
    }

    /**
     * @brief Validates and converts a board index.
     * @param n Proposed row or column index.
     * @return n as size_t.
     * @pre 0 <= n < vector_size.
     */
    static size_t index(int n) {
        assert(n >= 0 && n < vector_size);
        return n;
    }
};

/**
 * @brief Represents the state of a cell on the game board.
 */
enum Cell {
    E, ///< Empty cell
    O, ///< Cell occupied by player O
    X  ///< Cell occupied by player X
};

/**
 * @brief Prints a cell symbol as '.', 'O', or 'X'.
 * @param os Output stream.
 * @param c Cell value to print.
 * @return The output stream.
 */
ostream& operator<<(ostream& os, Cell c) {
    switch (c) {
    case E:
        return os << ".";
    case O:
        return os << "O";
    case X:
        return os << "X";
    }
    return os;
}

/**
 * @brief Represents a move made by a player, including the row, column, and symbol
 * placed.
 */
struct Move {
    inline static size_t size;
    size_t row;
    size_t col;
    Cell symbol;

    /**
     * @brief Constructs a Move with the given row, column, and symbol.
     * @param r The row index of the move.
     * @param c The column index of the move.
     * @param s The symbol to place (O or X).
     * @pre r >= 0 && r < size
     * @pre c >= 0 && c < size.
     * @pre s must be either O or X.
     */
    Move(int r, int c, Cell s)
        : row(Validate::index(r)), //
          col(Validate::index(c)), //
          symbol(s) {
        assert(s == O || s == X);
    }
};

/**
 * @brief Represents the game board and tracks possible winning conditions for both
 * players.
 */
class GameBoard {
  public:
    GameBoard() = delete;

    /**
     * @brief Constructs an n x n game board with all cells empty.
     * @param n Board size.
     * @pre n >= 1.
     */
    GameBoard(int n)
        : size(Validate::size(n)),                            //
          board(vector<vector<Cell>>(n, vector<Cell>(n, E))), //
          o_wins(n),                                          //
          x_wins(n) {
        Validate::vector_size = n;
    }

    /**
     * @brief Prints the board with row and column labels.
     */
    void print() const {
        int width = to_string(size).length() + 1;
        cout << setw(width) << "";
        for (int i = col_label_start; i <= size; ++i) {
            cout << setw(width) << i;
        }
        cout << "\n";
        for (size_t r = 0; r < size; ++r) {
            cout << setw(width) << char(row_label_start + r);
            for (size_t c = 0; c < size; ++c) {
                cout << setw(width) << board[r][c];
            }
            cout << "\n";
        }
    }

  private:
    /**
     * @brief Represents the possible winning conditions for a player, including rows,
     * columns, and diagonals. Each condition is represented as a boolean indicating
     * whether it is still possible for that player to win in that way.
     */
    struct Wins {
        vector<bool> rows;
        vector<bool> cols;
        bool diag1;
        bool diag2;

        /**
         * @brief Constructs Wins with the given board size, initializing all winning
         * conditions to true (indicating that they are all possible at the start of the
         * game).
         */
        Wins() = delete;
        Wins(const int n)
            : rows(Validate::size(n), true), //
              cols(Validate::size(n), true), //
              diag1(true), diag2(true) {}

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

    const size_t size;
    vector<vector<Cell>> board;
    const char row_label_start = 'A';
    const int col_label_start = 1;
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

    /**
     * @brief Identifies whether a player is trying to create order or chaos.
     */
    enum PlayerType { ORDER, CHAOS };
    const PlayerType type;

  public:
    /**
     * @brief Constructs a player with a name and role.
     * @param name Player display name.
     * @param type Player role (ORDER or CHAOS).
     */
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
    /**
     * @brief Runs full game sessions until the players choose to stop.
     */
    void play() {
        bool repeat = true;
        while (repeat) {
            start();
            // ... game loop
            repeat = end();
        }
    }

    /**
     * @brief Initializes the game and prints player instructions.
     */
    void start() {
        string instructions =
            "Welcome to Order and Chaos!\n"
            "In this game, two players take turns placing Os and Xs onto the board."
            "Each turn, both players can choose whether to place an O or and X.\n"
            "Order wins if they can place 5 Xs or Os in a row. Chaos wins if they can "
            "prevent this.";
        cout << instructions;
    }

    /**
     * @brief Executes one turn for the active player.
     * @param player The player taking this turn.
     */
    void turn(Player& player) {
        Move move = player.get_move(game_board);
    }

    /**
     * @brief Handles end-of-game logic.
     * @return true to start another game, false to stop.
     */
    bool end() {}

  private:
    /**
     * @brief Represents the current state of the game.
     */
    enum GameState {};

    GameBoard game_board;
    GameState game_state;
    Player* player1;
    Player* player2;
};

int main() {} // main
