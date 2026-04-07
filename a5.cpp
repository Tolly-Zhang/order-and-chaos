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
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Returns true if the character is a digit
bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Returns uppercase characters to lower case, otherwise returns character
char to_lower(char c) {
    if ('A' <= c && c <= 'Z') {
        return c + 32;
    }
    return c;
}

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
        : row(r), //
          col(c), //
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
     * @pre n >= MIN_SIZE && n <= MAX_SIZE.
     */
    GameBoard(int n)
        : size(n),                                            //
          board(vector<vector<Cell>>(n, vector<Cell>(n, E))), //
          o_wins(n),                                          //
          x_wins(n) {}

    /**
     * @brief Prints the board with row and column labels.
     */
    string print() const {
        ostringstream oss;
        int width = to_string(size).length() + 1;
        oss << setw(width) << "";
        for (int i = col_label_start; i <= static_cast<int>(size); ++i) {
            oss << setw(width) << i;
        }
        oss << "\n";
        for (size_t r = 0; r < size; ++r) {
            oss << setw(width) << char(row_label_start + r);
            for (size_t c = 0; c < size; ++c) {
                oss << setw(width) << board[r][c];
            }
            oss << "\n";
        }
        return oss.str();
    }

    int get_size() {
        return to_string(size).length() + 1;
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
            : rows(n, true), //
              cols(n, true), //
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
  public:
    enum PlayerType { ORDER, CHAOS };

  private:
    const string name;

    /**
     * @brief Identifies whether a player is trying to create order or chaos.
     */
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
    virtual Move get_move(GameBoard& game_board) const = 0;
};

/**
 * @brief Represents a human player in the game.
 */
class Human : public Player {
  public:
    Human(const string& name, Player::PlayerType type) : Player(name, type) {}

    // Returns a move with x, y
    Move get_move(GameBoard game_board) const {
        char symbol;
        int row = -1;
        int column = -1;
        bool valid = false;

        int gameSize = game_board.get_size();
        Cell cellSymbol;

        while (!valid) {
            cout << "Enter a symbol (x or o): ";
            cin >> symbol;
            symbol = to_lower(symbol);

            if (symbol == 'x') {
                cellSymbol = X;
            } else if (symbol == 'o') {
                cellSymbol = O;
            } else {
                cout << "Invalid symbol! Try again." << endl;
                cin.ignore(1000, '\n');
                continue;
            }

            cin.ignore(1000, '\n');

            cout << "Enter a coordinate: ";

            char inputChar;
            int foundCoords = 0;
            while (cin.get(inputChar) && inputChar != '\n') {

                if (is_alpha(inputChar)) {
                    char lowerChar = to_lower(inputChar);
                    column = lowerChar - 'a';
                    foundCoords++;
                } else if (is_digit(inputChar)) {
                    row = (inputChar - '0') - 1;
                    foundCoords++;
                }
            }

            // please add a way to check if its within game_board size later
            if (foundCoords == 2 && row > -1 && column > -1 && row < gameSize &&
                column < gameSize) {
                valid = true;
            } else {
                cout << "Invalid coordinates, please format (x,y)" << endl;
                cin.ignore(1000, '\n');
                continue;
                ;
            }
        }

        return Move(row, column, cellSymbol);
    }

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
        cout << instructions << endl;

        string orderName;
        string chaosName;

        cout << "What is Order's name: ";
        getline(cin, orderName);

        char opSelect;
        cout << "Player vs Player? (y,n)";
        cin >> opSelect;
        cin.ignore(1000, '\n');
        if (opSelect == 'y') {
            cout << "What is Chaos's name: ";
            getline(cin, chaosName);
        }
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
