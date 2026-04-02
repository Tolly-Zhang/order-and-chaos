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

enum Cell { E, O, X };

struct Move {
    int row;
    int col;
    Cell symbol;
    Move(int r, int c, Cell s) : row(r), col(c), symbol(s) {
        assert(s == O || s == X);
    }
};

class GameBoard {
  public:
  private:
    struct PossibleWins {
        vector<bool> rows;
        vector<bool> cols;
        bool diag1 = false;
        bool diag2 = false;

        PossibleWins(const int r, const int c) : rows(r, true), cols(c, true) {}
    };
    vector<vector<Cell>> board{};
    PossibleWins possible_wins;
};

class Player {
  private:
    enum PlayerType { ORDER, CHAOS };
    const string name;
    const PlayerType type;

  public:
    Player(const string& name, PlayerType type) : name(name), type(type) {}
};

class Human : public Player {
  public:
  private:
};

class Computer : public Player {
  public:
  private:
};

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
    void move(Player& player) {}

    bool end() {}

  private:
    enum GameState {};

    GameBoard game_board;
    GameState game_state;
    Player* player1;
    Player* player2;
};

int main() {} // main
