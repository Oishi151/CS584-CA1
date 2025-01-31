#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <string>
#include <unordered_map>
#include <climits>

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
    int g; // Cost from the start node
    int h; // Heuristic cost to goal
    int f; // Total estimated cost (g + h)
    vector<Move> moves;
    // Comparator for the priority queue
    bool operator>(const State& other) const {
        return f > other.f;
    }
};

// Function to find the position of the empty space
pair<int, int> findEmpty(const Board& board) {
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            if (board[i][j] == EMPTY)
                return {i, j};
    return {-1, -1}; // Should never happen
}

// Function to check if two boards are equal
bool boardsEqual(const Board& a, const Board& b) {
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            if (a[i][j] != b[i][j])
                return false;
    return true;
}

// Function to check if the board is in the goal state
bool isGoalState(const Board& board, const Board& goalBoard, const pair<int, int>& initialEmptyPos) {
    // Check if boards are equal and empty space is back to initial position
    return boardsEqual(board, goalBoard) && findEmpty(board) == initialEmptyPos;
}

// Function to get the goal positions of RED and BLUE pieces
unordered_map<Piece, vector<pair<int, int>>> getGoalPositions(const Board& goalBoard) {
    unordered_map<Piece, vector<pair<int, int>>> goalPositions;
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (goalBoard[i][j] == RED || goalBoard[i][j] == BLUE)
                goalPositions[goalBoard[i][j]].push_back({i, j});
        }
    return goalPositions;
}

// Function to calculate heuristic cost
int heuristic(const Board& board, const unordered_map<Piece, vector<pair<int, int>>>& goalPositions) {
    int cost = 0;
    // Keep track of assigned goal positions
    unordered_map<Piece, vector<bool>> assignedPositions;
    for (const auto& entry : goalPositions) {
        assignedPositions[entry.first] = vector<bool>(entry.second.size(), false);
    }
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            Piece piece = board[i][j];
            if (piece == RED || piece == BLUE) {
                const auto& positions = goalPositions.at(piece);
                auto& assigned = assignedPositions[piece];
                int minDistance = INT_MAX;
                int minIndex = -1;
                for (size_t k = 0; k < positions.size(); ++k) {
                    if (!assigned[k]) {
                        int dx = abs(i - positions[k].first);
                        int dy = abs(j - positions[k].second);
                        int distance = dx + dy;
                        if (distance < minDistance) {
                            minDistance = distance;
                            minIndex = k;
                        }
                    }
                }
                if (minIndex != -1) {
                    cost += minDistance;
                    assigned[minIndex] = true;
                }
            }
        }
    }
    return cost;
}

// Function to generate all possible moves from the current state
vector<Move> generateMoves(const Board& board) {
    vector<Move> moves;
    int dxR[] = {1, 0};       // Red moves: Down, Right
    int dyR[] = {0, 1};
    int dxB[] = {-1, 0};      // Blue moves: Up, Left
    int dyB[] = {0, -1};
    int nDirections = 2;

    for (int x = 0; x < BOARD_SIZE; ++x) {
        for (int y = 0; y < BOARD_SIZE; ++y) {
            Piece piece = board[x][y];
            if (piece == RED) {
                for (int d = 0; d < nDirections; ++d) {
                    int nx = x + dxR[d];
                    int ny = y + dyR[d];
                    // Slide move
                    if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
                        board[nx][ny] == EMPTY) {
                        moves.push_back({x, y, nx, ny});
                    }
                    // Jump move over BLUE piece only
                    int jx = x + 2 * dxR[d];
                    int jy = y + 2 * dyR[d];
                    if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
                        board[nx][ny] == BLUE &&
                        jx >= 0 && jx < BOARD_SIZE && jy >= 0 && jy < BOARD_SIZE &&
                        board[jx][jy] == EMPTY) {
                        moves.push_back({x, y, jx, jy});
                    }
                }
            } else if (piece == BLUE) {
                for (int d = 0; d < nDirections; ++d) {
                    int nx = x + dxB[d];
                    int ny = y + dyB[d];
                    // Slide move
                    if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
                        board[nx][ny] == EMPTY) {
                        moves.push_back({x, y, nx, ny});
                    }
                    // Jump move over RED piece only
                    int jx = x + 2 * dxB[d];
                    int jy = y + 2 * dyB[d];
                    if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE &&
                        board[nx][ny] == RED &&
                        jx >= 0 && jx < BOARD_SIZE && jy >= 0 && jy < BOARD_SIZE &&
                        board[jx][jy] == EMPTY) {
                        moves.push_back({x, y, jx, jy});
                    }
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

// A* Search function to solve the puzzle
bool aStar(const Board& initialBoard, const Board& goalBoard, const pair<int, int>& initialEmptyPos, ofstream& outFile) {
    unordered_set<string> closedSet;
    priority_queue<State, vector<State>, greater<State>> openSet;

    auto goalPositions = getGoalPositions(goalBoard);

    int h = heuristic(initialBoard, goalPositions);

    State initialState = {initialBoard, 0, h, h, {}};
    openSet.push(initialState);

    unordered_map<string, int> gScores; // Map of board string to g(n)
    gScores[boardToString(initialBoard)] = 0;

    while (!openSet.empty()) {
        State current = openSet.top();
        openSet.pop();

        string boardStr = boardToString(current.board);

        if (closedSet.count(boardStr))
            continue;

        closedSet.insert(boardStr);

        pair<int, int> currentEmptyPos = findEmpty(current.board);

        if (currentEmptyPos == initialEmptyPos && boardsEqual(current.board, goalBoard)) {
            // Found the solution
            outFile << "Total number of states: " << current.moves.size() + 1 << "\n\n";

            // Reconstruct the path to print intermediate states
            vector<Board> path;
            Board tempBoard = initialBoard;
            path.push_back(tempBoard);
            for (const auto& move : current.moves) {
                tempBoard = applyMove(tempBoard, move);
                path.push_back(tempBoard);
            }

            // Print each state
            for (size_t i = 0; i < path.size(); ++i) {
                outFile << "State " << i << ":\n";
                printBoard(path[i], outFile);
            }

            outFile << "Final arrangement of the board:\n";
            printBoard(current.board, outFile);

            // Write the solution to the output file
            outFile << "Solution found in " << current.moves.size() << " steps:\n";
            for (const auto& move : current.moves) {
                outFile << "Move from (" << move.x1 << "," << move.y1 << ") to (" << move.x2 << "," << move.y2 << ")\n";
            }
            return true;
        }

        vector<Move> moves = generateMoves(current.board);
        for (const auto& move : moves) {
            Board newBoard = applyMove(current.board, move);
            string newBoardStr = boardToString(newBoard);
            int tentative_gScore = current.g + 1;

            if (closedSet.count(newBoardStr))
                continue;

            if (!gScores.count(newBoardStr) || tentative_gScore < gScores[newBoardStr]) {
                gScores[newBoardStr] = tentative_gScore;
                int h = heuristic(newBoard, goalPositions);
                State neighbor = {newBoard, tentative_gScore, h, tentative_gScore + h, current.moves};
                neighbor.moves.push_back(move);
                openSet.push(neighbor);
            }
        }
    }

    return false;
}

int main() {
    cout << "Enter board size (5, 7, 9, or 11): ";
    cin >> BOARD_SIZE;

    if (BOARD_SIZE != 5 && BOARD_SIZE != 7 && BOARD_SIZE != 9 && BOARD_SIZE != 11) {
        cerr << "Invalid board size. Please enter 5, 7, 9, or 11." << endl;
        return 1;
    }

    // Initialize the initial board and the goal board
    Board initialBoard, goalBoard;
    pair<int, int> initialEmptyPos;

    if (BOARD_SIZE == 5) {
        // 5x5 initial arrangement
        initialBoard = {
            {RED, RED, RED, BLOCK, BLOCK},
            {RED, RED, RED, BLOCK, BLOCK},
            {RED, RED, EMPTY, BLUE, BLUE},
            {BLOCK, BLOCK, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLUE, BLUE, BLUE}
        };
        // Empty space initial position
        initialEmptyPos = {2, 2};

        // 5x5 goal arrangement
        goalBoard = {
            {BLUE, BLUE, BLUE, BLOCK, BLOCK},
            {BLUE, BLUE, BLUE, BLOCK, BLOCK},
            {BLUE, BLUE, EMPTY, RED, RED},
            {BLOCK, BLOCK, RED, RED, RED},
            {BLOCK, BLOCK, RED, RED, RED}
        };
    } else if (BOARD_SIZE == 7) {
        // 7x7 initial arrangement
        initialBoard = {
            {RED, RED, RED, RED, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, RED, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, RED, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, EMPTY, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE}
        };
        // Empty space initial position
        initialEmptyPos = {3, 3};

        // 7x7 goal arrangement
        goalBoard = {
            {BLUE, BLUE, BLUE, BLUE, BLOCK, BLOCK, BLOCK},
            {BLUE, BLUE, BLUE, BLUE, BLOCK, BLOCK, BLOCK},
            {BLUE, BLUE, BLUE, BLUE, BLOCK, BLOCK, BLOCK},
            {BLUE, BLUE, BLUE, EMPTY, RED, RED, RED},
            {BLOCK, BLOCK, BLOCK, RED, RED, RED, RED},
            {BLOCK, BLOCK, BLOCK, RED, RED, RED, RED},
            {BLOCK, BLOCK, BLOCK, RED, RED, RED, RED}
        };
    } else if (BOARD_SIZE == 9) {
        // 9x9 initial arrangement
        initialBoard = {
            {RED, RED, RED, RED, RED, BLOCK, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, RED, RED, BLOCK, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, RED, RED, BLOCK, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, RED, RED, BLOCK, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, RED, EMPTY, BLUE, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE, BLUE}
        };
        // Empty space initial position
        initialEmptyPos = {4, 4};

        // 9x9 goal arrangement
        goalBoard = {
            {BLUE, BLUE, BLUE, BLUE, BLUE, BLOCK, BLOCK, BLOCK, BLOCK},
            {BLUE, BLUE, BLUE, BLUE, BLUE, BLOCK, BLOCK, BLOCK, BLOCK},
            {BLUE, BLUE, BLUE, BLUE, BLUE, BLOCK, BLOCK, BLOCK, BLOCK},
            {BLUE, BLUE, BLUE, BLUE, BLUE, BLOCK, BLOCK, BLOCK, BLOCK},
            {BLUE, BLUE, BLUE, BLUE, EMPTY, RED, RED, RED, RED},
            {BLOCK, BLOCK, BLOCK, BLOCK, RED, RED, RED, RED, RED},
            {BLOCK, BLOCK, BLOCK, BLOCK, RED, RED, RED, RED, RED},
            {BLOCK, BLOCK, BLOCK, BLOCK, RED, RED, RED, RED, RED},
            {BLOCK, BLOCK, BLOCK, BLOCK, RED, RED, RED, RED, RED}
        };
    } else if (BOARD_SIZE == 11) {
        // 11x11 initial arrangement
        initialBoard = {
            {RED, RED, RED, RED, RED, RED, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, RED, RED, RED, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, RED, RED, RED, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, RED, RED, RED, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, RED, RED, RED, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK},
            {RED, RED, RED, RED, RED, EMPTY, BLUE, BLUE, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE},
            {BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE}
        };
        // Empty space initial position
        initialEmptyPos = {5, 5};

        // 11x11 goal arrangement
        goalBoard = {
            {BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK},
            {BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK},
            {BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK},
            {BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK},
            {BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK},
            {BLUE, BLUE, BLUE, BLUE, BLUE, EMPTY, RED, RED, RED, RED, RED},
            {BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, RED, RED, RED, RED, RED, RED},
            {BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, RED, RED, RED, RED, RED, RED},
            {BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, RED, RED, RED, RED, RED, RED},
            {BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, RED, RED, RED, RED, RED, RED},
            {BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, RED, RED, RED, RED, RED, RED}
        };
    }

    // Open the output file
    string fileName = "AStar" + to_string(BOARD_SIZE) + ".out";
    ofstream outFile(fileName);

    if (!outFile) {
        cerr << "Error opening output file." << endl;
        return 1;
    }

    // Solve the puzzle using A* search
    if (aStar(initialBoard, goalBoard, initialEmptyPos, outFile)) {
        cout << "Solution found and written to " << fileName << endl;
    } else {
        cout << "No solution found." << endl;
    }

    outFile.close();
    return 0;
}
