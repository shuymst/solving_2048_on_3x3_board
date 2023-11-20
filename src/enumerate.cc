#include "board.h"
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <filesystem>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "usage: enumeration <path-to-result-saving_dir>\n";
        return -1;
    }

    std::string result_dir = argv[1];
    if (!std::filesystem::create_directory(result_dir) && !std::filesystem::exists(result_dir)) {
        std::cerr << "creating directory " + result_dir + " failed\n";
        return -1;
    }
    if (!std::filesystem::create_directory(result_dir + "states/") && !std::filesystem::exists(result_dir + "states/")) {
        std::cerr << "creating directory " + result_dir + "states/ failed\n";
        return -1;
    }
    if (!std::filesystem::create_directory(result_dir + "afterstates/") && !std::filesystem::exists(result_dir + "afterstates/")) {
        std::cerr << "creating directory " + result_dir + "afterstates/ failed\n";
        return -1;
    }

    std::ofstream ofs_terminal_states(result_dir + "terminal_states.bin", std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);
    if (ofs_terminal_states.fail()) {
        std::cerr << "File open error: " << result_dir + "terminal_states.bin\n";
        return -1;
    }

    std::ofstream ofs_progress_information(result_dir + "enumeration_progress.txt", std::ios_base::out|std::ios_base::trunc);
    if (ofs_progress_information.fail()) {
        std::cerr << "File open error: " << result_dir + "enumeration_progress.txt\n";
        return -1;
    }
    
    Board::init();
    std::unordered_map<int, std::unordered_set<uint64_t>> found_states;
    std::unordered_map<int, std::unordered_set<uint64_t>> found_afterstates;
    std::unordered_set<uint64_t> terminal_states;

    int found_states_num = 0;
    int found_afterstates_num = 0;
    int terminal_states_num = 0;

    for (Square sq_i = SQ_1A; sq_i < SQUARE_NB; ++sq_i) {
        for (Square sq_j = (Square)((int)sq_i+1); sq_j < SQUARE_NB; ++sq_j) {
            for (int num_i = 1; num_i <= 2; ++num_i) {
                for (int num_j = 1; num_j <= 2; ++num_j) {
                    Board initial_board(0);
                    initial_board.spawn(sq_i, num_i);
                    initial_board.spawn(sq_j, num_j);
                    uint64_t normalized_key = initial_board.normalize();
                    int initial_board_sum = initial_board.get_board_sum();
                    
                    if (found_states[initial_board_sum].find(normalized_key) == found_states[initial_board_sum].end()) {
                        found_states[initial_board_sum].insert(normalized_key);
                        found_states_num += 1;
                    }
                }
            }
        }
    }

    int current_board_sum = 4;

    while (true) {
        if (found_states[current_board_sum].size() == 0) {
            break;
        }

        ofs_progress_information << current_board_sum << " " << found_states[current_board_sum].size() << "\n";
        std::cout << current_board_sum << " " << found_states[current_board_sum].size() << "\n";

        std::ofstream ofs_found_states(result_dir + "states/found_states_" + std::to_string(current_board_sum) + ".bin", std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);
        if (ofs_terminal_states.fail()) {
            std::cerr << "File open error: " << result_dir + "states/found_states_" + std::to_string(current_board_sum) + ".bin\n";
            return -1;
        }
        std::ofstream ofs_found_afterstates(result_dir + "afterstates/found_afterstates_" + std::to_string(current_board_sum) + ".bin", std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);
        if (ofs_terminal_states.fail()) {
            std::cerr << "File open error: " << result_dir + "afterstates/found_afterstates_" + std::to_string(current_board_sum) + ".bin\n";
            return -1;
        }

        for (const uint64_t& normalized_state_key : found_states[current_board_sum]) {
            ofs_found_states.write(reinterpret_cast<const char*>(&normalized_state_key), sizeof(uint64_t));
            Board state_board(0);
            state_board.set(normalized_state_key);

            if (state_board.is_gameover()) {
                ofs_terminal_states.write(reinterpret_cast<const char*>(&normalized_state_key), sizeof(uint64_t));
                terminal_states_num += 1;
                continue;
            }

            Board afterstate_board(0);
            for (Action action : state_board.legal_actions()) {
                afterstate_board.set(normalized_state_key);
                afterstate_board.do_action(action);
                uint64_t normalized_afterstate_key = afterstate_board.normalize();
                afterstate_board.set(normalized_afterstate_key);

                /* skip already found afterstate */
                if (found_afterstates[current_board_sum].find(normalized_afterstate_key) != found_afterstates[current_board_sum].end()) {
                    continue;
                }

                found_afterstates[current_board_sum].insert(normalized_afterstate_key);
                ofs_found_afterstates.write(reinterpret_cast<const char*>(&normalized_afterstate_key), sizeof(uint64_t));
                found_afterstates_num += 1;

                Board next_state_board(0);
                for (Square sq : afterstate_board.empty_squares()) {
                    for (int spawn_num = 1; spawn_num <= 2; ++spawn_num) {
                        next_state_board.set(normalized_afterstate_key);
                        next_state_board.spawn(sq, spawn_num);
                        uint64_t normalized_next_state_key = next_state_board.normalize();
                        int next_state_board_sum = next_state_board.get_board_sum();

                        if (found_states[next_state_board_sum].find(normalized_next_state_key) == found_states[next_state_board_sum].end()) {
                            found_states[next_state_board_sum].insert(normalized_next_state_key);
                            found_states_num += 1;
                        }
                    }
                }
            }
        }

        current_board_sum += 2;
    }

    std::cout << "legal states num: " << found_states_num << std::endl;
    std::cout << "legal afterstates num: " << found_afterstates_num << std::endl;
    std::cout << "terminal states num: " << terminal_states_num << std::endl;
}