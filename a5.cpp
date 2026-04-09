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

class ConsoleRenderer {
  public:
    vector<Block> blocks;
    int lines;
    /**
     * @brief Creates an empty renderer state.
     * @post No blocks are stored and lines == 0.
     */
    ConsoleRenderer() : lines(0) {}

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

    /**
     * @brief Removes the newest block and redraws.
     * @param extra_lines Number of additional input lines to clear.
     * @pre None.
     * @post If a block existed, the most recent one is removed.
     */
    void pop() {
        pop(0);
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
     * @brief Prints the board with row and column labels.
     * @return Multiline board representation with coordinate labels.
     * @post Returned string ends with a newline.
     */
    string str() const {
        string out = "\n";
        int width = column_width() + 1;
        out += string(width, ' ');
        for (int i = COL_LABEL_START; i <= int(size); ++i) {
            append_aligned(out, to_string(i), width);
        }
        out += "\n";
        for (size_t r = 0; r < size; ++r) {
            append_aligned(out, string(1, char(ROW_LABEL_START + r)), width);
            for (size_t c = 0; c < size; ++c) {
                append_aligned(out, to_string(board[r][c]), width);
            }
            out += "\n";
        }
        return out;
    }

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
    char get_row_start() const {
        return ROW_LABEL_START;
    }

    /**
     * @brief Returns the first valid column label.
     * @return Starting column number.
     * @pre None.
     */
    int get_col_start() const {
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

    /**
     * @brief Checks whether a index is within board bounds.
     * @param i index, represents column or row since board is a square.
     * @return true if i maps to a valid row/column otherwise false.
     * @pre i is any integer value.
     */
    bool check_size_bounds(int i) const {
        return (0 <= i && i < get_size());
    }

    /**
     * @brief Returns a move given a row and column
     * @param row index, represents row of the board starting at top.
     * @param column index, represents column of the board starting at left.
     * @return Cell, either E, O or X.
     * @pre row and column are within bounds.
     */
    Cell get_cell(int row, int column) const {
        if (check_size_bounds(row) && check_size_bounds(column)) {
            return board[row][column];
        }
    }

    bool is_empty(int row, int column) const {
        return get_cell(row, column) == E;
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

    /**
     * @brief Computes width needed to align board columns.
     * @return Number of digits in board size.
     * @pre size >= 0.
     */
    int column_width() const {
        return to_string(size).length();
    }

    static void append_aligned(string& text, const string& value, int width) {
        int padding = width - static_cast<int>(value.length());
        if (padding > 0) {
            text += string(padding, ' ');
        }
        text += value;
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
        ConsoleRenderer& console
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
    Move get_move(const GameBoard* game_board, ConsoleRenderer& console) const override {
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
        ConsoleRenderer& console     //
    ) const {
        string row_range =
            to_string(game_board->get_row_start()) + " to " +
            to_string(game_board->get_row_start() + game_board->get_size() - 1);
        string col_range =
            to_string(game_board->get_col_start()) + " to " +
            to_string(game_board->get_col_start() + game_board->get_size() - 1);

        char row;
        int col;

        string prompt = "\nEnter a coordinate in the format of [row] [col] from " + //
                        row_range + " and " + col_range + ": ";
        while (true) {
            console.push(prompt);
            cin >> row >> col;
            cin.ignore(1000, '\n');
            console.pop_prompt();

            if (!game_board->check_row_bounds(row)) {
                prompt = "\n" + to_string(row) +
                         " is not a valid row. Please enter a row from " + //
                         row_range + ": ";
                continue;
            }

            if (!game_board->check_column_bounds(col)) {
                prompt = "\n" + to_string(col) +
                         " is not a valid column. Please enter a column from " + //
                         col_range + ": ";
                continue;
            }
            break;
        }
        move.row = row - game_board->get_row_start();
        move.col = col - game_board->get_col_start();
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
        Move& move,              //
        ConsoleRenderer& console //
    ) const {
        char c;
        Cell cell;

        string prompt = "\nEnter a symbol (x or o): ";
        while (true) {
            console.push(prompt);
            cin >> c;
            cin.ignore(1000, '\n');
            c = to_lower(c);
            console.pop_prompt();

            if (!(c == 'o' || c == 'x')) {
                prompt = "\n" + to_string(c) + //
                         " is not a valid symbol. Enter x or o:";
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
    Move get_move(const GameBoard* game_board, ConsoleRenderer& console) const override {
        vector<Move> availableMoves = get_valid(game_board);
        int totalMoves = availableMoves.size();
        Move selectedMove = availableMoves[rand() % totalMoves];

        if (totalMoves <= 0) {
            // no moves left
            // add a game over message
            console.push("\nGame over!");
        }
        return selectedMove;
    }

    // returns all valid moves that computer can choose from
    vector<Move> get_valid(const GameBoard* game_board) const {
        vector<Move> availableMoves = {};
        int gameboardSize = game_board->get_size();

        for (int row; row < gameboardSize; row++) {
            for (int column; column < gameboardSize; column++) {
                Cell currentSymbol = game_board->get_cell(row, column);
                if (currentSymbol == E) {
                    Move newMove(row, column, E);
                }
            }
        }

        return availableMoves;
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
        : game_board(nullptr),         //
          game_board_display(nullptr), //
          player1(nullptr),            //
          player2(nullptr),            //
          console() {}

    void play() {
        bool repeat = true;
        start();
        console.push(game_board->str());
        sleep(1);
        console.pop();
        sleep(1);
        console.push("\nGame over!");
        sleep(1);
    }

    ~Game() {
        delete game_board;
        delete player1;
        delete player2;
    }

  private:
    GameBoard* game_board;
    string* game_board_display;
    Player* player1;
    Player* player2;
    ConsoleRenderer console;

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
        introduction();
        setup_board();
        setup_players();
        print_player_roles();
        print_player_order();
    }

    /**
     * @brief Executes one turn for the active player.
     * @param player The player taking this turn.
     * @pre player is a valid initialized Player object.
     * @post A move is requested from player.
     */
    void turn(Player& player) {
        Move move = player.get_move(game_board, console);
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
        console.push("\nPress Enter to continue>");
        cin.get();
        console.pop_prompt();
    }

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
        string input;
        int size;
        while (true) {
            console.push(prompt);
            getline(cin, input);
            console.pop_prompt();

            if (input.empty()) {
                prompt = "\nNo input detected. Please enter a board size from 6 to 9: ";
                continue;
            }

            try {
                size = stoi(input);
            } catch (...) {
                prompt = "\n" + input +
                         " is not an integer. Please enter an integer from 6 to 9: ";
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
