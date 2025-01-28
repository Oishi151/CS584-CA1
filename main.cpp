// Fore & Aft Puzzle Solver using A* Search
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <fstream>
#include <chrono>
#include <cmath>

using namespace std;

// Board representation
struct Board {
    vector<vector<char>> grid; // The board grid
    int cost; // Cost to reach this state
    int heuristic; // Heuristic value
    int x, y; // Empty space position (if relevant)

    // Custom comparator for priority queue
    bool operator<(const Board& other) const {
        return (cost + heuristic) > (other.cost + other.heuristic);
    }
};

// Function prototypes
vector<vector<char>> generateGoalState(int size);
int calculateHeuristic(const vector<vector<char>>& grid, const vector<vector<char>>& goal);
vector<Board> getNeighbors(const Board& current, const vector<vector<char>>& goal);
bool isGoalState(const vector<vector<char>>& grid, const vector<vector<char>>& goal);
void printBoard(const vector<vector<char>>& grid);

// Main function
int main() {
    int boardSize;
    cout << "Enter the board size (5, 7, 9, or 11): ";
    cin >> boardSize;

    if (boardSize != 5 && boardSize != 7 && boardSize != 9 && boardSize != 11) {
        cerr << "Invalid board size! Exiting...\n";
        return 1;
    }

    // Initialize the board
    vector<vector<char>> startState(boardSize, vector<char>(boardSize, '.'));
    vector<vector<char>> goalState = generateGoalState(boardSize);

    // Fill the board with initial pieces (example setup)
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize / 2; ++j) {
            startState[i][j] = 'R'; // Red pieces
            startState[i][boardSize - j - 1] = 'B'; // Blue pieces
        }
    }

    // Solve using A* Search
    auto startTime = chrono::high_resolution_clock::now();

    priority_queue<Board> openList;
    unordered_set<string> visited;

    Board start = {startState, 0, calculateHeuristic(startState, goalState), 0, 0};
    openList.push(start);

    while (!openList.empty()) {
        Board current = openList.top();
        openList.pop();

        if (isGoalState(current.grid, goalState)) {
            auto endTime = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = endTime - startTime;

            cout << "Solution found in " << duration.count() << " seconds.\n";
            printBoard(current.grid);

            // Save to file
            ofstream outFile("A*" + to_string(boardSize) + ".out");
            if (outFile.is_open()) {
                for (const auto& row : current.grid) {
                    for (char cell : row) {
                        outFile << cell << ' ';
                    }
                    outFile << '\n';
                }
                outFile.close();
            }

            return 0;
        }

        // Generate neighbors and add to open list
        for (const Board& neighbor : getNeighbors(current, goalState)) {
            // Serialize board state to check if visited
            string serialized;
            for (const auto& row : neighbor.grid) {
                serialized += string(row.begin(), row.end());
            }

            if (visited.find(serialized) == visited.end()) {
                openList.push(neighbor);
                visited.insert(serialized);
            }
        }
    }

    cout << "No solution found within the time limit." << endl;
    return 1;
}

// Generate the goal state for the puzzle
vector<vector<char>> generateGoalState(int size) {
    vector<vector<char>> goal(size, vector<char>(size, '.'));
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size / 2; ++j) {
            goal[i][j] = 'B'; // Blue pieces on the left
            goal[i][size - j - 1] = 'R'; // Red pieces on the right
        }
    }
    return goal;
}

// Calculate the heuristic (e.g., Manhattan distance)
int calculateHeuristic(const vector<vector<char>>& grid, const vector<vector<char>>& goal) {
    int heuristic = 0;
    for (size_t i = 0; i < grid.size(); ++i) {
        for (size_t j = 0; j < grid[i].size(); ++j) {
            if (grid[i][j] != '.' && grid[i][j] != goal[i][j]) {
                heuristic += 1; // Simplified heuristic
            }
        }
    }
    return heuristic;
}

// Generate neighbors for the current board state
vector<Board> getNeighbors(const Board& current, const vector<vector<char>>& goal) {
    vector<Board> neighbors;
    // Implement logic to generate valid neighbor states by swapping pieces
    return neighbors;
}

// Check if the current state is the goal state
bool isGoalState(const vector<vector<char>>& grid, const vector<vector<char>>& goal) {
    return grid == goal;
}

// Print the board to the console
void printBoard(const vector<vector<char>>& grid) {
    for (const auto& row : grid) {
        for (char cell : row) {
            cout << cell << ' ';
        }
        cout << '\n';
    }
}
