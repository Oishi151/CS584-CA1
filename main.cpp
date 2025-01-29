#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <fstream>
using namespace std;

struct State {
    vector<vector<char>> board;
    int g, h;
    State* parent;

    State(vector<vector<char>> b, int g, int h, State* p) : board(b), g(g), h(h), parent(p) {}

    int f() const { return g + h; }
};

struct CompareState {
    bool operator()(const State* a, const State* b) {
        return a->f() > b->f();
    }
};

int heuristic(const vector<vector<char>>& board) {
    int h = 0;
    int n = board.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] == 'R') h += (n - 1 - i);
            else if (board[i][j] == 'B') h += i;
        }
    }
    return h;
}

vector<State*> get_neighbors(State* current) {
    vector<State*> neighbors;
    int n = current->board.size();
    vector<vector<char>> board = current->board;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] == 'R' && i + 1 < n && board[i + 1][j] == '.') {
                swap(board[i][j], board[i + 1][j]);
                neighbors.push_back(new State(board, current->g + 1, heuristic(board), current));
                swap(board[i][j], board[i + 1][j]);
            }
            if (board[i][j] == 'B' && i - 1 >= 0 && board[i - 1][j] == '.') {
                swap(board[i][j], board[i - 1][j]);
                neighbors.push_back(new State(board, current->g + 1, heuristic(board), current));
                swap(board[i][j], board[i - 1][j]);
            }
        }
    }
    return neighbors;
}

bool is_goal(State* state) {
    int n = state->board.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if ((i < n / 2 && state->board[i][j] != 'B') || (i > n / 2 && state->board[i][j] != 'R'))
                return false;
        }
    }
    return true;
}

void save_solution(State* final_state, int board_size) {
    string filename = "A*" + to_string(board_size);
    ofstream file(filename);
    
    vector<State*> path;
    while (final_state) {
        path.push_back(final_state);
        final_state = final_state->parent;
    }
    
    int step = 0;
    for (auto it = path.rbegin(); it != path.rend(); ++it) {
        file << step++ << ":\n";
        for (const auto& row : (*it)->board) {
            for (char c : row) file << c;
            file << "\n";
        }
        file << "\n";
    }
    file.close();
}

void a_star_search(vector<vector<char>> start_board, int board_size) {
    priority_queue<State*, vector<State*>, CompareState> open_list;
    open_list.push(new State(start_board, 0, heuristic(start_board), nullptr));
    
    while (!open_list.empty()) {
        State* current = open_list.top();
        open_list.pop();
        
        if (is_goal(current)) {
            save_solution(current, board_size);
            cout << "Solution saved in A*" << board_size << endl;
            return;
        }
        
        vector<State*> neighbors = get_neighbors(current);
        for (State* neighbor : neighbors) open_list.push(neighbor);
    }
    
    cout << "No solution found." << endl;
}

int main() {
    int n;
    cout << "Enter board size (5, 7, 9, 11): ";
    cin >> n;
    if (n < 5 || n > 11 || n % 2 == 0) {
        cout << "Invalid board size. Choose 5, 7, 9, or 11." << endl;
        return 0;
    }
    
    vector<vector<char>> board(n, vector<char>(n, '.'));
    for (int i = 0; i < 8; i++) {
        board[i / (n / 2)][i % (n / 2)] = 'R';
        board[n - 1 - (i / (n / 2))][n - 1 - (i % (n / 2))] = 'B';
    }
    
    a_star_search(board, n);
    return 0;
}
