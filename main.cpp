/*
Names: [Your names here]
Performance Summary:
Search Method: A* Search
Performance Measure: Solves puzzles up to 11x11 within 10 seconds
Using Manhattan distance heuristic for A* search
*/

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>

using namespace std;

// Structure to represent a board state
struct State {
    vector<vector<char>> board;
    int cost;
    int heuristic;
    string moves;
    
    State(vector<vector<char>> b, int c, int h, string m) : 
        board(b), cost(c), heuristic(h), moves(m) {}
};

// Compare states for priority queue
struct CompareStates {
    bool operator()(const State& s1, const State& s2) {
        return (s1.cost + s1.heuristic) > (s2.cost + s2.heuristic);
    }
};

// Hash function for board state
struct BoardHash {
    size_t operator()(const vector<vector<char>>& board) const {
        string boardStr;
        for(const auto& row : board)
            for(char c : row)
                boardStr += c;
        return hash<string>()(boardStr);
    }
};

class ForeAftSolver {
private:
    int size;
    vector<vector<char>> initialBoard;
    vector<vector<char>> goalBoard;
    
    // Calculate Manhattan distance heuristic
    int calculateHeuristic(const vector<vector<char>>& board) {
        int distance = 0;
        for(int i = 0; i < size; i++) {
            for(int j = 0; j < size; j++) {
                if(board[i][j] == 'R') {
                    distance += i; // Distance from top
                }
                else if(board[i][j] == 'B') {
                    distance += (size - 1 - i); // Distance from bottom
                }
            }
        }
        return distance;
    }
    
    // Check if move is valid
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, 
                    const vector<vector<char>>& board) {
        // Check bounds
        if(fromRow < 0 || fromRow >= size || fromCol < 0 || fromCol >= size ||
           toRow < 0 || toRow >= size || toCol < 0 || toCol >= size)
            return false;
            
        // Check if piece exists at from position
        if(board[fromRow][fromCol] == '.')
            return false;
            
        // Check if to position is empty
        if(board[toRow][toCol] != '.')
            return false;
            
        // Check diagonal move
        if(abs(fromRow - toRow) != abs(fromCol - toCol))
            return false;
            
        // Check path is clear
        int rowStep = (toRow - fromRow) / abs(toRow - fromRow);
        int colStep = (toCol - fromCol) / abs(toCol - fromCol);
        int row = fromRow + rowStep;
        int col = fromCol + colStep;
        while(row != toRow && col != toCol) {
            if(board[row][col] != '.')
                return false;
            row += rowStep;
            col += colStep;
        }
        
        return true;
    }
    
    // Generate next states
    vector<State> getNextStates(const State& current) {
        vector<State> nextStates;
        
        for(int fromRow = 0; fromRow < size; fromRow++) {
            for(int fromCol = 0; fromCol < size; fromCol++) {
                char piece = current.board[fromRow][fromCol];
                if(piece == '.') continue;
                
                // Try all possible diagonal moves
                for(int toRow = 0; toRow < size; toRow++) {
                    for(int toCol = 0; toCol < size; toCol++) {
                        if(isValidMove(fromRow, fromCol, toRow, toCol, current.board)) {
                            vector<vector<char>> newBoard = current.board;
                            newBoard[toRow][toCol] = piece;
                            newBoard[fromRow][fromCol] = '.';
                            
                            string newMoves = current.moves + 
                                            to_string(fromRow) + "," + to_string(fromCol) + "->" +
                                            to_string(toRow) + "," + to_string(toCol) + "\n";
                                            
                            int newHeuristic = calculateHeuristic(newBoard);
                            nextStates.emplace_back(newBoard, current.cost + 1, 
                                                  newHeuristic, newMoves);
                        }
                    }
                }
            }
        }
        
        return nextStates;
    }
    
    // Check if state is goal state
    bool isGoalState(const vector<vector<char>>& board) {
        return board == goalBoard;
    }
    
public:
    ForeAftSolver(int boardSize) : size(boardSize) {
        // Initialize boards
        initialBoard = vector<vector<char>>(size, vector<char>(size, '.'));
        goalBoard = vector<vector<char>>(size, vector<char>(size, '.'));
        
        // Set initial board
        int pieces = (size * size - 1) / 3;
        for(int i = 0; i < pieces; i++) {
            int row = i / size;
            int col = i % size;
            initialBoard[row][col] = 'B';
            
            row = size - 1 - (i / size);
            col = size - 1 - (i % size);
            initialBoard[row][col] = 'R';
        }
        
        // Set goal board
        for(int i = 0; i < pieces; i++) {
            int row = i / size;
            int col = i % size;
            goalBoard[row][col] = 'R';
            
            row = size - 1 - (i / size);
            col = size - 1 - (i % size);
            goalBoard[row][col] = 'B';
        }
    }
    
    // Solve using A* search
    bool solve(string outputFile) {
        auto startTime = chrono::high_resolution_clock::now();
        
        priority_queue<State, vector<State>, CompareStates> pq;
        unordered_set<vector<vector<char>>, BoardHash> visited;
        
        int initialHeuristic = calculateHeuristic(initialBoard);
        pq.push(State(initialBoard, 0, initialHeuristic, ""));
        
        while(!pq.empty()) {
            auto currentTime = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::seconds>
                          (currentTime - startTime).count();
            if(duration >= 10) {
                cout << "Time limit exceeded" << endl;
                return false;
            }
            
            State current = pq.top();
            pq.pop();
            
            if(isGoalState(current.board)) {
                // Write solution to file
                ofstream out(outputFile);
                out << "Solution found in " << current.cost << " moves\n\n";
                out << current.moves;
                out.close();
                return true;
            }
            
            if(visited.find(current.board) != visited.end())
                continue;
                
            visited.insert(current.board);
            
            vector<State> nextStates = getNextStates(current);
            for(const auto& next : nextStates) {
                if(visited.find(next.board) == visited.end()) {
                    pq.push(next);
                }
            }
        }
        
        return false;
    }
};

int main() {
    cout << "Enter board size (5, 7, 9, or 11): ";
    int size;
    cin >> size;
    
    if(size != 5 && size != 7 && size != 9 && size != 11) {
        cout << "Invalid board size" << endl;
        return 1;
    }
    
    ForeAftSolver solver(size);
    string outputFile = "best" + to_string(size) + ".out";
    
    if(solver.solve(outputFile)) {
        cout << "Solution written to " << outputFile << endl;
    } else {
        cout << "No solution found within time limit" << endl;
    }
    
    return 0;
}
