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
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

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

enum PlayerType { ORDER, CHAOS };

ostream& operator<<(ostream& os, PlayerType type) {
    switch (type) {
    case ORDER:
        return os << "Order";
    case CHAOS:
        return os << "Chaos";
    }
    return os;
}

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

struct Block {
    string text;
    int lines;

    Block() = delete;

    Block(const string& t) {
        text = t;
        lines = count(t.begin(), t.end(), '\n') + 1;
    }
};

class ConsoleRenderer {
  public:
    ConsoleRenderer() : lines(0) {}

    void push(const Block& block) {
        blocks.push_back(block);
        refresh();
    }

    void pop(bool consumed_input = false) {
        if (blocks.empty()) return;
        blocks.pop_back();
        if (consumed_input) ++lines;
        refresh();
    }

  private:
    vector<Block> blocks;
    int lines;

    void refresh() {
        int next_lines = 0;
        for (const Block& block : blocks) {
            next_lines += block.lines;
        }
        clear();
        print();
        lines = next_lines;
    }
    void print() const {
        for (const Block& block : blocks) {
            cout << block.text;
        }
    }

    void clear() {
        if (lines <= 0) return;
        cout << "\033[2K\r";
        for (int i = 1; i < lines; ++i) {
            cout << "\033[1A\033[2K\r";
        }
    }
};

/**
 * @brief Represents the game board and tracks possible winning conditions for both
 * players.
 */
class GameBoard {
  public:
    GameBoard() : GameBoard(0) {}

    /**
     * @brief Constructs an n x n game board with all cells empty.
     * @param n Board size.
     * @pre n >= MIN_SIZE && n <= MAX_SIZE.
     */
    GameBoard(int n) : size(n), board(vector<vector<Cell>>(n, vector<Cell>(n, E))) {}

    /**
     * @brief Prints the board with row and column labels.
     */
    string str() const {
        ostringstream oss;
        int width = column_width() + 1;
        oss << setw(width) << "";
        for (int i = COL_LABEL_START; i <= int(size); ++i) {
            oss << setw(width) << i;
        }
        oss << "\n";
        for (size_t r = 0; r < size; ++r) {
            oss << setw(width) << char(ROW_LABEL_START + r);
            for (size_t c = 0; c < size; ++c) {
                oss << setw(width) << board[r][c];
            }
            oss << "\n";
        }
        return oss.str();
    }
    size_t get_size() const {
        return size;
    }

    char get_row_start() const {
        return ROW_LABEL_START;
    }
    int get_col_start() const {
        return COL_LABEL_START;
    }

    bool check_row_bounds(const char r) const {
        int row_index = r - ROW_LABEL_START;
        if (!(row_index >= 0)) return false;
        if (!(row_index <= size - 1)) return false;
        return true;
    }
    bool check_column_bounds(const int c) const {
        int col_index = c - COL_LABEL_START;
        if (!(col_index >= 0)) return false;
        if (!(col_index <= size - 1)) return false;
        return true;
    }

  private:
    static constexpr char ROW_LABEL_START = 'A';
    static constexpr int COL_LABEL_START = 1;
    /**
     * @brief Represents the possible winning conditions for a player, including rows,
     * columns, and diagonals. Each condition is represented as a boolean indicating
     * whether it is still possible for that player to win in that way.
     */
    struct Wins {
        // TODO: reimplement for different board sizes and refactor
    };
    size_t size;
    vector<vector<Cell>> board;
    Wins o_wins;
    Wins x_wins;

    int column_width() const {
        return to_string(size).length();
    }
};

/**
 * @brief Represents a player in the game, which can be either a human or a computer.
 * Each player has a name and a type (ORDER or CHAOS).
 */
class Player {
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
    virtual Move get_move(
        const GameBoard& game_board, //
        ConsoleRenderer& console
    ) const = 0;

  private:
    const string name;
    /**
     * @brief Identifies whether a player is trying to create order or chaos.
     */
    const PlayerType type;
};

/**
 * @brief Represents a human player in the game.
 */
class Human : public Player {
  public:
    Human() : Human("Player", ORDER) {}
    Human(const string& name, PlayerType type) : Player(name, type) {}

    // Returns a move with x, y
    Move get_move(const GameBoard& game_board, ConsoleRenderer& console) const override {
        Move move(0, 0, E);
        set_coords(move, game_board, console);
        set_symbol(move, game_board, console);
        return move;
    }

  private:
    void
    set_coords(Move& move, const GameBoard& game_board, ConsoleRenderer& console) const {
        string row_range =
            to_string(game_board.get_row_start()) + " to " +
            to_string(game_board.get_row_start() + game_board.get_size() - 1);
        string col_range =
            to_string(game_board.get_col_start()) + " to " +
            to_string(game_board.get_col_start() + game_board.get_size() - 1);

        console.push(Block(
            "\nEnter a coordinate in the format of [row] [col] from " + //
            row_range + " and " + col_range + ": "
        ));

        char row;
        int col;
        while (true) {
            cin >> row >> col;

            cin.ignore(1000, '\n');
            console.pop(true);

            if (!game_board.check_row_bounds(row)) {
                console.push(Block(
                    "\n" + to_string(row) +
                    " is not a valid row. Please enter a row from " + row_range + ": "
                ));
                continue;
            }
            if (!game_board.check_column_bounds(col)) {
                console.push(Block(
                    "\n" + to_string(col) +
                    " is not a valid column. Please enter a column from " + col_range +
                    ": "
                ));
                continue;
            }
            break;
        }
        move.row = row - game_board.get_row_start();
        move.col = col - game_board.get_col_start();
    }

    void set_symbol(
        Move& move,                  //
        const GameBoard& game_board, //
        ConsoleRenderer& console
    ) const {
        char symbol;
        Cell cell;

        console.push(Block("\nEnter a symbol (x or o): "));

        while (true) {
            cin >> symbol;
            symbol = to_lower(symbol);

            cin.ignore(1000, '\n');
            console.pop(true);

            if (symbol != 'o' && symbol != 'x') {
                console.push(Block(
                    "\n" + to_string(symbol) + " is not a valid symbol. Enter x or o:"
                ));
                continue;
            }

            cell = (symbol == 'x') ? X : O;
            move.symbol = cell;
            break;
        }
    }
};

/**
 * @brief Represents a computer player in the game.
 */
class Computer : public Player {
  public:
    Computer() : Computer("Computer", CHAOS) {}
    Computer(const string& name, PlayerType type) : Player(name, type) {}

    Move get_move(const GameBoard& game_board, ConsoleRenderer& console) const override {}

  private:
};

/**
 * @brief Represents the main game logic, including the game loop, and
 * player turns.
 */
class Game {
  public:
    Game() {}
    /**
     * @brief Runs full game sessions until the players choose to
     * stop.
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
        introduction();
        setup_board();
    }

    /**
     * @brief Executes one turn for the active player.
     * @param player The player taking this turn.
     */
    void turn(Player& player) {
        Move move = player.get_move(game_board, console);
    }

    /**
     * @brief Handles end-of-game logic.
     * @return true to start another game, false to stop.
     */
    bool end() {}

  private:
    GameBoard game_board;
    Player* player1;
    Player* player2;
    Human human;
    Computer computer;
    ConsoleRenderer console;

    void introduction() {
        console.push(Block( //
            "\nWelcome to Order and Chaos!"
            "\nIn this game, two players take turns placing Os and Xs onto the board."
            "\nEach turn, both players can choose whether to place an O or and X."
            "\nOrder wins if they can place 5 Xs or Os in a row. Chaos wins if they can "
            "prevent this."
            "\nPress enter to begin:"
        ));
        cin.get();
        console.pop(true);
    }

    void setup_board() {
        console.push(Block("\nEnter a board size from 6 to 9: "));
        string input;
        int size;
        while (true) {
            getline(cin, input);
            console.pop(true);

            try {
                size = stoi(input);
            } catch (...) {
                console.push(Block(
                    "\n" + input +
                    " is not an integer. Please enter an integer from 6 to 9: "
                ));
                continue;
            }

            if (!(size >= 6 && size <= 9)) {
                console.push(Block(
                    "\n" + to_string(size) +
                    " is not a valid board size. Please enter one from 6 to 9: "
                ));
                continue;
            }
            break;
        }
        game_board = GameBoard(size);
    }

    void setup_players() {
        int num = rand() % 2;
        if (num == 0) {
            player1 = &human;
            player2 = &computer;
        } else {
            player1 = &computer;
            player2 = &human;
        }
        // TODO: setup order and chaos
    }
};

int main() {
    srand(time(nullptr));
    Game game;
    game.play();
} // main
