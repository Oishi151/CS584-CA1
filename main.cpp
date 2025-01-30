#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <cmath>

using namespace std;

int boardSize;

// Generate initial board
vector<vector<char>> generateInitialboard(int size) {
    vector<vector<char>> board(size, vector<char>(size, ' '));

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i <= ceil(size / 2) && j <= ceil(size / 2)) {
                board[i][j] = 'R';  // R portion
            }
            else if (i < ceil(size / 2) && j >= ceil(size / 2)) {
                board[i][j] = '#';  // #s in top part
            }
            else if (i > ceil(size / 2) && j < ceil(size / 2)) {
                board[i][j] = '#';  // #s in bottom part
            }
            else if (i >= ceil(size / 2) && j >= ceil(size / 2)) {
                board[i][j] = 'B';  // B portion
            }
        }
    }
    // Empty space in the middle
    board[ceil(size / 2)][ceil(size / 2)] = ' ';

    return board;
}

// Generate goal board
vector<vector<char>> generateGoalboard(int size) {
    vector<vector<char>> board(size, vector<char>(size, ' '));

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i <= ceil(size / 2) && j <= ceil(size / 2)) {
                board[i][j] = 'B';  // B portion
            }
            else if (i < ceil(size / 2) && j >= ceil(size / 2)) {
                board[i][j] = '#';  // #s in top part
            }
            else if (i > ceil(size / 2) && j < ceil(size / 2)) {
                board[i][j] = '#';  // #s in bottom part
            }
            else if (i >= ceil(size / 2) && j >= ceil(size / 2)) {
                board[i][j] = 'R';  // R portion
            }
        }
    }
    // Place empty space in the middle
    board[size / 2][size / 2] = ' ';

    return board;
}

// Check if position is within board boundaries
bool isValid(int x, int y) {
    return x >= 0 && x < boardSize && y >= 0 && y < boardSize;
}

// Define possible moves (up, down, left, right)
const vector<pair<int, int>> MOVES = {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1}
};

// Convert board to a string (for hashing)
string boardToString(const vector<vector<char>>& board) {
    string result;
    for (const auto& row : board)
        for (char cell : row)
            result += cell;
    return result;
}

// Calculate heuristic (Manhattan distance)
int heuristic(const vector<vector<char>>& board, const vector<vector<char>>& goal) {
    int distance = 0;
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (board[i][j] == 'R') {
                distance += abs(i - (boardSize - 1)) + abs(j - (boardSize - 1));
            }
            else if (board[i][j] == 'B') {
                distance += abs(i - 0) + abs(j - 0);
            }
        }
    }
    return distance;
}

void printboard(const vector<vector<char>>& board) {
    for (const auto& row : board) {
        for (char cell : row) {
            cout << cell << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

bool aStarSearch(const vector<vector<char>>& initialboard, const vector<vector<char>>& goalboard) {

    priority_queue<pair<int, vector<vector<char>>>> pq;
    unordered_set<string> visited;

    pq.push({ heuristic(initialboard, goalboard), initialboard });
    visited.insert(boardToString(initialboard));

    while (!pq.empty()) {
        pair<int, vector<vector<char>>> top = pq.top(); // get state with lowest heuristic value  
        int h = top.first;                              // extract heuristic value 
        vector<vector<char>> current = top.second;      // extract current board state 
        pq.pop();                                       // remove this state from priority queue
        printboard(current);

        if (current == goalboard) {
            cout << "Solution found:" << endl;
            printboard(current);
            return true;
        }
        // searches for the empty space
        int emptyX = -1, emptyY = -1;
        for (int i = 0; i < boardSize; ++i) {
            for (int j = 0; j < boardSize; ++j) {
                if (current[i][j] == ' ') {
                    emptyX = i;
                    emptyY = j;
                    break;
                }
            }
            if (emptyX != -1) break;
        }

        for (const auto& move : MOVES) {
            int newX = emptyX + move.first;
            int newY = emptyY + move.second;

            if (isValid(newX, newY) && current[newX][newY] != '#') {
                vector<vector<char>> newboard = current;
                swap(newboard[emptyX][emptyY], newboard[newX][newY]);
                string boardStr = boardToString(newboard);
                if (visited.find(boardStr) == visited.end()) {
                    pq.push({ heuristic(newboard, goalboard), newboard });
                    visited.insert(boardStr);
                }
            }
        }
    }

    cout << "No solution found." << endl;
    return false;
}

int main() {
    cout << "Enter board size (5, 7, 9, 11): ";
    cin >> boardSize;

    if (boardSize < 5 || boardSize > 11 || boardSize % 2 == 0) {
        cout << "Invalid board size! Please enter board size 5, 7, 9, or 11." << endl;
        return 1;
    }

    vector<vector<char>> initialboard = generateInitialboard(boardSize);
    vector<vector<char>> goalboard = generateGoalboard(boardSize);

    cout << "Initial board:" << endl;
    aStarSearch(initialboard, goalboard);

    return 0;
}
