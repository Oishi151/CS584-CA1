#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <unordered_set>
#include <string>

using namespace std;

// Define the board size
int BOARD_SIZE;

// Define the pieces
enum Piece { EMPTY, RED, BLUE, BLOCK };

// Define the board state
using Board = vector<vector<Piece>>;

// Define a structure to represent a move
struct Move {
    int x1, y1, x2, y2;
};

// Define a structure to represent the state of the puzzle
struct State {
    Board board;
    vector<Move> moves;
};

// Function to check if the board is in the goal state
bool isGoalState(const Board& board) {
    // Check if all red pieces are on the right side and all blue pieces are on the left side
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE / 2; ++j) {
            if (board[i][j] != BLUE) return false; // Left side should be blue
        }
        for (int j = BOARD_SIZE / 2; j < BOARD_SIZE; ++j) {
            if (board[i][j] != RED) return false; // Right side should be red
        }
    }
    return true;
}

// Function to generate all possible moves from the current state
vector<Move> generateMoves(const Board& board) {
    vector<Move> moves;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == EMPTY) {
                // Check moves for RED pieces (can move down and right)
                if (i > 0 && board[i - 1][j] == RED) {
                    moves.push_back({i - 1, j, i, j}); // Move RED down
                }
                if (j > 0 && board[i][j - 1] == RED) {
                    moves.push_back({i, j - 1, i, j}); // Move RED right
                }
                // Check moves for BLUE pieces (can move up and left)
                if (i < BOARD_SIZE - 1 && board[i + 1][j] == BLUE) {
                    moves.push_back({i + 1, j, i, j}); // Move BLUE up
                }
                if (j < BOARD_SIZE - 1 && board[i][j + 1] == BLUE) {
                    moves.push_back({i, j + 1, i, j}); // Move BLUE left
                }
            }
        }
    }
    return moves;
}

// Function to apply a move to the board
Board applyMove(const Board& board, const Move& move) {
    Board newBoard = board;
    swap(newBoard[move.x1][move.y1], newBoard[move.x2][move.y2]);
    return newBoard;
}

// Function to convert the board state to a string for hashing
string boardToString(const Board& board) {
    string s;
    for (const auto& row : board) {
        for (const auto& piece : row) {
            s += to_string(piece);
        }
    }
    return s;
}

// Function to print the board
void printBoard(const Board& board, ofstream& outFile) {
    for (const auto& row : board) {
        for (const auto& piece : row) {
            if (piece == EMPTY) outFile << "_";
            else if (piece == RED) outFile << "R";
            else if (piece == BLUE) outFile << "B";
            else if (piece == BLOCK) outFile << "#";
        }
        outFile << endl;
    }
    outFile << endl;
}

// Depth-First Search function to solve the puzzle
bool dfs(State& state, unordered_set<string>& visited, ofstream& outFile) {
    // Debug: Print the current board state
    cout << "Current board state:\n";
    for (const auto& row : state.board) {
        for (const auto& piece : row) {
            if (piece == EMPTY) cout << "_";
            else if (piece == RED) cout << "R";
            else if (piece == BLUE) cout << "B";
            else if (piece == BLOCK) cout << "#";
        }
        cout << endl;
    }
    cout << endl;

    if (isGoalState(state.board)) {
        // Write the final arrangement of the board
        outFile << "Final arrangement of the board:\n";
        printBoard(state.board, outFile);

        // Write the solution to the output file
        outFile << "Solution found in " << state.moves.size() << " steps:\n";
        for (const auto& move : state.moves) {
            outFile << "Move from (" << move.x1 << "," << move.y1 << ") to (" << move.x2 << "," << move.y2 << ")\n";
        }
        return true;
    }

    string boardStr = boardToString(state.board);
    if (visited.find(boardStr) != visited.end()) {
        // Debug: Print if the state is already visited
        cout << "State already visited. Backtracking...\n";
        return false;
    }
    visited.insert(boardStr);

    vector<Move> moves = generateMoves(state.board);
    // Debug: Print the number of possible moves
    cout << "Number of possible moves: " << moves.size() << endl;

    for (const auto& move : moves) {
        // Debug: Print the move being explored
        cout << "Exploring move from (" << move.x1 << "," << move.y1 << ") to (" << move.x2 << "," << move.y2 << ")\n";

        State newState;
        newState.board = applyMove(state.board, move);
        newState.moves = state.moves;
        newState.moves.push_back(move);

        if (dfs(newState, visited, outFile)) {
            return true;
        }
    }

    // Debug: Print if no solution is found from this state
    cout << "No solution found from this state. Backtracking...\n";
    return false;
}

int main() {
    cout << "Enter board size (5 or 7): ";
    cin >> BOARD_SIZE;

    if (BOARD_SIZE != 5 && BOARD_SIZE != 7) {
        cerr << "Invalid board size. Please enter 5 or 7." << endl;
        return 1;
    }

    // Initialize the board
    Board board(BOARD_SIZE, vector<Piece>(BOARD_SIZE, BLOCK));
    if (BOARD_SIZE == 5) {
        // 5x5 initial arrangement
        board = {
            {RED, RED, RED, BLOCK, BLOCK},
            {RED, RED, RED, BLOCK, BLOCK},
            {RED, RED, EMPTY, BLUE, BLUE},
            {BLOCK, BLOCK, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLUE, BLUE, BLUE}
        };
    } else if (BOARD_SIZE == 7) {
        // 7x7 initial arrangement
        board = {
            {RED, RED, RED, RED, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, RED, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, RED, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, EMPTY, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE}
        };
    }

    // Create the initial state
    State initialState;
    initialState.board = board;

    // Set to keep track of visited states
    unordered_set<string> visited;

    // Open the output file
    string fileName = "DFS" + to_string(BOARD_SIZE) + ".out";
    ofstream outFile(fileName);

    if (!outFile) {
        cerr << "Error opening output file." << endl;
        return 1;
    }

    // Print the initial arrangement of the board
    outFile << "Initial arrangement of the board:\n";
    printBoard(initialState.board, outFile);

    // Solve the puzzle using DFS
    if (dfs(initialState, visited, outFile)) {
        cout << "Solution found and written to " << fileName << endl;
    } else {
        cout << "No solution found." << endl;
    }

    outFile.close();
    return 0;
}
