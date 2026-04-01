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
#include <iostream>
#include <vector>

using namespace std;

class GameBoard {
  public:
  private:
    struct PossibleWins {
        vector<bool> rows{5, false};
        vector<bool> cols{5, false};
        bool diag1 = false;
        bool diag2 = false;
    };

    vector<vector<int>>* board;
    PossibleWins* possible_wins;
};

class Player {
  public:
  private:
    enum PlayerType { ORDER, CHAOS };
    const string name;
    const char symbol;
    const PlayerType type;
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

    void startGame(){
      string instructions = 
        R"(Order and Chaos!
        Two players take turns placing Os and Xs onto the board.
        Both players can choose wheter to place an O or and X each turn.
        Order wins if they can place 5 Xs or Os in a row and Choas wins if they can prevent this)";

      
      cout<<instructions;
    }

  private:
    enum GameState {};

    GameBoard* game_board = new GameBoard();
    GameState game_state;
    Player* player1;
    Player* player2;
};

int main() {} // main
