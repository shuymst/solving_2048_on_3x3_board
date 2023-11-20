#include "board.h"
#include <algorithm>
#include <gtest/gtest.h>

TEST(spawn, empty) {
    Board::init();
    Board board(0);
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        EXPECT_EQ(true, board.is_empty(sq));
        board.spawn(sq, 1);
        EXPECT_EQ(false, board.is_empty(sq));
    }
}

TEST(do_action, right) {
    /* 
    1 0 1
    0 0 1
    0 1 1
    */
    Board board(0);
    board.spawn(SQ_1A, 1);
    board.spawn(SQ_1C, 1);
    board.spawn(SQ_2C, 1);
    board.spawn(SQ_3B, 1);
    board.spawn(SQ_3C, 1);
    EXPECT_EQ(board.do_action(RIGHT_ACTION), 8);
    EXPECT_EQ(board.get_num(SQ_1A), 0); EXPECT_EQ(board.get_num(SQ_1B), 0); EXPECT_EQ(board.get_num(SQ_1C), 2);
    EXPECT_EQ(board.get_num(SQ_2A), 0); EXPECT_EQ(board.get_num(SQ_2B), 0); EXPECT_EQ(board.get_num(SQ_2C), 1);
    EXPECT_EQ(board.get_num(SQ_3A), 0); EXPECT_EQ(board.get_num(SQ_3B), 0); EXPECT_EQ(board.get_num(SQ_3C), 2);
}

TEST(do_action, down) {
    /* 
    2 2 1
    0 4 3
    0 1 3
    */
    Board board(0);
    board.spawn(SQ_1A, 2);
    board.spawn(SQ_1B, 2);
    board.spawn(SQ_1C, 1);
    board.spawn(SQ_2A, 0);
    board.spawn(SQ_2B, 4);
    board.spawn(SQ_2C, 3);
    board.spawn(SQ_3A, 0);
    board.spawn(SQ_3B, 1);
    board.spawn(SQ_3C, 3);
    EXPECT_EQ(board.do_action(DOWN_ACTION), 16);
    EXPECT_EQ(board.get_num(SQ_1A), 0); EXPECT_EQ(board.get_num(SQ_1B), 2); EXPECT_EQ(board.get_num(SQ_1C), 0);
    EXPECT_EQ(board.get_num(SQ_2A), 0); EXPECT_EQ(board.get_num(SQ_2B), 4); EXPECT_EQ(board.get_num(SQ_2C), 1);
    EXPECT_EQ(board.get_num(SQ_3A), 2); EXPECT_EQ(board.get_num(SQ_3B), 1); EXPECT_EQ(board.get_num(SQ_3C), 4);
}

TEST(do_action, left) {
    /* 
    2 0 1
    0 3 1
    1 1 1
    */
    Board board(0);
    board.spawn(SQ_1A, 2);
    board.spawn(SQ_1B, 0);
    board.spawn(SQ_1C, 1);
    board.spawn(SQ_2A, 0);
    board.spawn(SQ_2B, 3);
    board.spawn(SQ_2C, 1);
    board.spawn(SQ_3A, 1);
    board.spawn(SQ_3B, 1);
    board.spawn(SQ_3C, 1);
    EXPECT_EQ(board.do_action(LEFT_ACTION), 4);
    EXPECT_EQ(board.get_num(SQ_1A), 2); EXPECT_EQ(board.get_num(SQ_1B), 1); EXPECT_EQ(board.get_num(SQ_1C), 0);
    EXPECT_EQ(board.get_num(SQ_2A), 3); EXPECT_EQ(board.get_num(SQ_2B), 1); EXPECT_EQ(board.get_num(SQ_2C), 0);
    EXPECT_EQ(board.get_num(SQ_3A), 2); EXPECT_EQ(board.get_num(SQ_3B), 1); EXPECT_EQ(board.get_num(SQ_3C), 0);
}

TEST(do_action, up) {
    /* 
    1 0 1
    2 2 1
    1 2 1
    */
    Board board(0);
    board.spawn(SQ_1A, 1);
    board.spawn(SQ_1B, 0);
    board.spawn(SQ_1C, 1);
    board.spawn(SQ_2A, 2);
    board.spawn(SQ_2B, 2);
    board.spawn(SQ_2C, 1);
    board.spawn(SQ_3A, 1);
    board.spawn(SQ_3B, 2);
    board.spawn(SQ_3C, 1);
    EXPECT_EQ(board.do_action(UP_ACTION), 8+4);
    EXPECT_EQ(board.get_num(SQ_1A), 1); EXPECT_EQ(board.get_num(SQ_1B), 3); EXPECT_EQ(board.get_num(SQ_1C), 2);
    EXPECT_EQ(board.get_num(SQ_2A), 2); EXPECT_EQ(board.get_num(SQ_2B), 0); EXPECT_EQ(board.get_num(SQ_2C), 1);
    EXPECT_EQ(board.get_num(SQ_3A), 1); EXPECT_EQ(board.get_num(SQ_3B), 0); EXPECT_EQ(board.get_num(SQ_3C), 0);
}

TEST(is_game_over, case1) {
    /*
    1 2 3
    4 5 6
    7 8 9
    */
    Board board(0);
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        board.spawn(sq, (int)sq+1);
    }
    EXPECT_TRUE(board.is_gameover());
}

TEST(is_game_over2, case2) {
    Board board(0);
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        board.spawn(sq, 1);
    }
    EXPECT_FALSE(board.is_gameover());
}

TEST(key, _) {
    /*
    1 2 3
    4 5 6
    7 8 9
    */
    Board board(0);
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        board.spawn(sq, (int)sq+1);
    }
    uint64_t key = board.key();
    Board another_board(0);
    another_board.set(key);
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        EXPECT_EQ(board.get_num(sq), another_board.get_num(sq));
    }
}

TEST(rotate, case90) {
    Board board(0);
    /*
    1 2 3
    4 5 6
    7 8 9
    */
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        board.spawn(sq, (int)sq+1);
    }
    Board rotated_board(0);
    /*
    3 6 9
    2 5 8
    1 4 7
    */
    rotated_board.spawn(SQ_1A, 3); rotated_board.spawn(SQ_1B, 6); rotated_board.spawn(SQ_1C, 9);
    rotated_board.spawn(SQ_2A, 2); rotated_board.spawn(SQ_2B, 5); rotated_board.spawn(SQ_2C, 8);
    rotated_board.spawn(SQ_3A, 1); rotated_board.spawn(SQ_3B, 4); rotated_board.spawn(SQ_3C, 7);

    EXPECT_EQ(board.rotate90(), rotated_board.key());
}

TEST(rotate, case180) {
    Board board(0);
    /*
    1 2 3
    4 5 6
    7 8 9
    */
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        board.spawn(sq, (int)sq+1);
    }
    Board rotated_board(0);
    /*
    9 8 7
    6 5 4
    3 2 1
    */
    rotated_board.spawn(SQ_1A, 9); rotated_board.spawn(SQ_1B, 8); rotated_board.spawn(SQ_1C, 7);
    rotated_board.spawn(SQ_2A, 6); rotated_board.spawn(SQ_2B, 5); rotated_board.spawn(SQ_2C, 4);
    rotated_board.spawn(SQ_3A, 3); rotated_board.spawn(SQ_3B, 2); rotated_board.spawn(SQ_3C, 1);

    EXPECT_EQ(board.rotate180(), rotated_board.key());
}

TEST(rotate, case270) {
    Board board(0);
    /*
    1 2 3
    4 5 6
    7 8 9
    */
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        board.spawn(sq, (int)sq+1);
    }
    Board rotated_board(0);
    /*
    7 4 1
    8 5 2
    9 6 3
    */
    rotated_board.spawn(SQ_1A, 7); rotated_board.spawn(SQ_1B, 4); rotated_board.spawn(SQ_1C, 1);
    rotated_board.spawn(SQ_2A, 8); rotated_board.spawn(SQ_2B, 5); rotated_board.spawn(SQ_2C, 2);
    rotated_board.spawn(SQ_3A, 9); rotated_board.spawn(SQ_3B, 6); rotated_board.spawn(SQ_3C, 3);

    EXPECT_EQ(board.rotate270(), rotated_board.key());
}

TEST(flip, horizontal) {
    Board board(0);
    /*
    1 2 3
    4 5 6
    7 8 9
    */
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        board.spawn(sq, (int)sq+1);
    }
    Board flipped_board(0);
    /*
    3 2 1
    6 5 4
    9 8 7
    */
    flipped_board.spawn(SQ_1A, 3); flipped_board.spawn(SQ_1B, 2); flipped_board.spawn(SQ_1C, 1);
    flipped_board.spawn(SQ_2A, 6); flipped_board.spawn(SQ_2B, 5); flipped_board.spawn(SQ_2C, 4);
    flipped_board.spawn(SQ_3A, 9); flipped_board.spawn(SQ_3B, 8); flipped_board.spawn(SQ_3C, 7);

    EXPECT_EQ(board.flip_horizontal(), flipped_board.key());
}

TEST(flip, vertical) {
    Board board(0);
    /*
    1 2 3
    4 5 6
    7 8 9
    */
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        board.spawn(sq, (int)sq+1);
    }
    Board flipped_board(0);
    /*
    7 8 9
    4 5 6
    1 2 3
    */
    flipped_board.spawn(SQ_1A, 7); flipped_board.spawn(SQ_1B, 8); flipped_board.spawn(SQ_1C, 9);
    flipped_board.spawn(SQ_2A, 4); flipped_board.spawn(SQ_2B, 5); flipped_board.spawn(SQ_2C, 6);
    flipped_board.spawn(SQ_3A, 1); flipped_board.spawn(SQ_3B, 2); flipped_board.spawn(SQ_3C, 3);

    EXPECT_EQ(board.flip_vertical(), flipped_board.key());
}

TEST(flip, diagonal1) {
    Board board(0);
    /*
    1 2 3
    4 5 6
    7 8 9
    */
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        board.spawn(sq, (int)sq+1);
    }
    Board flipped_board(0);
    /*
    1 4 7
    2 5 8
    3 6 9
    */
    flipped_board.spawn(SQ_1A, 1); flipped_board.spawn(SQ_1B, 4); flipped_board.spawn(SQ_1C, 7);
    flipped_board.spawn(SQ_2A, 2); flipped_board.spawn(SQ_2B, 5); flipped_board.spawn(SQ_2C, 8);
    flipped_board.spawn(SQ_3A, 3); flipped_board.spawn(SQ_3B, 6); flipped_board.spawn(SQ_3C, 9);

    EXPECT_EQ(board.flip_diagonal1(), flipped_board.key());
}

TEST(flip, diagonal2) {
    Board board(0);
    /*
    1 2 3
    4 5 6
    7 8 9
    */
    for (Square sq = SQ_1A; sq < SQUARE_NB; ++sq) {
        board.spawn(sq, (int)sq+1);
    }
    Board flipped_board(0);
    /*
    9 6 3
    8 5 2
    7 4 1
    */
    flipped_board.spawn(SQ_1A, 9); flipped_board.spawn(SQ_1B, 6); flipped_board.spawn(SQ_1C, 3);
    flipped_board.spawn(SQ_2A, 8); flipped_board.spawn(SQ_2B, 5); flipped_board.spawn(SQ_2C, 2);
    flipped_board.spawn(SQ_3A, 7); flipped_board.spawn(SQ_3B, 4); flipped_board.spawn(SQ_3C, 1);

    EXPECT_EQ(board.flip_diagonal2(), flipped_board.key());
}

TEST(empty_tiles, _) {
    Board board(0);
    /*
    9 0 0
    3 2 4
    5 0 2
    */
    board.spawn(SQ_1A, 9);
    board.spawn(SQ_2A, 3);
    board.spawn(SQ_2B, 2);
    board.spawn(SQ_2C, 4);
    board.spawn(SQ_3A, 5);
    board.spawn(SQ_3C, 2);

    std::vector<Square> empty_squares = board.empty_squares();
    EXPECT_TRUE(std::find(empty_squares.begin(), empty_squares.end(), SQ_1A) == empty_squares.end());
    EXPECT_TRUE(std::find(empty_squares.begin(), empty_squares.end(), SQ_1B) != empty_squares.end());
    EXPECT_TRUE(std::find(empty_squares.begin(), empty_squares.end(), SQ_1C) != empty_squares.end());
    EXPECT_TRUE(std::find(empty_squares.begin(), empty_squares.end(), SQ_2A) == empty_squares.end());
    EXPECT_TRUE(std::find(empty_squares.begin(), empty_squares.end(), SQ_2B) == empty_squares.end());
    EXPECT_TRUE(std::find(empty_squares.begin(), empty_squares.end(), SQ_2C) == empty_squares.end());
    EXPECT_TRUE(std::find(empty_squares.begin(), empty_squares.end(), SQ_3A) == empty_squares.end());
    EXPECT_TRUE(std::find(empty_squares.begin(), empty_squares.end(), SQ_3B) != empty_squares.end());
    EXPECT_TRUE(std::find(empty_squares.begin(), empty_squares.end(), SQ_3C) == empty_squares.end());
}

TEST(board_sum, zero) {
    Board board(0);
    /*
    0 0 0
    0 0 0
    0 0 0
    */
    
    
    EXPECT_EQ(board.get_board_sum(), 0);
}

TEST(board_sum, full) {
    Board board(0);
    /*
    1 2 3
    4 5 6
    7 8 9
    */
    
    board.spawn(SQ_1A, 1); board.spawn(SQ_1B, 2); board.spawn(SQ_1C, 3);
    board.spawn(SQ_2A, 4); board.spawn(SQ_2B, 5); board.spawn(SQ_2C, 6);
    board.spawn(SQ_3A, 7); board.spawn(SQ_3B, 8); board.spawn(SQ_3C, 9);
    EXPECT_EQ(board.get_board_sum(), 2+4+8+16+32+64+128+256+512);
}


