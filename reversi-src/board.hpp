#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <cstdint>
#include "types.hpp"
#include "field.hpp"

typedef __uint128_t u_int128_t;

class Board {

	u_int128_t board;
	std::vector<std::vector<std::vector<double>>> white_ranks;
	std::vector<std::vector<std::vector<double>>> black_ranks;
	std::vector<Field> dirs;
	Field last_move;
	Field last_last_move;

	bool stable_row_col(Player player, int x, int y);

	void set_pawn(std::size_t x, std::size_t y, Player player);
	void set_pawn(Field& f, Player player);

	bool fields_empty();
	
public:

	std::pair<u_int128_t, std::pair<Field, Field>> copy();

	bool can_beat(Field field, Field& direction, Player player);
	int potential_mobility(Player player);
	int captured_corenrs(Player player);
	int captured_edges(Player player);
	int captured_stables(Player player);
	int captured_stables2(Player player);
	int captured_stables3(Player player);
	int number_of_blocks();
	double prepared_boards(Player player, int num);
	int sum_of_pawns(Player player);

	int no = 0;

	Board();

	void moves(Player player, std::vector<Field>& res);
	int how_many_moves(Player player);

	Player get(int x, int y);
	Player get(Field& field);

	void do_move(Field move, Player player);
	void undo_move(std::pair<u_int128_t, std::pair<Field, Field>>& last_board);

	void load_white_ranks();
	void load_black_ranks();

	void print();
	void restart();

	int result();
	bool terminal();
};

#endif