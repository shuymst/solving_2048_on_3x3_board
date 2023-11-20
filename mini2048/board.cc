#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "board.h"

void Board::init() {
    right_action_.resize(0x1000);
    right_action_scores_.resize(0x1000);

    uint16_t a[3] = {};
    for (a[0] = 0; a[0] < 11; ++a[0]) {
        for (a[1] = 0; a[1] < 11; ++a[1]) {
            for (a[2] = 0; a[2] < 11; ++a[2]) {
                uint16_t score = 0;
                uint16_t tmp = (a[2] << 8) |
                               (a[1] << 4) |
                                a[0];
                int target = 2; // 右端
                uint16_t b[3] = {a[0], a[1], a[2]};
                for (int j = 1; j >= 0; --j) {
                    // 今見ているタイルに数字がなければスキップ
                    if (b[j] == 0) 
                        continue;
                    if (b[target] == 0) {
                        b[target] = b[j];
                        b[j] = 0;
                    } else {
                        if (b[j] == b[target]) {
                            b[target]++;
                            b[j] = 0;
                            score = b[target]; 
                            target--;
                        } else {
                            target--;
                            if (target != j) {
                                b[target] = b[j];
                                b[j] = 0;
                            }
                        }
                    }
                }
                uint16_t tmp_ = (b[2] << 8) |
                                (b[1] << 4) |
                                 b[0];
                right_action_[tmp] = tmp_;
                right_action_scores_[tmp] = score;
            }
        }
    }
}

Board::Board(int64_t seed) : seed(seed), mt_(seed) {
    clear();
}

bool Board::is_empty(Square sq) const {
    return get_num(sq) == 0;
}

void Board::spawn(Square sq, int num) {
    assert(is_empty(sq));
    tiles[sq] = num;
}

int Board::get_num(Square sq) const {
    return tiles[sq];
}

int Board::get_board_sum() const {
    int sum = 0;
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        if (!is_empty(sq)) {
            sum += (1<<get_num(sq));
        }
    }
    return sum;
}

void Board::random_spawn() {
    std::vector<Square> empty_tiles;
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        if (is_empty(sq)) {
            empty_tiles.push_back(sq);
        }
    }
    assert(empty_tiles.size() > 0);
    Square loc = empty_tiles[mt_() % empty_tiles.size()];
    if (mt_() % 10 == 1) {
        spawn(loc, 2);
    } else {
        spawn(loc, 1);
    }
}

void Board::reset() {
    clear();
    random_spawn();
    random_spawn();
}

void Board::clear() {
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        tiles[sq] = 0;
    }
}

bool Board::is_gameover() const {
    return !is_changed_by(RIGHT_ACTION) && !is_changed_by(DOWN_ACTION) && !is_changed_by(LEFT_ACTION) && !is_changed_by(UP_ACTION);
}

std::vector<Square> Board::empty_squares() const {
    std::vector<Square> res;
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        if (is_empty(sq)) {
            res.emplace_back(sq);
        }
    }
    return res;
}

uint32_t Board::do_action(Action action) {
    if (action == RIGHT_ACTION) {
        uint16_t row0 = tiles[SQ_1A] | (tiles[SQ_1B] << 4) | (tiles[SQ_1C] << 8);
        uint16_t row1 = tiles[SQ_2A] | (tiles[SQ_2B] << 4) | (tiles[SQ_2C] << 8);
        uint16_t row2 = tiles[SQ_3A] | (tiles[SQ_3B] << 4) | (tiles[SQ_3C] << 8);

        uint16_t new_row0 = right_action_[row0];
        uint16_t new_row1 = right_action_[row1];
        uint16_t new_row2 = right_action_[row2];

        tiles[SQ_1A] = new_row0 & 0xF;
        tiles[SQ_1B] = (new_row0 >> 4) & 0xF;
        tiles[SQ_1C] = (new_row0 >> 8) & 0xF;
        tiles[SQ_2A] = new_row1 & 0xF;
        tiles[SQ_2B] = (new_row1 >> 4) & 0xF;
        tiles[SQ_2C] = (new_row1 >> 8) & 0xF;
        tiles[SQ_3A] = new_row2 & 0xF;
        tiles[SQ_3B] = (new_row2 >> 4) & 0xF;
        tiles[SQ_3C] = (new_row2 >> 8) & 0xF;

        // 0xFFFEとの論理積を取ることで0をビットシフトした際の余計な1点に対応
        return (1U << right_action_scores_[row0] & 0xFFFE) + (1U << right_action_scores_[row1] & 0xFFFE) + (1U << right_action_scores_[row2] & 0xFFFE);
    } else if (action == DOWN_ACTION) {
        uint16_t line0 = tiles[SQ_1A] | (tiles[SQ_2A] << 4) | (tiles[SQ_3A] << 8);
        uint16_t line1 = tiles[SQ_1B] | (tiles[SQ_2B] << 4) | (tiles[SQ_3B] << 8);
        uint16_t line2 = tiles[SQ_1C] | (tiles[SQ_2C] << 4) | (tiles[SQ_3C] << 8);

        uint16_t new_line0 = right_action_[line0];
        uint16_t new_line1 = right_action_[line1];
        uint16_t new_line2 = right_action_[line2];

        tiles[SQ_1A] = new_line0 & 0xF;
        tiles[SQ_1B] = new_line1 & 0xF;
        tiles[SQ_1C] = new_line2 & 0xF;
        tiles[SQ_2A] = (new_line0 >> 4) & 0xF;
        tiles[SQ_2B] = (new_line1 >> 4) & 0xF;
        tiles[SQ_2C] = (new_line2 >> 4) & 0xF;
        tiles[SQ_3A] = (new_line0 >> 8) & 0xF;
        tiles[SQ_3B] = (new_line1 >> 8) & 0xF;
        tiles[SQ_3C] = (new_line2 >> 8) & 0xF;

        return (1U << right_action_scores_[line0] & 0xFFFE) + (1U << right_action_scores_[line1] & 0xFFFE) + (1U << right_action_scores_[line2] & 0xFFFE);
    } else if (action == LEFT_ACTION) {
        uint16_t row0 = tiles[SQ_1C] | (tiles[SQ_1B] << 4) | (tiles[SQ_1A] << 8);
        uint16_t row1 = tiles[SQ_2C] | (tiles[SQ_2B] << 4) | (tiles[SQ_2A] << 8);
        uint16_t row2 = tiles[SQ_3C] | (tiles[SQ_3B] << 4) | (tiles[SQ_3A] << 8);

        uint16_t new_row0 = right_action_[row0];
        uint16_t new_row1 = right_action_[row1];
        uint16_t new_row2 = right_action_[row2];

        tiles[SQ_1A] = (new_row0 >> 8) & 0xF;
        tiles[SQ_1B] = (new_row0 >> 4) & 0xF;
        tiles[SQ_1C] = new_row0 & 0xF;
        tiles[SQ_2A] = (new_row1 >> 8) & 0xF;
        tiles[SQ_2B] = (new_row1 >> 4) & 0xF;
        tiles[SQ_2C] = new_row1 & 0xF;
        tiles[SQ_3A] = (new_row2 >> 8) & 0xF;
        tiles[SQ_3B] = (new_row2 >> 4) & 0xF;
        tiles[SQ_3C] = new_row2 & 0xF;

        return (1U << right_action_scores_[row0] & 0xFFFE) + (1U << right_action_scores_[row1] & 0xFFFE) + (1U << right_action_scores_[row2] & 0xFFFE);
    } else if (action == UP_ACTION) {
        uint16_t line0 = tiles[SQ_3A] | (tiles[SQ_2A] << 4) | (tiles[SQ_1A] << 8);
        uint16_t line1 = tiles[SQ_3B] | (tiles[SQ_2B] << 4) | (tiles[SQ_1B] << 8);
        uint16_t line2 = tiles[SQ_3C] | (tiles[SQ_2C] << 4) | (tiles[SQ_1C] << 8);

        uint16_t new_line0 = right_action_[line0];
        uint16_t new_line1 = right_action_[line1];
        uint16_t new_line2 = right_action_[line2];

        tiles[SQ_1A] = (new_line0 >> 8) & 0xF;
        tiles[SQ_1B] = (new_line1 >> 8) & 0xF;
        tiles[SQ_1C] = (new_line2 >> 8) & 0xF;
        tiles[SQ_2A] = (new_line0 >> 4) & 0xF;
        tiles[SQ_2B] = (new_line1 >> 4) & 0xF;
        tiles[SQ_2C] = (new_line2 >> 4) & 0xF;
        tiles[SQ_3A] = new_line0 & 0xF;
        tiles[SQ_3B] = new_line1 & 0xF;
        tiles[SQ_3C] = new_line2 & 0xF;

        return (1U << right_action_scores_[line0] & 0xFFFE) + (1U << right_action_scores_[line1] & 0xFFFE) + (1U << right_action_scores_[line2] & 0xFFFE);
    }
    return 1U << 15;
}

std::vector<Action> Board::legal_actions() const {
    std::vector<Action> res;
    if (is_changed_by(RIGHT_ACTION)) {
        res.push_back(RIGHT_ACTION);
    }
    if (is_changed_by(DOWN_ACTION)) {
        res.push_back(DOWN_ACTION);
    }
    if (is_changed_by(LEFT_ACTION)) {
        res.push_back(LEFT_ACTION);
    }
    if (is_changed_by(UP_ACTION)) {
        res.push_back(UP_ACTION);
    }
    return res;
}

uint32_t Board::compute_reward(Action action) const {
    assert(action != INVALID_ACTION);
    if (action == RIGHT_ACTION || action == LEFT_ACTION) {
        uint16_t row0 = tiles[SQ_1A] | (tiles[SQ_1B] << 4) | (tiles[SQ_1C] << 8);
        uint16_t row1 = tiles[SQ_2A] | (tiles[SQ_2B] << 4) | (tiles[SQ_2C] << 8);
        uint16_t row2 = tiles[SQ_3A] | (tiles[SQ_3B] << 4) | (tiles[SQ_3C] << 8);
        return (1U << right_action_scores_[row0] & 0xFFFE) + (1U << right_action_scores_[row1] & 0xFFFE) + (1U << right_action_scores_[row2] & 0xFFFE);
    } else {
        uint16_t line0 = tiles[SQ_1A] | (tiles[SQ_2A] << 4) | (tiles[SQ_3A] << 8);
        uint16_t line1 = tiles[SQ_1B] | (tiles[SQ_2B] << 4) | (tiles[SQ_3B] << 8);
        uint16_t line2 = tiles[SQ_1C] | (tiles[SQ_2C] << 4) | (tiles[SQ_3C] << 8);
        return (1U << right_action_scores_[line0] & 0xFFFE) + (1U << right_action_scores_[line1] & 0xFFFE) + (1U << right_action_scores_[line2] & 0xFFFE);
    }
}

bool Board::is_changed_by(Action action) const {
    if (action == RIGHT_ACTION) {
        uint16_t row0 = tiles[SQ_1A] | (tiles[SQ_1B] << 4) | (tiles[SQ_1C] << 8);
        uint16_t row1 = tiles[SQ_2A] | (tiles[SQ_2B] << 4) | (tiles[SQ_2C] << 8);
        uint16_t row2 = tiles[SQ_3A] | (tiles[SQ_3B] << 4) | (tiles[SQ_3C] << 8);

        uint16_t new_row0 = right_action_[row0];
        uint16_t new_row1 = right_action_[row1];
        uint16_t new_row2 = right_action_[row2];

        if (row0 != new_row0) return true;
        if (row1 != new_row1) return true;
        if (row2 != new_row2) return true;

        return false;
    } else if (action == DOWN_ACTION) {
        uint16_t line0 = tiles[SQ_1A] | (tiles[SQ_2A] << 4) | (tiles[SQ_3A] << 8);
        uint16_t line1 = tiles[SQ_1B] | (tiles[SQ_2B] << 4) | (tiles[SQ_3B] << 8);
        uint16_t line2 = tiles[SQ_1C] | (tiles[SQ_2C] << 4) | (tiles[SQ_3C] << 8);

        uint16_t new_line0 = right_action_[line0];
        uint16_t new_line1 = right_action_[line1];
        uint16_t new_line2 = right_action_[line2];

        if (line0 != new_line0) return true;
        if (line1 != new_line1) return true;
        if (line2 != new_line2) return true;

        return false;
    } else if (action == LEFT_ACTION) {
        uint16_t row0 = tiles[SQ_1C] | (tiles[SQ_1B] << 4) | (tiles[SQ_1A] << 8);
        uint16_t row1 = tiles[SQ_2C] | (tiles[SQ_2B] << 4) | (tiles[SQ_2A] << 8);
        uint16_t row2 = tiles[SQ_3C] | (tiles[SQ_3B] << 4) | (tiles[SQ_3A] << 8);

        uint16_t new_row0 = right_action_[row0];
        uint16_t new_row1 = right_action_[row1];
        uint16_t new_row2 = right_action_[row2];

        if (row0 != new_row0) return true;
        if (row1 != new_row1) return true;
        if (row2 != new_row2) return true;

        return false;
    } else if (action == UP_ACTION) {
        uint16_t line0 = tiles[SQ_3A] | (tiles[SQ_2A] << 4) | (tiles[SQ_1A] << 8);
        uint16_t line1 = tiles[SQ_3B] | (tiles[SQ_2B] << 4) | (tiles[SQ_1B] << 8);
        uint16_t line2 = tiles[SQ_3C] | (tiles[SQ_2C] << 4) | (tiles[SQ_1C] << 8);

        uint16_t new_line0 = right_action_[line0];
        uint16_t new_line1 = right_action_[line1];
        uint16_t new_line2 = right_action_[line2];

        if (line0 != new_line0) return true;
        if (line1 != new_line1) return true;
        if (line2 != new_line2) return true;

        return false;
    }
    return true;
}

uint64_t Board::key() const {
    uint64_t k = 0;
    k |= (uint64_t)get_num(SQ_1A);
    k |= (uint64_t)get_num(SQ_1B) << 4;
    k |= (uint64_t)get_num(SQ_1C) << 8;
    k |= (uint64_t)get_num(SQ_2A) << 12;
    k |= (uint64_t)get_num(SQ_2B) << 16;
    k |= (uint64_t)get_num(SQ_2C) << 20;
    k |= (uint64_t)get_num(SQ_3A) << 24;
    k |= (uint64_t)get_num(SQ_3B) << 28;
    k |= (uint64_t)get_num(SQ_3C) << 32;
    return k;
}

void Board::set(uint64_t key) {
    tiles[SQ_1A] = key & 0xF;
    tiles[SQ_1B] = (key >> 4) & 0xF;
    tiles[SQ_1C] = (key >> 8) & 0xF;
    tiles[SQ_2A] = (key >> 12) & 0xF;
    tiles[SQ_2B] = (key >> 16) & 0xF;
    tiles[SQ_2C] = (key >> 20) & 0xF;
    tiles[SQ_3A] = (key >> 24) & 0xF;
    tiles[SQ_3B] = (key >> 28) & 0xF;
    tiles[SQ_3C] = (key >> 32) & 0xF;
}

uint64_t Board::rotate90() const {
    uint64_t k = 0;
    k |= (uint64_t)get_num(SQ_1C);
    k |= (uint64_t)get_num(SQ_2C) << 4;
    k |= (uint64_t)get_num(SQ_3C) << 8;
    k |= (uint64_t)get_num(SQ_1B) << 12;
    k |= (uint64_t)get_num(SQ_2B) << 16;
    k |= (uint64_t)get_num(SQ_3B) << 20;
    k |= (uint64_t)get_num(SQ_1A) << 24;
    k |= (uint64_t)get_num(SQ_2A) << 28;
    k |= (uint64_t)get_num(SQ_3A) << 32;
    return k;
}

uint64_t Board::rotate180() const {
    uint64_t k = 0;
    k |= (uint64_t)get_num(SQ_3C); 
    k |= (uint64_t)get_num(SQ_3B) << 4; 
    k |= (uint64_t)get_num(SQ_3A) << 8; 
    k |= (uint64_t)get_num(SQ_2C) << 12; 
    k |= (uint64_t)get_num(SQ_2B) << 16; 
    k |= (uint64_t)get_num(SQ_2A) << 20; 
    k |= (uint64_t)get_num(SQ_1C) << 24; 
    k |= (uint64_t)get_num(SQ_1B) << 28; 
    k |= (uint64_t)get_num(SQ_1A) << 32; 
    return k;
}

uint64_t Board::rotate270() const {
    uint64_t k = 0;
    k |= (uint64_t)get_num(SQ_3A); 
    k |= (uint64_t)get_num(SQ_2A) << 4; 
    k |= (uint64_t)get_num(SQ_1A) << 8; 
    k |= (uint64_t)get_num(SQ_3B) << 12; 
    k |= (uint64_t)get_num(SQ_2B) << 16; 
    k |= (uint64_t)get_num(SQ_1B) << 20; 
    k |= (uint64_t)get_num(SQ_3C) << 24; 
    k |= (uint64_t)get_num(SQ_2C) << 28; 
    k |= (uint64_t)get_num(SQ_1C) << 32; 
    return k;
}

uint64_t Board::flip_horizontal() const {
    uint64_t k = 0;
    k |= (uint64_t)get_num(SQ_1C); 
    k |= (uint64_t)get_num(SQ_1B) << 4; 
    k |= (uint64_t)get_num(SQ_1A) << 8; 
    k |= (uint64_t)get_num(SQ_2C) << 12; 
    k |= (uint64_t)get_num(SQ_2B) << 16; 
    k |= (uint64_t)get_num(SQ_2A) << 20; 
    k |= (uint64_t)get_num(SQ_3C) << 24; 
    k |= (uint64_t)get_num(SQ_3B) << 28; 
    k |= (uint64_t)get_num(SQ_3A) << 32; 
    return k;
}

uint64_t Board::flip_vertical() const {
    uint64_t k = 0;
    k |= (uint64_t)get_num(SQ_3A); 
    k |= (uint64_t)get_num(SQ_3B) << 4; 
    k |= (uint64_t)get_num(SQ_3C) << 8; 
    k |= (uint64_t)get_num(SQ_2A) << 12; 
    k |= (uint64_t)get_num(SQ_2B) << 16; 
    k |= (uint64_t)get_num(SQ_2C) << 20; 
    k |= (uint64_t)get_num(SQ_1A) << 24; 
    k |= (uint64_t)get_num(SQ_1B) << 28; 
    k |= (uint64_t)get_num(SQ_1C) << 32; 
    return k;
}

uint64_t Board::flip_diagonal1() const {
    uint64_t k = 0;
    k |= (uint64_t)get_num(SQ_1A); 
    k |= (uint64_t)get_num(SQ_2A) << 4; 
    k |= (uint64_t)get_num(SQ_3A) << 8; 
    k |= (uint64_t)get_num(SQ_1B) << 12; 
    k |= (uint64_t)get_num(SQ_2B) << 16; 
    k |= (uint64_t)get_num(SQ_3B) << 20; 
    k |= (uint64_t)get_num(SQ_1C) << 24; 
    k |= (uint64_t)get_num(SQ_2C) << 28; 
    k |= (uint64_t)get_num(SQ_3C) << 32; 
    return k;
}

uint64_t Board::flip_diagonal2() const {
    uint64_t k = 0;
    k |= (uint64_t)get_num(SQ_3C); 
    k |= (uint64_t)get_num(SQ_2C) << 4; 
    k |= (uint64_t)get_num(SQ_1C) << 8; 
    k |= (uint64_t)get_num(SQ_3B) << 12; 
    k |= (uint64_t)get_num(SQ_2B) << 16; 
    k |= (uint64_t)get_num(SQ_1B) << 20; 
    k |= (uint64_t)get_num(SQ_3A) << 24; 
    k |= (uint64_t)get_num(SQ_2A) << 28; 
    k |= (uint64_t)get_num(SQ_1A) << 32; 
    return k;
}

uint64_t Board::normalize() const {
    uint64_t k1 = key();
    uint64_t k2 = rotate90();
    uint64_t k3 = rotate180();
    uint64_t k4 = rotate270();
    uint64_t k5 = flip_horizontal();
    uint64_t k6 = flip_vertical();
    uint64_t k7 = flip_diagonal1();
    uint64_t k8 = flip_diagonal2();
    return std::min({k1, k2, k3, k4, k5, k6, k7, k8});
}

void Board::print() const {
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            Square sq = Square(3*y + x);
            printf("%3d", get_num(sq));
        }
        printf("\n");
    }
    std::cout << std::flush;
}