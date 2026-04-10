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
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

/**
 * @brief Converts an uppercase ASCII letter to lowercase.
 * @param c Character to convert.
 * @return Lowercase version of c when c is between 'A' and 'Z'; otherwise c.
 * @pre c is any valid char value.
 * @post Return value is unchanged for non-uppercase ASCII input.
 */
char to_lower(char c) {
    if ('A' <= c && c <= 'Z') {
        return c + 32;
    }
    return c;
}

bool is_alpha(char c) {
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

string to_string(char c) {
    return string(1, c);
}

/**
 * @brief Represents the state of a cell on the game board.
 */
enum Cell {
    E, ///< Empty cell
    O, ///< Cell occupied by player O
    X  ///< Cell occupied by player X
};

string to_string(Cell c) {
    switch (c) {
    case E:
        return ".";
    case O:
        return "O";
    case X:
        return "X";
    }
    return "";
}

/**
 * @brief Represents the role a player has in the game.
 */
enum PlayerType { ORDER, CHAOS };

string to_string(PlayerType type) {
    switch (type) {
    case ORDER:
        return "Order";
    case CHAOS:
        return "Chaos";
    }
    return "";
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
     * @pre r and c are valid zero-based board indices in the caller's context.
     * @pre s is either O or X.
     * @post row == r, col == c, and symbol == s.
     */
    Move(int r, int c, Cell s)
        : row(r), //
          col(c), //
          symbol(s) {
        assert(s == O || s == X);
    }
};

/**
 * @brief Console text block with a cached line count for redraw logic.
 */
struct Block {
    string text;
    int lines;

    Block() = delete;

    /**
     * @brief Builds a block from text and counts its display lines.
     * @param t Text to display.
     * @pre t may be empty.
     * @post text == t and lines == number of '\n' in t + 1.
     */
    Block(const string& t) {
        text = t;
        lines = count(t.begin(), t.end(), '\n');
    }
};

class Console {
  public:
    vector<Block> blocks;
    int lines;
    /**
     * @brief Creates an empty renderer state.
     * @post No blocks are stored and lines == 0.
     */
    Console() : lines(0) {}

    size_t size() const {
        return blocks.size();
    }

    /**
     * @brief Adds a block to the render stack and redraws.
     * @param block Block to append.
     * @pre block.lines >= 1.
     * @post block is the newest rendered block.
     */
    void push(const Block& block) {
        blocks.push_back(block);
        lines += block.lines;
        cout << block.text << flush;
    }

    void push(const string& text) {
        push(Block(text));
    }

    void overwrite(const size_t i, const string& text) {
        if (i >= blocks.size()) return;
        lines -= blocks[i].lines;
        blocks[i] = Block(text);
        lines += blocks[i].lines;
        refresh();
    }

    /**
     * @brief Removes the newest block and redraws.
     * @param extra_lines Number of additional input lines to clear.
     * @pre None.
     * @post If a block existed, the most recent one is removed.
     */
    void pop() {
        pop(0);
    }

    stringstream read(const string& prompt) {
        push(prompt);

        string input;
        getline(cin, input);
        pop_prompt();

        stringstream ss(input);
        return ss;
    }

    void pop_prompt() {
        pop(1);
    }

    /**
     * @brief Recomputes total line count and redraws all blocks.
     * @pre Internal block list is valid.
     * @post lines equals total rendered block lines.
     */
    void refresh() {
        erase(lines);
        lines = 0;
        for (const Block& b : blocks)
            lines += b.lines;
        print();
    }

    /**
     * @brief Prints all stacked blocks in order.
     * @pre Output stream is writable.
     * @post Visible console output matches stacked blocks.
     */
    void print() const {
        for (const Block& block : blocks) {
            cout << block.text;
        }
    }

    void erase() {
        erase_line();
        move_cursor_up();
        move_cursor_end();
    }

    void erase(int n) {
        for (int i = 0; i < n; i++) {
            erase();
        }
    }

  private:
    void pop(int extra_lines) {
        if (blocks.empty()) return;
        erase(blocks.back().lines + extra_lines);
        lines -= blocks.back().lines;
        blocks.pop_back();
    }

    void erase_line() {
        cout << "\x1B[2K";
    }

    void move_cursor_up() {
        cout << "\x1B[1A";
    }

    void move_cursor_front() {
        cout << "\r";
    }

    void move_cursor_end() {
        cout << "\x1B[9999C";
    }
};

/**
 * @brief Represents the game board and tracks possible winning conditions for both
 * players.
 */
class GameBoard {
  public:
    /**
     * @brief Constructs an empty 0x0 board placeholder.
     * @post get_size() == 0.
     */
    GameBoard() : GameBoard(0) {}

    /**
     * @brief Constructs an n x n game board with all cells empty.
     * @param n Board size.
     * @pre n >= 0.
     * @post get_size() == n and every board cell is E.
     */
    GameBoard(int n)
        : size(n),                                            //
          board(vector<vector<Cell>>(n, vector<Cell>(n, E))), //
          x_wins(0),                                          //
          o_wins(0) {}

    /**
     * @brief Returns the board dimension.
     * @return Number of rows/columns in the board.
     * @pre None.
     */
    size_t get_size() const {
        return size;
    }

    /**
     * @brief Returns the first valid row label.
     * @return Starting row label character.
     * @pre None.
     */
    char get_row_start_label() const {
        return ROW_LABEL_START;
    }

    /**
     * @brief Returns the first valid column label.
     * @return Starting column number.
     * @pre None.
     */
    int get_col_start_label() const {
        return COL_LABEL_START;
    }

    /**
     * @brief Checks whether a row label is within board bounds.
     * @param r Row label character to validate.
     * @return true if r maps to a valid board row; otherwise false.
     * @pre r is any char value.
     */
    bool check_row_bounds(const char r) const {
        int row_index = r - ROW_LABEL_START;
        if (!(row_index >= 0)) return false;
        if (!(row_index <= size - 1)) return false;
        return true;
    }

    /**
     * @brief Checks whether a column label is within board bounds.
     * @param c Column label number to validate.
     * @return true if c maps to a valid board column; otherwise false.
     * @pre c is any integer value.
     */
    bool check_column_bounds(const int c) const {
        int col_index = c - COL_LABEL_START;
        if (!(col_index >= 0)) return false;
        if (!(col_index <= size - 1)) return false;
        return true;
    }

    size_t row(const char r) const {
        return r - ROW_LABEL_START;
    }
    size_t col(const int c) const {
        return c - COL_LABEL_START;
    }

    /**
     * @brief Returns a move given a row and column
     * @param row index, represents row of the board starting at top.
     * @param column index, represents column of the board starting at left.
     * @return Cell, either E, O or X.
     * @pre row and column are within bounds.
     */
    Cell at(size_t row, size_t column) const {
        return board[row][column];
    }

    void place(const Move& move) {
        board[move.row][move.col] = move.symbol;
    }

    bool is_empty(size_t r, size_t c) const {
        return at(r, c) == E;
    }

  private:
    const char ROW_LABEL_START = 'a';
    const int COL_LABEL_START = 1;
    /**
     * @brief Represents the possible winning conditions for a player, including rows,
     * columns, and diagonals. Each condition is represented as a boolean indicating
     * whether it is still possible for that player to win in that way.
     */
    struct Wins {
        int wins;
        vector<vector<bool>> rows;
        vector<vector<bool>> cols;
        vector<vector<bool>> left_diag;
        vector<vector<bool>> right_diag;

        /**
         * @brief Initializes all possible win paths as available.
         * @param n Board dimension.
         * @pre n >= 0.
         * @post wins == 4 * n * n and every path flag is true.
         */
        Wins(size_t n)
            : wins(4 * n * n),                     //
              rows(n, vector<bool>(n, true)),      //
              cols(n, vector<bool>(n, true)),      //
              left_diag(n, vector<bool>(n, true)), //
              right_diag(n, vector<bool>(n, true)) {}
    };
    size_t size;
    vector<vector<Cell>> board;
    Wins o_wins;
    Wins x_wins;
};

string to_string(const GameBoard* board) {
    auto append_aligned = [](std::string& text, const std::string& str, const int width) {
        int padding = width - int(str.length());
        if (padding > 0) {
            text += std::string(padding, ' ');
        }
        text += str;
    };

    string output = "";
    int width = to_string(board->get_size()).length() + 1;
    int row_label = board->get_row_start_label();
    int col_label = board->get_col_start_label();
    int size = int(board->get_size());

    output += string(width, ' ');

    for (int i = 0; i < size; ++i) {
        append_aligned(output, to_string(char(col_label + i)), width);
    }

    for (size_t r = 0; r < size; ++r) {
        output += "\n";
        append_aligned(output, to_string(char(row_label + r)), width);
        for (size_t c = 0; c < size; ++c) {
            append_aligned(output, to_string(board->at(r, c)), width);
        }
    }

    return output;
}

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
     * @pre type is ORDER or CHAOS.
     * @post Player stores the provided name and role.
     */
    Player(PlayerType type) : type(type) {}

    PlayerType get_type() const {
        return type;
    }

    void set_type(PlayerType t) {
        type = t;
    }

    /**
     * @brief Gets the player's move based on the current game board state. This is a pure
     * virtual function that must be implemented by derived classes (Human and Computer).
     * @param game_board The current game board.
     * @param console Console renderer used for prompts and updates.
     * @return The move chosen by the player.
     * @pre Implementations must return a valid move.
     */
    virtual Move get_move(
        const GameBoard* game_board, //
        Console& console
    ) const = 0;

  private:
    /**
     * @brief Identifies whether a player is trying to create order or chaos.
     */
    PlayerType type;
};

/**
 * @brief Represents a human player in the game.
 */
class Human : public Player {
  public:
    /**
     * @brief Constructs a default human player named "Player" as ORDER.
     * @post Human player is initialized with default identity.
     */
    Human() = delete;

    /**
     * @brief Constructs a human player with explicit identity.
     * @param name Display name for the player.
     * @param type Role for this player.
     * @pre type is ORDER or CHAOS.
     */
    Human(PlayerType type) : Player(type) {}

    /**
     * @brief Collects a full move from console input.
     * @param game_board Current board used to validate input.
     * @param console Renderer for interactive prompts.
     * @return A validated move with row, column, and symbol.
     * @pre game_board has valid coordinate bounds.
     */
    Move get_move(const GameBoard* game_board, Console& console) const override {
        Move move(0, 0, X);
        set_coords(move, game_board, console);
        set_symbol(move, console);
        return move;
    }

  private:
    /**
     * @brief Prompts until the user enters valid board coordinates.
     * @param move Move object to update.
     * @param game_board Board used for bounds validation.
     * @param console Renderer for prompts and validation feedback.
     * @pre move is writable.
     * @post move.row and move.col contain valid zero-based indices.
     */
    void set_coords(
        Move& move,                  //
        const GameBoard* game_board, //
        Console& console             //
    ) const {
        char row_start = game_board->get_row_start_label();
        char row_end = char(row_start + game_board->get_size() - 1);
        int col_start = game_board->get_col_start_label();
        int col_end = col_start + game_board->get_size() - 1;

        string row_range = to_string(row_start) + " to " + to_string(row_end);
        string col_range = to_string(col_start) + " to " + to_string(col_end);

        char row_input;
        int col_input;
        size_t row;
        size_t col;
        string question = "Please enter a row from " + row_range + //
                          " and a column from " + col_range + ": ";
        string prompt = "\n" + question;
        while (true) {
            stringstream input = console.read(prompt);
            input >> row_input >> col_input;

            if (input.fail()) {
                prompt =
                    "\n\"" + input.str() + "\" is not a valid coordinate. " + question;
                continue;
            }

            string prompt_invalid_row =
                "\n\"" + to_string(row_input) + "\" is not a valid row. " + question;
            string prompt_invalid_col =
                "\n\"" + to_string(col_input) + "\" is not a valid column. " + question;

            if (!(is_alpha(row_input))) {
                prompt = prompt_invalid_row;
                continue;
            }

            row_input = to_lower(row_input);

            if (!game_board->check_row_bounds(row_input)) {
                prompt = prompt_invalid_row;
                continue;
            }

            if (!game_board->check_column_bounds(col_input)) {
                prompt = prompt_invalid_col;
                continue;
            }

            row = game_board->row(row_input);
            col = game_board->col(col_input);

            if (!game_board->is_empty(row, col)) {
                Cell cell = game_board->at(row, col);
                prompt = "\n(" + to_string(row_input) + ", " + to_string(col_input) +
                         ") is already taken (" + to_string(cell) +
                         "). Please enter an empty coordinate: ";
                continue;
            }
            break;
        }
        move.row = row;
        move.col = col;
    }

    /**
     * @brief Prompts until the user enters a valid symbol (x or o).
     * @param move Move object to update.
     * @param game_board Current board context (reserved for future checks).
     * @param console Renderer for prompts and validation feedback.
     * @pre move is writable.
     * @post move.symbol is set to X or O.
     */
    void set_symbol(
        Move& move,      //
        Console& console //
    ) const {
        char c;
        Cell cell;
        string question = "Please enter a symbol (x or o): ";
        string prompt = "\n" + question;
        while (true) {
            stringstream input = console.read(prompt);
            input >> c;

            string prompt_invalid_symbol =
                "\n\"" + input.str() + "\" is not a valid symbol. " + question;

            if (input.fail()) {
                prompt = prompt_invalid_symbol;
                continue;
            }

            c = to_lower(c);

            if (!(c == 'o' || c == 'x')) {
                prompt = prompt_invalid_symbol;
                continue;
            }
            break;
        }
        cell = (c == 'x') ? X : O;
        move.symbol = cell;
    }
};

/**
 * @brief Represents a computer player in the game.
 */
class Computer : public Player {
  public:
    /**
     * @brief Constructs a default computer player named "Computer" as CHAOS.
     * @post Computer player is initialized with default identity.
     */
    Computer() = delete;

    /**
     * @brief Constructs a computer player with explicit identity.
     * @param name Display name for the player.
     * @param type Role for this player.
     * @pre type is ORDER or CHAOS.
     */
    Computer(PlayerType type) : Player(type) {}

    /**
     * @brief Chooses a move for the computer player.
     * @param game_board Current board state.
     * @param console Renderer available for status output.
     * @return Computer-selected move.
     * @pre Method implementation must return a valid move.
     */
    Move get_move(const GameBoard* game_board, Console& console) const override {
        vector<Move> available_moves = get_valid(game_board);
        int total_moves = available_moves.size();
        Move move = available_moves[rand() % total_moves];
        return move;
    }

    // returns all valid moves that computer can choose from
    vector<Move> get_valid(const GameBoard* game_board) const {
        vector<Move> available_moves = {};
        int size = game_board->get_size();

        for (size_t r = 0; r < size; r++) {
            for (size_t c = 0; c < size; c++) {
                Cell symbol = game_board->at(r, c);
                if (symbol == E) {
                    Move move(r, c, X);
                    available_moves.push_back(move);
                }
            }
        }
        return available_moves;
    }

  private:
};

/**
 * @brief Represents the main game logic, including the game loop, and
 * player turns.
 */
class Game {
  public:
    /**
     * @brief Constructs a new game controller.
     * @post Game object is ready for initialization via play/start.
     */
    Game()
        : game_board(nullptr), //
          player1(nullptr),    //
          player2(nullptr),    //
          console() {}

    void play() {
        bool repeat = true;
        start();
        sleep(1);
        cycle_turns();
    }

    ~Game() {
        delete game_board;
        delete player1;
        delete player2;
    }

  private:
    GameBoard* game_board;
    Player* player1;
    Player* player2;
    Console console;
    size_t console_game_board;

    /**
     * @brief Runs full game sessions until the players choose to
     * stop.
     * @pre Input/output streams are available.
     * @post Returns only after the user declines another game.
     */

    /**
     * @brief Initializes the game and prints player instructions.
     * @pre Game object exists.
     * @post Board has been configured by user input.
     */
    void start() {
        title();
        introduction();
        setup_board();
        setup_players();
        print_player_roles();
        print_player_order();
        setup_console();
    }

    /**
     * @brief Executes one turn for the active player.
     * @param player The player taking this turn.
     * @pre player is a valid initialized Player object.
     * @post A move is requested from player.
     */
    void cycle_turns() {
        while (true) {
            game_board->place(player1->get_move(game_board, console));
            console.overwrite(console_game_board, to_string(game_board));
            game_board->place(player2->get_move(game_board, console));
            console.overwrite(console_game_board, to_string(game_board));
        }
    }

    /**
     * @brief Handles end-of-game logic.
     * @return true to start another game, false to stop.
     * @post Return value controls whether play() repeats.
     */
    bool end() {
        return false;
    }

    void display(const string& str) {
        console.push(Block(str));
        wait_for_enter();
        console.pop();
    }

    void wait_for_enter() {
        console.read("\nPress Enter to continue: ");
    }

    void title() {}

    /**
     * @brief Shows opening instructions and waits for Enter.
     * @pre Console I/O is available.
     * @post Intro text has been displayed and dismissed.
     */
    void introduction() {
        display( //
            "\nWelcome to Order and Chaos!"
            "\nIn this game, two players take turns placing Os and Xs onto the board."
            "\nEach turn, both players can choose whether to place an O or and X."
            "\nOrder wins if they can place 5 Xs or Os in a row. Chaos wins if they can "
            "prevent this."
        );
    }

    /**
     * @brief Prompts for and validates board size.
     * @pre Console I/O is available.
     * @post game_board is set to a size between 6 and 9.
     */
    void setup_board() {
        string prompt = "\nEnter a board size from 6 to 9: ";
        int size;
        while (true) {
            stringstream input = console.read(prompt);

            input >> size;

            if (input.fail()) {
                prompt = "\n\"" + input.str() +
                         "\" is not an integer. Please enter an integer from 6 to 9: ";
                continue;
            }

            if (!(size >= 6 && size <= 9)) {
                prompt = "\n" + to_string(size) +
                         " is not a valid board size. Please enter one from 6 to 9: ";
                continue;
            }
            break;
        }
        game_board = new GameBoard(size);
    }

    /**
     * @brief Randomly assigns player order between human and computer.
     * @pre human and computer players are initialized.
     * @post player1 and player2 point to different player objects.
     */
    void setup_players() {
        int num = rand() % 2;
        PlayerType p1_type = (num == 0) ? ORDER : CHAOS;
        PlayerType p2_type = (num == 0) ? CHAOS : ORDER;

        num = rand() % 2;
        if (num == 0) {
            player1 = new Human(p1_type);
            player2 = new Computer(p2_type);
        } else {
            player1 = new Computer(p1_type);
            player2 = new Human(p2_type);
        }
    }

    void print_player_roles() {
        display(
            "\nPlayer 1 is " + to_string(player1->get_type()) + //
            " and Player 2 is " + to_string(player2->get_type()) + "."
        );
    }
    void print_player_order() {
        display(
            "\nPlayer 1 goes first, followed by Player 2. " //
            "\nLet's begin."
        );
    }

    void setup_console() {
        console.push("\nHere's the initial game board:");
        console.push(to_string(game_board));
        console_game_board = console.size() - 1;
    }
};

/**
 * @brief Program entry point.
 * @return Exit status code from the process.
 * @post Game session is run until completion.
 */
int main() {
    srand(time(nullptr));
    Game game;
    game.play();
} // main
