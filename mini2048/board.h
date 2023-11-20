#ifndef _BOARD_H_
#define _BOARD_H_

#include "types.h"
#include <vector>
#include <random>
#include <cassert>

class Board {
    private:
        const int64_t seed;
        std::mt19937_64 mt_;
        uint8_t tiles[9];
        inline static std::vector<uint16_t> right_action_;
        inline static std::vector<uint16_t> right_action_scores_;
    
    public:
        public:
        static void init();
        Board(int64_t seed);
        Board(const Board&) = delete;
        Board(Board&&) = delete;

        bool is_empty(Square sq) const;
        void spawn(Square sq, int num);
        int get_num(Square sq) const;
        int get_board_sum() const;
        void random_spawn();
        void reset();
        void clear();
        bool is_gameover() const;
        std::vector<Square> empty_squares() const;

        uint32_t do_action(Action action);
        std::vector<Action> legal_actions() const;
        uint32_t compute_reward(Action action) const;
        bool is_changed_by(Action action) const;
        
        uint64_t key() const;
        void set(uint64_t key);
        uint64_t rotate90() const;
        uint64_t rotate180() const;
        uint64_t rotate270() const;
        uint64_t flip_horizontal() const;
        uint64_t flip_vertical() const;
        uint64_t flip_diagonal1() const;
        uint64_t flip_diagonal2() const;
        uint64_t normalize() const;

        void print() const;
};

#endif