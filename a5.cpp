// a5.cpp

/////////////////////////////////////////////////////////////////////////
//
// Student Info
// ------------
//
// Name : Tolly Zhang
// St.# : 301662413
// Email: tza115@sfu.ca
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
#include <cstdlib>
#include <iostream>
#include <limits>
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
char to_lower(char c) {
    if ('A' <= c && c <= 'Z') {
        return c + 32;
    }
    return c;
}

char to_upper(char c) {
    if ('a' <= c && c <= 'z') {
        return c - 32;
    }
    return c;
}

string capitalize(string str) {
    str[0] = to_upper(str[0]);
    return str;
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

string blue(const string& str) {
    return "\033[34m" + str + "\033[0m";
}

string green(const string& str) {
    return "\033[32m" + str + "\033[0m";
}

enum Cell { E, O, X };

string str(Cell c) {
    switch (c) {
    case E:
        return " ";
    case O:
        return "O";
    case X:
        return "X";
    }
    return "";
}

string color(Cell c) {
    switch (c) {
    case E:
        return " ";
    case O:
        return blue(str(c));
    case X:
        return green(str(c));
    }
    return "";
}

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

enum ComputerDifficulty { RANDOM, SMART };
struct Move {
    size_t row;
    size_t col;
    Cell symbol;

    Move(int r, int c, Cell s)
        : row(r), //
          col(c), //
          symbol(s) {}
};

struct MoveScore {
    Move move;
    double score;
    MoveScore(const Move& move, const double score) : move(move), score(score) {}
};

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
    Console() {
        cout << open_terminal << flush;
    }

    size_t size() const {
        return blocks.size();
    }

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

    void display(const Block& block, const int duration_s) {
        push(block);
        sleep(duration_s);
        pop();
    }

    ~Console() {
        cout << close_terminal << flush;
    }

  private:
    vector<Block> blocks;
    const string                        //
        open_terminal = "\033[?1049h",  //
        close_terminal = "\033[?1049l", //
        clear_all = "\033[2J",          //
        cursor_home = "\033[H";

    void refresh() {
        erase_all();
        move_cursor_top_left();
        print();
    }

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
        cout << clear_all << flush;
    }
    void move_cursor_top_left() {
        cout << cursor_home << flush;
    }
};
class GameBoard {
  public:
    GameBoard() : GameBoard(0) {}

    GameBoard(int n)
        : board(vector<vector<Cell>>(n, vector<Cell>(n, E))), //
          size(n),                                            //
          game_over(false) {}

    size_t get_size() const {
        return size;
    }

    char get_row_start_label() const {
        return ROW_LABEL_START;
    }

    int get_col_start_label() const {
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

    size_t row(const char r) const {
        return r - ROW_LABEL_START;
    }
    size_t col(const int c) const {
        return c - COL_LABEL_START;
    }

    string r_label(const size_t r) const {
        return str(char(r + ROW_LABEL_START));
    }

    string c_label(const size_t c) const {
        return to_string(c + COL_LABEL_START);
    }

    Cell at(size_t row, size_t column) const {
        return board[row][column];
    }

    void place(const Move& move, Console& console, const PlayerType type) {
        size_t r = move.row;
        size_t c = move.col;

        board[r][c] = move.symbol;

        string message =
            str(type) + " played " + str(move.symbol) + " at " + coords(r_label(r), c_label(c));
        console.display(Block({"", message}), 2);

        game_over = check_win(move);
    }

    void place(const Move& move) {
        board[move.row][move.col] = move.symbol;
        game_over = check_win(move);
    }

    bool order_won() const {
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
            string plain = str(board->at(r, c));
            string padded = aligned(plain, width - 2);
            string colored = color(board->at(r, c));
            if (plain.size() > 0) {
                padded = padded.substr(0, padded.size() - plain.size()) + colored;
            }
            append(padded);
        }
        append(" |");
        add_line(row_separator);
    }
}
class Player {
  public:
    Player(PlayerType type, const string& header) : type(type), display_header(header) {}

    PlayerType get_type() const {
        return type;
    }

    void set_type(PlayerType t) {
        type = t;
    }

    Move move(const GameBoard* board, Console& console) {
        Move move = determine_move(board, console);
        return move;
    }

    virtual Move determine_move(const GameBoard* board, Console& console) = 0;

    virtual ~Player() {}

  protected:
    PlayerType type;
    string display_header;
};
class Human : public Player {
  public:
    Human() = delete;

    Human(PlayerType type) : Player(type, "you") {}

    Move determine_move(const GameBoard* board, Console& console) override {
        Move move(0, 0, X);
        set_coords(move, board, console);
        if (resigned) {
            return move;
        }
        set_symbol(move, console);
        return move;
    }

    bool has_resigned() const {
        return resigned;
    }

    ~Human() {}

  private:
    bool resigned = false;

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

class Engine {
  public:
    Engine() = delete;

    static vector<Move> get_valid_moves(const GameBoard* board) {
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

    static vector<MoveScore> evaluate_move_scores(
        const GameBoard* board, const PlayerType current_player, const int max_depth
    ) {
        vector<MoveScore> move_scores;
        vector<Move> moves = get_valid_moves(board);
        const double neg_inf = -numeric_limits<double>::infinity();
        const double pos_inf = numeric_limits<double>::infinity();

        for (const Move& move : moves) {
            GameBoard next_board = *board;
            next_board.place(move);

            PlayerType next_player = current_player == ORDER ? CHAOS : ORDER;
            double score = evaluate(&next_board, next_player, neg_inf, pos_inf, 1, max_depth);
            move_scores.push_back(MoveScore(move, score));
        }
        sort(move_scores);
        return move_scores;
    }

    static void sort(vector<MoveScore>& move_scores) {
        std::sort(
            move_scores.begin(), move_scores.end(), [](const MoveScore& a, const MoveScore& b) {
                return a.score > b.score;
            }
        );
    }

    static double evaluate(
        const GameBoard* board,
        PlayerType player,
        double alpha,
        double beta,
        const int depth,
        const int max_depth
    ) {
        if (board->order_won()) {
            return 1000000000.0 - depth;
        }

        if (board->is_full()) {
            return -1000000000.0 + depth;
        }

        if (depth >= max_depth) {
            return evaluate_board(board);
        }

        vector<Move> moves = get_valid_moves(board);

        if (player == ORDER) {
            double best = -numeric_limits<double>::infinity();

            for (const Move& move : moves) {
                GameBoard next_board = *board;
                next_board.place(move);

                double score = evaluate(&next_board, CHAOS, alpha, beta, depth + 1, max_depth);
                best = max(best, score);
                alpha = max(alpha, best);

                if (beta <= alpha) {
                    break;
                }
            }
            return best;
        }

        double best = numeric_limits<double>::infinity();

        for (const Move& move : moves) {
            GameBoard next_board = *board;
            next_board.place(move);

            double score = evaluate(&next_board, ORDER, alpha, beta, depth + 1, max_depth);
            best = min(best, score);
            beta = min(beta, best);

            if (beta <= alpha) {
                break;
            }
        }
        return best;
    }

  private:
    static double evaluate_board(const GameBoard* board) {
        return evaluate_symbol(board, X) + evaluate_symbol(board, O);
    }

    static int evaluate_symbol(const GameBoard* board, Cell symbol) {
        int size = board->get_size();
        vector<vector<vector<int>>> bounds = {
            {{0, size - 1}, {0, size - 5}, {0, 1}},
            {{0, size - 5}, {0, size - 1}, {1, 0}},
            {{0, size - 5}, {0, size - 5}, {1, 1}},
            {{0, size - 5}, {4, size - 1}, {1, -1}}
        };

        int score = 0;
        for (vector<vector<int>> bound : bounds) {
            score += evaluate_direction(
                board,
                symbol,
                bound[0][0], // r_start
                bound[0][1], // r_end
                bound[1][0], // c_start
                bound[1][1], // c_end
                bound[2][0], // dr
                bound[2][1]  // dc
            );
        }
        return score;
    }

    static int evaluate_direction(
        const GameBoard* board,
        Cell symbol,
        int r_start,
        int r_end,
        int c_start,
        int c_end,
        int dr,
        int dc
    ) {
        int score = 0;
        for (int r = r_start; r <= r_end; ++r) {
            for (int c = c_start; c <= c_end; ++c) {
                score += connected_score(board, symbol, r, c, dr, dc);
            }
        }
        return score;
    }

    static int connected_score(
        const GameBoard* board_state, //
        const Cell target,            //
        int r,                        //
        int c,                        //
        const int dr,                 //
        const int dc
    ) {
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

        vector<int> scores = {0, 1, 10, 100, 10000, 1000000};
        if (target_count >= 0 && target_count <= scores.size() - 1) {
            return scores[target_count];
        }
        return 0;
    }
};

class Computer : public Player {
  public:
    Computer() = delete;

    Computer(PlayerType type) : Player(type, "the computer") {}

    Move determine_move(const GameBoard* board, Console& console) override {
        (void)board;
        console.display(Block({"", "Computer is thinking..."}), 1);
        return Move(0, 0, X);
    }

    ~Computer() {}
};

class RandomComputer : public Computer {
  public:
    RandomComputer(PlayerType type) : Computer(type) {}

    Move determine_move(const GameBoard* board, Console& console) override {
        Computer::determine_move(board, console);
        vector<Move> moves = Engine::get_valid_moves(board);

        int num_moves = moves.size();
        int choice = rand() % num_moves;
        Move move = moves[choice];

        return move;
    }
};

class SmartComputer : public Computer {
  public:
    SmartComputer(PlayerType type) : Computer(type) {}

    Move determine_move(const GameBoard* board, Console& console) override {
        Computer::determine_move(board, console);
        vector<MoveScore> move_scores = Engine::evaluate_move_scores(board, type, DEPTH);
        if (type == ORDER) {
            return move_scores[0].move;
        } else {
            return move_scores.back().move;
        }
    }

  private:
    const int DEPTH = 2;
};
class Game {
  public:
    Game()
        : board(nullptr),                    //
          human(nullptr), computer(nullptr), //
          winner(nullptr),                   //
          first(nullptr), second(nullptr),   //
          order(nullptr), chaos(nullptr),    //
          console(),                         //
          console_board(0)                   //
    {}

    void play() {
        title();
        introduction();
        while (true) {
            setup_game();
            cycle_turns();
            confirm({str(winner->get_type()) + " wins!"});

            if (end()) {
                cleanup();
                continue;
            }
            break;
        }
    }

    ~Game() {
        cleanup();
    }

  private:
    GameBoard* board;

    Human* human;
    Computer* computer;

    Player* winner;
    Player *first, *second;
    Player *order, *chaos;

    Console console;
    size_t console_board;

    void cleanup() {
        delete board;
        board = nullptr;

        delete human;
        delete computer;
        human = nullptr;
        computer = nullptr;

        first = nullptr;
        second = nullptr;

        winner = nullptr;

        console.clear();
        console_board = 0;
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

    void introduction() {
        confirm(
            {"Welcome to Order and Chaos!",
             "In this game, two players take turns placing Os and Xs onto the board.",
             "Each turn, both players can choose whether to place an O or an X.",
             "Order wins if they can place 5 Xs or Os in a row. Chaos wins if they can "
             "prevent this until the board is full."}
        );
    }

    void setup_game() {
        setup_board();
        setup_players();
        setup_console();
    }

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

    void setup_players() {
        setup_human_computer();
        setup_order_chaos();
        setup_move_order();

        string first_name = first == human ? "You" : "The computer";
        confirm(
            {"You are playing as " + str(human->get_type()) + ". The computer is playing as " +
                 str(computer->get_type()) + ".",
             first_name + " will go first."}
        );
    }

    void setup_human_computer() {
        int num = rand() % 2;
        PlayerType human_type = num == 0 ? ORDER : CHAOS;
        PlayerType computer_type = num == 0 ? CHAOS : ORDER;

        human = new Human(human_type);
        if (get_computer_difficulty() == RANDOM) {
            computer = new RandomComputer(computer_type);
        } else {
            computer = new SmartComputer(computer_type);
        }
    }

    void setup_order_chaos() {
        if (human->get_type() == ORDER) {
            order = human;
            chaos = computer;
        } else {
            order = computer;
            chaos = human;
        }
    }

    void setup_move_order() {
        int num = rand() % 2;

        if (num == 0) {
            first = human;
            second = computer;
        } else {
            first = computer;
            second = human;
        }
    }

    ComputerDifficulty get_computer_difficulty() {
        Block prompt({"", "Please choose difficulty: (1) Random, (2) Smart"});
        int num;
        while (true) {
            stringstream input = console.ask(prompt);

            input >> num;

            if (input.fail()) {
                prompt.text[0] = "Invalid input.";
                continue;
            }

            if (!(num == 1 || num == 2)) {
                prompt.text[0] = quote(to_string(num)) + " is not a valid choice.";
                continue;
            }
            break;
        }
        return num == 1 ? RANDOM : SMART;
    }

    void setup_console() {
        console.push(Block(board));
        console_board = console.size() - 1;
        console.push({""});
    }

    void cycle_turns() {
        while (true) {
            if (turn(first)) return;
            if (turn(second)) return;
        }
    }

    bool turn(Player* player) {
        Move move = player->move(board, console);

        if (player == human && human->has_resigned()) {
            winner = computer;
            return true;
        }

        board->place(move, console, player->get_type());
        console.overwrite(console_board, Block(board));

        if (board->order_won()) {
            winner = order;
            return true;
        }
        if (board->is_full()) {
            winner = chaos;
            return true;
        }
        return false;
    }

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
};
int main() {
    srand(time(nullptr));
    Game game;
    game.play();
} // main

/*
Assignment 5 Report
===================

Description of Computers Playing Strategy
-----------------------------------------
Both computer are built upon an BoardEngine class that has methods that return possible moves and
evaluates the current board state for how "winnable" it is for Order. It does this by scanning every
possible 5-in a row and seeing how filled in they are and sums up weights to generate an integer
score.

RandomComputer
- This computer plays random moves from a vector of available moves on the board. This bot is good
if you want to debug.

SmartComputer
- This computer simulates all possible moves and uses the engine to evaluate all the future possible
boards to determine the move that creates the most favorable position.



Extra Features
--------------
1. Console Class
- This is a custom-built class that handles all I/O for the user, It stores a stack of string blocks
for all content displayed on screen and updates them by erasing then reprinting everything using
ASCII escape codes.
- It allows complex UI structures and efficient handling combined with stringstream.
- It was probably the most challenging aspect of this project and took several versions to get right
reliably.
- It allows the code to print, remove, overwrite, and take in user input all together all within
itself.

2. GameBoard Class
- Stores a 2d vector matrix of Cells that represent the current game state.
- It handles move placement and win checking efficiently in O(1) time
- Checks whether the last move made by players was a winning move.

3. Game Class
- Handles game setup
- Handles game end
- Handles turn cycling

Class Hierarchy:

4. Player Class
    5. Human Class
    - Handles user input within itself

    6. Computer Class
        7. Smart Computer
            - Refer to Playing strategy

        8. Random Computer
            - Refer to Playing strategy


Known Bugs and Limitations
--------------------------
- Please ensure the terminal window is tall enough to show the entire game. The Console class should
theoretically handle window cutoffs without issue but there may still be unknown bugs.
- Only type input when prompted and don't spam characters when text is being displayed. All prompts
start with a "| " and end with a ">". The code attempts to clean out stream buffers but it is not
completely reliable yet.
- Extra trailing input is disregarded by design, so input such as "a4someextratext" will still be
parsed as "a4".

Acknowledgements of All Help
----------------------------
- You can view this project's GitHub at https://github.com/Tolly-Zhang/order-and-chaos which shows
the entire commit history. Most edits were made on one machine while collaborating using VS Code
Live Share.
- The project utilizes clang-format, a C++ linter, any extra trailing //s are used to aid in its
formatting.

SOURCES
1. https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797 for ASCII escape codes.
2. https://en.cppreference.com/w/cpp/io/manip/flush.html for cout << flush.
3. https://www.w3schools.com/cpp/cpp_enum.asp for enums.
4. https://en.cppreference.com/w/cpp/types/size_t.html for appropriate size_t usage.
5. https://www.geeksforgeeks.org/cpp/stringstream-c-applications/ for stringstream usage.
6. https://www.w3schools.com/cpp/ref_keyword_protected.asp for protected members in classes
7. https://cplusplus.com/reference/streambuf/streambuf/in_avail/ for stream buffer clearing
7. https://en.cppreference.com/w/cpp/language/lambda.html for lamdas
8. https://pubs.opengroup.org/onlinepubs/009696799/functions/sleep.html for sleep()
9. https://en.cppreference.com/w/cpp/language/function.html#Deleted_functions for deleted functions
*/