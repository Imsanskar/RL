#include <inttypes.h>
#include <unordered_map>
#include <vector>
#include <math.h>
#include <iostream>
#include <iostream>
#include <pthread.h>

//TODO: FIX for O case game
typedef std::unordered_map<uint64_t, float> ValueTable;
#define EPSILON 0.1

enum State{
    DRAW,
    X = 1, 
    O, 
    GAME_RUNNING
};

const float learning_rate = 0.1;

struct Board {
    uint8_t board[9] = {0};

    Board() = default;
    Board(int _board[9]) {
        for(int i = 0; i < 9; i++) {
            board[i] = _board[i];
        }
    }

    inline void move(uint8_t position, uint8_t marker) {
        board[position] = marker;
    }

    std::vector<uint8_t> get_possible_moves() {
        std::vector <uint8_t> moves;
        for (int i = 0; i < 9; i++) {
            if (board[i] == 0) {
                moves.push_back(i);
            }
        } 

        return moves;
    }

    State is_game_over() const{
        if ((board[0] == board[4] && board[0] == board[8]) && (board[0] != 0)) {
            return (State) board[0];
        }
        if ((board[2] == board[6] && board[2] == board[4]) and board[2] != 0)
            return (State) board[2];

        for (int i = 0; i < 3; i++) {
            if ((board[3 * i] == board[3 * i + 1] && board[3 * i] == board[3 * i + 2]) && board[3 * i] != 0)
                return (State) board[3 * i];
            if ((board[i % 3] == board[i % 3 + 3] && board[i % 3] == board[i % 3 + 6]) && board[i % 3] != 0)
                return (State) board[i % 3];
        }

        bool is_all_filled = true;
        for (int i = 0; i < 9; i++) {
            if (!board[i]) {
                return GAME_RUNNING;
            }
        }
        return DRAW; 
    }

    Board copy() {
        Board b;
        for (int i = 0; i < 9; i++) {
            b.board[i] = board[i];
        }
        return b;
    }

    bool operator == (Board b) {
        for (int i = 0; i < 9; i++) {
            if (board[i] != b.board[i]) 
                return false;
        }
        return true;
    }

    uint64_t encode() const {
        uint64_t value = 0;
        for(int i = 0; i < 9; i++) {
            value |= ((uint64_t)board[i]) << (i * 2);
        }

        return value;
    }

    void decode(uint64_t value) {
        for(int i = 0; i < 9; i++) {
            board[i] = (value >> (i * 2)) & 0b11;
        }
    }

    void print() const {
        printf("%d | %d | %d \n", board[0], board[1], board[2]);
        printf("%d | %d | %d \n", board[3], board[4], board[5]);
        printf("%d | %d | %d \n", board[6], board[7], board[8]);
    }
};

uint8_t get_max_move(Board board, ValueTable &value_table, const uint8_t marker = X) {
    auto moves = board.get_possible_moves();
    uint8_t best_move = moves[0];
    const int size = moves.size();
    if (marker == X) {
        float max_score = -100.0f;
        for (int i = 0; i < size; i++) {
            board.move(moves[i], marker);
            if (value_table[board.encode()] >= max_score) {
                best_move = moves[i];
                max_score = value_table[board.encode()];
            }
            // undo the move done by temporary state
            board.move(moves[i], 0);
        }
    } else {
        float min_score = 1000.0f;
        for (int i = 0; i < size; i++) {
            board.move(moves[i], marker);
            if (value_table[board.encode()] <= min_score) {
                best_move = moves[i];
                min_score = value_table[board.encode()];
            }
            board.move(moves[i], 0);
        }
    }

    return best_move;
}

#define ITERATIONS 1000000

ValueTable train(float epsilon) {
    ValueTable value_table;

    for(int i = 0; i < ITERATIONS; i++) {
        Board board;
        // uint8_t turn = (float)rand() / (float)RAND_MAX > 0.5?0:1;
        uint8_t turn = 0;
        while (true) {
            turn = turn % 2;
            const uint64_t prev_state = board.encode();
            int marker = turn == 0? X: O;
            auto moves = board.get_possible_moves();
            float random = (float)rand() / (float)RAND_MAX;
            uint8_t move = random < EPSILON? moves[rand() % moves.size()]: get_max_move(board.copy(), value_table, marker);
            //TODO: Exploration, exploitation
            board.move(move, marker);
            auto winner = board.is_game_over();
            const uint64_t next_state = board.encode();
            if (winner == X) {
                value_table[next_state] =  1.0f;
            } else if (winner == O) {
                value_table[next_state] =  0.0f;
            } else if (winner == DRAW) {
                value_table[next_state] =  0.5f;
            }
            value_table.try_emplace(prev_state, 0.5f);
            value_table.try_emplace(next_state, 0.5f);
            
            value_table[prev_state] +=  learning_rate * (value_table[next_state] - value_table[prev_state]);

            turn += 1;
            if (winner != GAME_RUNNING) {
                break;
            }
        }
    }

    return value_table;
}

void game(ValueTable value_table) {
    Board b;
    uint8_t marker;
    bool is_game_over = false;
    bool is_x_turn = false;
    while (!is_game_over) {
        if (is_x_turn) {
            marker = X;
            printf("X turn\n");
            Board board = b.copy();
            auto max_move = get_max_move(board, value_table, marker);
            b.move(max_move, X);
        } else {
            printf("O turn\n");
            marker = O;
            bool is_valid_input = false;
            while (!is_valid_input) {
                uint8_t position;
                printf("Input position:");
                std::cin >> position;
                position = position - '0';
                if (position > 8) {
                    printf("Out of bounds error, input: %d\n", position);
                } else if (b.board[position] != 0) {
                    printf("Position already ocupied\n");
                } else {
                    b.move(position, O);
                    b.print();
                    is_valid_input = true;
                }
            }
        }
        is_x_turn = !is_x_turn;
        b.print();
        printf("\n--------------------------------\n");
        auto winner = b.is_game_over();
        is_game_over= true;
        if (winner == X) {
            printf("Yes, AI wins\n");
        } else if (winner == O) {
            printf("Noooo, AI looses\n");
        } else if (winner == DRAW) {
            printf("Draw\n");
        } else {
            is_game_over = false;
        }
    }
}

void log_value_table(const ValueTable &value_table) {
    FILE *file = fopen("log.csv", "w");
    for (const auto& key_value: value_table) {
        fprintf(file, "%d, %f\n", key_value.first, key_value.second);
    }

    fclose(file);
}

int main() {
    auto value_table = train(0.1f);
    log_value_table(value_table);
    game(value_table);
}