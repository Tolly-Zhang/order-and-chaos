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

class GameBoard;
class Console;
class Player;
class Human;
class Computer;
class Game;

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

string str(char c) {
    return string(1, c);
}

string quote(const string& str) {
    return "\"" + str + "\"";
}

string parenthesis(const string& str) {
    return "(" + str + ")";
}

string coords(const string& row, const string& col) {
    return "(" + row + ", " + col + ")";
}

/**
 * @brief Represents the state of a cell on the game board.
 */
enum Cell {
    E, ///< Empty cell
    O, ///< Cell occupied by player O
    X  ///< Cell occupied by player X
};

string str(Cell c) {
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

string str(PlayerType type) {
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
    vector<string> text;

    Block() {};

    Block(const vector<string>& text) : text(text) {}

    Block(const GameBoard* board);

    size_t size() const {
        return text.size();
    }

    void append(const string& str) {
        text.back() += str;
    }

    void add_line(const string& str = "") {
        text.push_back(str);
    }
};
class Console {
  public:
    /**
     * @brief Creates an empty renderer state.
     * @post No blocks are stored and lines == 0.
     */
    Console() {
        cout << "\033[?1049h" << flush;
    }

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
        refresh();
    }

    void push(const vector<string>& text) {
        push(Block(text));
    }

    void pop() {
        if (blocks.empty()) return;
        blocks.pop_back();
        refresh();
    }

    void clear() {
        blocks.clear();
        refresh();
    }

    void overwrite(const size_t i, const Block& block) {
        if (i >= blocks.size()) return;
        blocks[i] = block;
        refresh();
    }

    stringstream ask(Block prompt) {
        clear_input_buffer();
        if (!prompt.text.empty()) {
            prompt.text.back() = "| " + prompt.text.back() + "> ";
        }

        push(prompt);

        string input;
        getline(cin, input);
        pop();

        stringstream ss(input);
        return ss;
    }

    ~Console() {
        cout << "\033[?1049l" << flush;
    }

  private:
    vector<Block> blocks;
    /**
     * @brief Recomputes total line count and redraws all blocks.
     * @pre Internal block list is valid.
     * @post lines equals total rendered block lines.
     */
    void refresh() {
        erase_all();
        move_cursor_top_left();
        print();
    }

    /**
     * @brief Prints all stacked blocks in order.
     * @pre Output stream is writable.
     * @post Visible console output matches stacked blocks.
     */
    void print() const {
        for (const Block& block : blocks) {
            for (const string& line : block.text) {
                cout << "\n" << line << flush;
            }
        }
    }

    void clear_input_buffer() {
        if (!cin.good()) cin.clear();
        while (cin.rdbuf()->in_avail() > 0) {
            string junk;
            getline(cin, junk);
        }
    }

    void erase_all() {
        cout << "\033[2J";
    }
    void move_cursor_top_left() {
        cout << "\033[H";
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
        : board(vector<vector<Cell>>(n, vector<Cell>(n, E))), //
          size(n),                                            //
          game_over(false) {}

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
        game_over = check_win(move);
    }

    bool is_game_over() const {
        return game_over;
    }

    bool is_full() const {
        for (vector<Cell> row : board) {
            for (Cell cell : row) {
                if (cell == E) {
                    return false;
                }
            }
        }
        return true;
    }

    bool is_empty(size_t r, size_t c) const {
        return at(r, c) == E;
    }

  private:
    vector<vector<Cell>> board;
    size_t size;
    bool game_over;
    const char ROW_LABEL_START = 'a';
    const int COL_LABEL_START = 1;

    bool check_win(const Move& move) {
        if (check_win_direction(move, 0, 1) || //
            check_win_direction(move, 1, 0) || //
            check_win_direction(move, 1, 1) || //
            check_win_direction(move, 1, -1)) {
            return true;
        }
        return false;
    }

    bool check_win_direction(const Move& move, int dr, int dc) {
        int r = move.row;
        int c = move.col;
        int count = 0;

        while (r >= 0 && c >= 0 && r <= size - 1 && c <= size - 1) {
            if (at(r, c) == move.symbol) {
                ++count;
                r -= dr;
                c -= dc;
                continue;
            }
            break;
        }

        r = move.row + dr;
        c = move.col + dc;

        while (r >= 0 && c >= 0 && r <= size - 1 && c <= size - 1) {
            if (at(r, c) == move.symbol) {
                ++count;
                r += dr;
                c += dc;
                continue;
            }
            break;
        }

        if (count >= 5) {
            return true;
        }
        return false;
    }
};

string str(const Move& move, const GameBoard* board) {
    return str(move.symbol) + " at " +
           coords(
               str(char(move.row + board->get_row_start_label())),
               to_string(move.col + board->get_col_start_label())
           );
}

Block::Block(const GameBoard* board) {
    auto aligned = [](const string& str, const int width) {
        int padding = width - int(str.length());
        if (padding > 0) {
            return string(padding, ' ') + str;
        }
        return str;
    };

    auto separator = [](const int size) {
        string separator = "     +--";
        for (size_t c = 1; c < size; ++c) {
            separator += "-+--";
        }
        separator += "-+";
        return separator;
    };

    int width = to_string(board->get_size()).length() + 3;
    char row_label = board->get_row_start_label();
    int col_label = board->get_col_start_label();
    int size = int(board->get_size());

    string row_separator = separator(size);

    add_line("    ");

    for (int i = 0; i < size; ++i) {
        append(aligned(to_string(col_label + i), width));
    }

    add_line(row_separator);

    for (size_t r = 0; r < size; ++r) {
        add_line();
        append(aligned(str(char(row_label + r)), width));
        for (size_t c = 0; c < size; ++c) {
            append(" |");
            append(aligned(str(board->at(r, c)), width - 2));
        }
        append(" |");
        add_line(row_separator);
    }
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
     * @brief Gets the player's move based on the current game board state. This is a
     * pure virtual function that must be implemented by derived classes (Human and
     * Computer).
     * @param game_board The current game board.
     * @param console Console renderer used for prompts and updates.
     * @return The move chosen by the player.
     * @pre Implementations must return a valid move.
     */
    virtual Move get_move(
        const GameBoard* board, //
        Console& console
    ) = 0;

    bool has_resigned() const {
        return resigned;
    }

    virtual ~Player() = default;

  protected:
    bool resigned = false;
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
    Move get_move(const GameBoard* board, Console& console) override {
        Move move(0, 0, X);
        set_coords(move, board, console);
        set_symbol(move, console);
        return move;
    }

    ~Human() {}

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
        Move& move,             //
        const GameBoard* board, //
        Console& console        //
    ) {
        char row_start = board->get_row_start_label();
        char row_end = char(row_start + board->get_size() - 1);
        int col_start = board->get_col_start_label();
        int col_end = col_start + board->get_size() - 1;

        string range = parenthesis(str(row_start) + to_string(col_start)) + " to " + //
                       parenthesis(str(row_end) + to_string(col_end));

        char row_input;
        int col_input;
        size_t row;
        size_t col;

        Block prompt({"", "Please enter a coordinate from " + range + " or (quit)"});
        while (true) {
            stringstream input = console.ask(prompt);

            if (input.str() == "quit") {
                resigned = true;
                return;
            }

            input >> row_input >> col_input;
            if (input.fail()) {
                prompt.text[0] = "Invalid input. ";
                continue;
            }

            string invalid_row = quote(str(row_input)) + " is not a valid row. ";
            string invalid_col = quote(to_string(col_input)) + " is not a valid column. ";

            if (!(is_alpha(row_input))) {
                prompt.text[0] = invalid_row;
                continue;
            }

            row_input = to_lower(row_input);

            if (!board->check_row_bounds(row_input)) {
                prompt.text[0] = invalid_row;
                continue;
            }
            if (!board->check_column_bounds(col_input)) {
                prompt.text[0] = invalid_col;
                continue;
            }

            row = board->row(row_input);
            col = board->col(col_input);

            if (!board->is_empty(row, col)) {
                Cell cell = board->at(row, col);
                prompt.text[0] = coords(str(row_input), to_string(col_input)) +
                                 " is already taken by " + str(cell) +
                                 ". Please enter an empty coordinate";
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
        Block prompt({"", "Please enter a symbol (x) or (o)"});
        while (true) {
            stringstream input = console.ask(prompt);
            input >> c;

            string invalid = "Invalid input. ";

            if (input.fail()) {
                prompt.text[0] = invalid;
                continue;
            }

            c = to_lower(c);

            if (!(c == 'o' || c == 'x')) {
                prompt.text[0] = invalid;
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
    Computer(PlayerType type) : Player(type), engine() {}

    Move get_move(const GameBoard* board, Console& console) override {
        if (engine.board == nullptr) {
            engine.board = board;
        }
        console.push(Block({"", "Computer is thinking..."}));
        sleep(1);
        console.pop();
        return Move(0, 0, X);
    }

    ~Computer() {}

  protected:
    class BoardEngine {
      public:
        const GameBoard* board;

        BoardEngine() : board(nullptr) {}

        BoardEngine(const GameBoard* board) : board(board) {}

        bool is_board_set() const {
            return board != nullptr;
        }

        vector<Move> get_valid_moves() const {
            vector<Move> available_moves = {};
            int size = board->get_size();

            for (size_t r = 0; r < size; r++) {
                for (size_t c = 0; c < size; c++) {
                    Cell symbol = board->at(r, c);
                    if (symbol == E) {
                        available_moves.push_back(Move(r, c, X));
                        available_moves.push_back(Move(r, c, O));
                    }
                }
            }
            return available_moves;
        }

        int evaluate(const GameBoard* board_state) const {
            return evaluate_symbol(board_state, X) + evaluate_symbol(board_state, O);
        }

      private:
        int evaluate_symbol(const GameBoard* board_state, Cell symbol) const {
            int score = 0;
            int size = board_state->get_size();

            for (int r = 0; r <= size - 1; ++r) {
                for (int c = 0; c + 4 <= size - 1; ++c) {
                    score += calculate_score(board_state, symbol, r, c, 0, 1);
                }
            }

            for (int r = 0; r + 4 <= size - 1; ++r) {
                for (int c = 0; c <= size - 1; ++c) {
                    score += calculate_score(board_state, symbol, r, c, 1, 0);
                }
            }

            for (int r = 0; r + 4 <= size - 1; ++r) {
                for (int c = 0; c + 4 <= size - 1; ++c) {
                    score += calculate_score(board_state, symbol, r, c, 1, 1);
                }
            }

            for (int r = 0; r + 4 <= size - 1; ++r) {
                for (int c = 0; c + 4 <= size - 1; ++c) {
                    score += calculate_score(board_state, symbol, r, c + 4, 1, -1);
                }
            }

            return score;
        }

        int calculate_score(
            const GameBoard* board_state, //
            const Cell target,            //
            int r,                        //
            int c,                        //
            const int dr,                 //
            const int dc
        ) const {
            Cell opposite = target == X ? O : X;
            int target_count = 0;
            int opposite_count = 0;

            for (int i = 0; i < 5; ++i) {
                Cell current = board_state->at(r, c);
                if (current == target) {
                    ++target_count;
                } else if (current == opposite) {
                    ++opposite_count;
                }
                r += dr;
                c += dc;
            }

            if (target_count > 0 && opposite_count > 0) {
                return 0;
            }

            return target_count == 0   ? 0     //
                   : target_count == 1 ? 1     //
                   : target_count == 2 ? 10    //
                   : target_count == 3 ? 100   //
                   : target_count >= 4 ? 10000 //
                                       : 0;
        }
    };

    BoardEngine engine;

    void display_move(const Move& move, Console& console) {
        console.push(Block({"", "Computer played " + str(move, engine.board)}));
        sleep(2);
        console.pop();
    }
};

class RandomComputer : public Computer {
  public:
    RandomComputer(PlayerType type) : Computer(type) {}

    Move get_move(const GameBoard* board, Console& console) override {
        Computer::get_move(board, console);
        vector<Move> moves = engine.get_valid_moves();

        int num_moves = moves.size();
        int choice = rand() % num_moves;
        Move move = moves[choice];
        move.symbol = (rand() % 2 == 0) ? X : O;

        display_move(move, console);
        return move;
    }
};

class SmartComputer : public Computer {
  public:
    SmartComputer(PlayerType type) : Computer(type) {}

    Move get_move(const GameBoard* board, Console& console) override {
        Computer::get_move(board, console);
        Move move(0, 0, X);
        if (type == ORDER) {
            move = maximize_score();
        } else {
            move = minimize_score();
        }
        display_move(move, console);
        return move;
    }

  private:
    Move maximize_score() {
        vector<Move> moves = engine.get_valid_moves();
        int best_score = -1;
        Move best_move(0, 0, X);
        for (Move move : moves) {
            GameBoard board_copy = *engine.board;
            board_copy.place(move);
            int score = engine.evaluate(&board_copy);
            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
        }
        return best_move;
    }
    Move minimize_score() {
        vector<Move> moves = engine.get_valid_moves();
        int worst_score = 1000000000;
        Move worst_move(0, 0, X);
        for (Move move : moves) {
            GameBoard board_copy = *engine.board;
            board_copy.place(move);
            int score = engine.evaluate(&board_copy);
            if (score < worst_score) {
                worst_score = score;
                worst_move = move;
            }
        }
        return worst_move;
    }

    const int SEARCH_DEPTH = 5;
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
        : board(nullptr),    //
          player1(nullptr),  //
          player2(nullptr),  //
          order(nullptr),    //
          chaos(nullptr),    //
          human(nullptr),    //
          computer(nullptr), //
          console(),         //
          console_board(0),  //
          winner(nullptr) {}

    void play() {
        title();
        introduction();
        while (true) {
            setup_game();
            cycle_turns();
            confirm({"Game over! " + str(winner->get_type()) + " wins!"});
            if (!end()) {
                break;
            }
        }
    }

    ~Game() {
        delete board;
        delete player1;
        delete player2;
    }

  private:
    GameBoard* board;
    Player *player1, *player2;
    Player *order, *chaos;
    Human* human;
    Computer* computer;
    Console console;
    size_t console_board;
    Player* winner;

    /**
     * @brief Initializes the game and prints player instructions.
     * @pre Game object exists.
     * @post Board has been configured by user input.
     */
    void setup_game() {

        setup_board();
        setup_players();
        setup_console();
    }

    void cycle_turns() {
        while (true) {
            if (turn(player1)) {
                winner = player1;
                break;
            }
            if (turn(player2)) {
                winner = player2;
                break;
            }
        }
    }

    /**
     * @brief Executes one turn for the active player.
     * @param player The player taking this turn.
     * @pre player is a valid initialized Player object.
     * @post A move is requested from player.
     */
    Player* turn(Player* player) {

        if (player->has_resigned()) {
            return player == order ? chaos : order;
        }

        Move move = player->get_move(board, console);
        board->place(move);
        console.overwrite(console_board, board);
        if (board->is_game_over()) {
            return order;
        }
        if (board->is_full()) {
            return chaos;
        }
        return nullptr;
    }

    /**
     * @brief Handles end-of-game logic.
     * @return true to start another game, false to stop.
     * @post Return value controls whether play() repeats.
     */
    bool end() {
        Block prompt({"", "Play again? (y) or (n)"});
        string input;
        while (true) {
            stringstream ss = console.ask(prompt);
            ss >> input;

            if (ss.fail()) {
                prompt.text[0] = "Invalid input.";
                continue;
            }

            input = to_lower(input[0]);

            if (!(input == "y" || input == "n")) {
                prompt.text[0] = quote(input) + " is not a valid choice. ";
                continue;
            }
            break;
        }
        return input == "y";
    }

    void confirm(const Block& b) {
        console.push(b);
        wait_for_enter();
        console.pop();
    }

    void confirm(const vector<string>& text) {
        confirm(Block(text));
    }

    void wait_for_enter() {
        console.ask(Block({"Press Enter to continue"}));
    }

    void title() {
        confirm({
            R"(+=============================================================================+)",
            R"(|  ___          _                             _    ____ _                     |)",
            R"(| / _ \ _ __ __| | ___ _ __    __ _ _ __   __| |  / ___| |__   __ _  ___  ___ |)",
            R"(|| | | | '__/ _` |/ _ | '__|  / _` | '_ \ / _` | | |   | '_ \ / _` |/ _ \/ __||)",
            R"(|| |_| | | | (_| |  __| |    | (_| | | | | (_| | | |___| | | | (_| | (_) \__ \|)",
            R"(| \___/|_|  \__,_|\___|_|     \__,_|_| |_|\__,_|  \____|_| |_|\__,_|\___/|___/|)",
            R"(+=============================================================================+)",
        });
    }

    /**
     * @brief Shows opening instructions and waits for Enter.
     * @pre Console I/O is available.
     * @post Intro text has been displayed and dismissed.
     */
    void introduction() {
        confirm(
            {"Welcome to Order and Chaos!",
             "In this game, two players take turns placing Os and Xs onto the board.",
             "Each turn, both players can choose whether to place an O or an X.",
             "Order wins if they can place 5 Xs or Os in a row. Chaos wins if they can "
             "prevent this until the board is full."}
        );
    }

    /**
     * @brief Prompts for and validates board size.
     * @pre Console I/O is available.
     * @post game_board is set to a size between 6 and 9.
     */
    void setup_board() {
        Block prompt({"", "Enter a board size from (6) to (9)"});
        int size;
        while (true) {
            stringstream input = console.ask(prompt);

            input >> size;

            if (input.fail()) {
                prompt.text[0] = "Invalid input.";
                continue;
            }

            if (!(size >= 6 && size <= 9)) {
                prompt.text[0] = quote(to_string(size)) + " is not a valid board size.";
                continue;
            }
            break;
        }
        board = new GameBoard(size);
    }

    Computer* set_computer_difficulty(const PlayerType type) {
        Computer* computer = nullptr;

        Block prompt({"", "Please choose difficulty: (1) Random, (2) Smart"});
        int choice;
        while (true) {
            stringstream input = console.ask(prompt);

            input >> choice;

            if (input.fail()) {
                prompt.text[0] = "Invalid input.";
                continue;
            }

            if (!(choice == 1 || choice == 2)) {
                prompt.text[0] = quote(to_string(choice)) + " is not a valid choice.";
                continue;
            }
            break;
        }
        if (choice == 1) {
            computer = new RandomComputer(type);
        } else {
            computer = new SmartComputer(type);
        }
        return computer;
    }

    /**
     * @brief Randomly assigns player order between human and computer.
     * @pre human and computer players are initialized.
     * @post player1 and player2 point to different player objects.
     */
    void setup_players() {
        int num = rand() % 2;
        PlayerType p1_type;
        PlayerType p2_type;

        if (num == 0) {
            p1_type = ORDER;
            p2_type = CHAOS;
            order = player1;
            chaos = player2;
        } else {
            p1_type = CHAOS;
            p2_type = ORDER;
            order = player2;
            chaos = player1;
        }

        num = rand() % 2;
        if (num == 0) {
            human = new Human(p1_type);
            computer = set_computer_difficulty(p2_type);
            player1 = human;
            player2 = computer;
        } else {
            computer = set_computer_difficulty(p1_type);
            human = new Human(p2_type);
            player1 = computer;
            player2 = human;
        }

        string first = num == 0 ? "You" : "The computer";
        confirm(
            {"You are playing as " + str(p2_type) + ". The computer is playing as " + str(p1_type) +
                 ".",
             first + " will go first."}
        );
    }

    void setup_console() {
        console.push(Block(board));
        console_board = console.size() - 1;
        console.push({""});
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