#include <stdio.h>
#include <inttypes.h>

#define ROW 5
#define COL 5 

double grid[5][5] = {0.0};

struct Position {
    int row = 0, col = 0;
    Position operator -(Position p) {
        return {row - p.row, col - p.col};
    }

    Position operator +(Position p) {
        return {row + p.row, col + p.col};
    }
};

template <class T1, class T2>
struct Pair {
    T1 first;
    T2 second;

    Pair() = default;
    Pair(T1 x, T2 y): first(x), second(y) {}
    
};
typedef Pair<Position, double> Reward;

enum Move {
    LEFT,
    TOP,
    RIGHT, 
    DOWN 
};

#define abs(x) x > 0? x: -x

Pair<Position, double> move(Position current_pos, Move move) {
    if (current_pos.row == 0 && current_pos.col == 1) {
        return Reward({4, 1}, 10.0);
    }
    if (current_pos.row == 0 && current_pos.col == 3) {
        return Reward({2, 3}, 5.0);
    }

    if (current_pos.row == 0 && move == TOP) {
        return Reward(current_pos, -1.0);
    } else if (current_pos.row == 4 && move == DOWN) {
        return Reward(current_pos, -1.0);
    } else if (current_pos.col == 0 && move == LEFT) {
        return Reward(current_pos, -1.0);
    } else if (current_pos.col == 4 && move == RIGHT) {
        return Reward(current_pos, -1.0);
    }
    
    Position p;
    if (move == RIGHT) {
        p.col = 1;
    } else if (move == LEFT) {
        p.col = -1;
    } else if (move == RIGHT) {
        p.row = 1;
    } else if (move == LEFT) {
        p.row = -1;
    }
    return Reward(current_pos + p, 0.0);
}

void log() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (j == 4) {
                printf("%.2f\n", grid[i][j]);
            } else {
                printf("%.2f | ", grid[i][j]);
            }
        }
    }
}

void copy(double src[5][5], double dest[5][5]) {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < ROW; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

#define GAMMA 0.9
void solve_bellman(uint64_t iterations) {
    for (uint64_t itr = 0; itr < iterations; itr++) {
        double grid_temp[ROW][COL] = {0.0}; 
        for(int row = 0; row < ROW; row++) {
            Position current_pos;
            current_pos.row = row;
            for(int col = 0; col < COL; col++) {
                current_pos.col = col;
                double update = 0.0f;
                for (int i = 0; i < 4; i++) {
                    auto reward = move(current_pos, (Move)i);
                    update += 0.25 * (reward.second + GAMMA * grid[reward.first.row][reward.first.col]);
                }

                grid[row][col] = update;
            }   
        }
    }
}

int main() {
    solve_bellman(10000);
    log();
}
