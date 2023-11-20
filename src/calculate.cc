#include "board.h"
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <fstream>
#include <filesystem>
#include <numeric>
#include <limits>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "usage: calculation <path-to-found_states_dir> <path-to-value-saving_dir>\n";
        return -1;
    }

    std::string found_states_result_dir = argv[1];
    if (!std::filesystem::exists(found_states_result_dir)) {
        std::cerr << found_states_result_dir + "not exist\n";
        return -1;
    }

    std::string value_result_dir = argv[2];
    if (!std::filesystem::create_directory(value_result_dir) && !std::filesystem::exists(value_result_dir)) {
        std::cerr << "creating directory " + value_result_dir + " failed\n";
        return -1;
    }

    Board::init();

    std::unordered_map<uint64_t, double> state_values;
    std::unordered_map<uint64_t, double> afterstate_values;

    std::ofstream ofs_state_values_file(value_result_dir + "state_values.bin", std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);
    if (ofs_state_values_file.fail()) {
        std::cerr << "File open error: " << value_result_dir + "state_values.bin\n";
        return -1;
    }
    
    std::ofstream ofs_afterstate_values_file(value_result_dir + "afterstate_values.bin", std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);
    if (ofs_afterstate_values_file.fail()) {
        std::cerr << "File open error: " << value_result_dir + "afterstate_values.bin\n";
        return -1;
    }

    const int max_board_sum = 2044;
    const int min_board_sum = 4;

    for (int current_board_sum = max_board_sum; current_board_sum >= min_board_sum; current_board_sum -= 2) {
        std::ifstream ifs_found_states(found_states_result_dir + "states/found_states_" + std::to_string(current_board_sum) + ".bin", std::ios_base::in|std::ios_base::binary);
        if (ifs_found_states.fail()) {
            std::cerr << "File open error: " << found_states_result_dir + "states/found_states_" + std::to_string(current_board_sum) + ".bin\n";
            return -1;
        }

        ifs_found_states.seekg(0, std::ios::end);
        const auto end_pos = ifs_found_states.tellg();
        ifs_found_states.seekg(0, std::ios::beg);

        while (ifs_found_states.tellg() != end_pos) {
            uint64_t normalized_state_key;
            ifs_found_states.read(reinterpret_cast<char*>(&normalized_state_key), sizeof(uint64_t));

            Board state_board(0);
            state_board.set(normalized_state_key);

            if (state_board.is_gameover()) {
                state_values[normalized_state_key] = 0;
                ofs_state_values_file.write(reinterpret_cast<const char*>(&normalized_state_key), sizeof(uint64_t));
                ofs_state_values_file.write(reinterpret_cast<const char*>(&state_values[normalized_state_key]), sizeof(double));
                continue;
            }

            double max_q_value = std::numeric_limits<double>::lowest();

            Board afterstate_board(0);
            for (Action action : state_board.legal_actions()) {
                afterstate_board.set(normalized_state_key);
                const double reward = afterstate_board.do_action(action);
                const uint64_t normalized_afterstate_key = afterstate_board.normalize();

                if (afterstate_values.find(normalized_afterstate_key) != afterstate_values.end()) {
                    max_q_value = std::max(max_q_value, reward + afterstate_values[normalized_afterstate_key]);
                    continue;
                }

                afterstate_board.set(normalized_afterstate_key);
                const std::vector<Square> empty_squares = afterstate_board.empty_squares();
                const size_t empty_square_num = empty_squares.size();
                std::vector<double> next_state_values;
                std::vector<double> next_state_probs;

                Board next_state_board(0);
                for (size_t empty_sq_i = 0; empty_sq_i < empty_square_num; ++empty_sq_i) {
                    const Square sq = empty_squares[empty_sq_i];

                    for (int spawn_num = 1; spawn_num <= 2; ++spawn_num) {
                        next_state_board.set(normalized_afterstate_key);
                        next_state_board.spawn(sq, spawn_num);
                        const uint64_t normalized_next_state_key = next_state_board.normalize();

                        if (state_values.find(normalized_next_state_key) != state_values.end()) {
                            next_state_values.emplace_back(state_values[normalized_next_state_key]);
                            if (spawn_num == 1) {
                                next_state_probs.emplace_back(0.9 / (double)empty_square_num);
                            } else {
                                next_state_probs.emplace_back(0.1 / (double)empty_square_num);
                            }
                        } else {
                            std::cerr << "not calculated state!\n";
                            return -1;
                        }
                    }
                }

                const double afterstate_value = std::inner_product(next_state_values.begin(), next_state_values.end(), next_state_probs.begin(), 0.0);
                afterstate_values[normalized_afterstate_key] = afterstate_value;
                ofs_afterstate_values_file.write(reinterpret_cast<const char*>(&normalized_afterstate_key), sizeof(uint64_t));
                ofs_afterstate_values_file.write(reinterpret_cast<const char*>(&afterstate_value), sizeof(double));
                max_q_value = std::max(max_q_value, reward + afterstate_value);
            }

            state_values[normalized_state_key] = max_q_value;
            ofs_state_values_file.write(reinterpret_cast<const char*>(&normalized_state_key), sizeof(uint64_t));
            ofs_state_values_file.write(reinterpret_cast<const char*>(&max_q_value), sizeof(double));
        }

        if (ifs_found_states.fail()) {
            std::cerr << "File read error: " << found_states_result_dir + "states/found_states_" + std::to_string(current_board_sum) + ".bin\n";
            return -1;
        }
    }

    return 0;
}