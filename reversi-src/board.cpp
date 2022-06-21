#include "board.hpp"
#include <iostream>
#include <fstream>

Board::Board() {

	restart();

	dirs = {Field(0, 1, false), Field(1, 0, false), Field(-1, 0, false), Field(0, -1, false), Field(1, 1, false), Field(-1, -1, false), Field(1, -1, false), Field(-1, 1, false)};
}

void Board::set_pawn(std::size_t x, std::size_t y, Player player) {
	u_int8_t shift = 2*(y*M + x);
	board = (board & ~(((u_int128_t)3) << shift)) | ((u_int128_t)player << shift);
}

void Board::set_pawn(Field& f, Player player) {
	set_pawn(f.x, f.y, player);
}

void Board::restart() {

	board = 0;
	no = 0;
	set_pawn(3, 3, black);
	set_pawn(4, 4, black);
	set_pawn(3, 4, white);
	set_pawn(4, 3, white);
	last_last_move = last_move = Field(-1, -1, false);
}

std::pair<u_int128_t, std::pair<Field, Field>> Board::copy() {
	return std::make_pair(board, std::make_pair(last_last_move, last_move));
}

Player Board::get(int x, int y) {
	if (0 <= x && x < M && 0 <= y && y < M) {
		return (Player)((board >> (2*(y*M + x))) & 3);
	}
	return none;
}

Player Board::get(Field& field) {
	return get(field.x, field.y);
}

bool Board::fields_empty() {
	for (std::size_t y = 0; y < M*M; y++) {
		if (get(y % M, y / M) == none) return false;
	}

	return true;
}

bool Board::can_beat(Field field, Field& direction, Player player) {
		
	int cnt = 0;
	field += direction;
		
	while (get(field) == oposite(player)) {
		field += direction;
		cnt++;
	}

	return cnt > 0 && get(field) == player;
}

void Board::moves(Player player, std::vector<Field>& res) {
	
	for (std::size_t i = 0UL; i < M*M; i++) {
		if ((board >> (2*i)) & 3) continue;

		Field ret = Field(i % M, i / M, true);

		for (auto& l : dirs) {
			if (can_beat(ret, l, player)) {
				res.push_back(ret);
				break;
			}
		}				
	}

	if (res.empty()) res.push_back(Field(-1, -1, false));
}

int Board::how_many_moves(Player player) {
	std::vector<Field> res;
	moves(player, res);
	return (int)res.size();
}

int Board::potential_mobility(Player player) {

	int sum = 0;

	for (size_t y = 0UL; y < M*M; y++) {
		if ((board >> (2*y)) & 3) continue;

		bool p = false;
		bool o = false;

		for (auto& dir : dirs) {
			if (get(y % M + dir.x, y / M + dir.y) == player) p = true;
			if (get(y % M + dir.x, y / M + dir.y) == oposite(player)) o = true;
		}

		sum += ((int)p - (int)o);
	}

	return sum;
}

int Board::captured_corenrs(Player player) {
	return (get(0, 0) == player) + 
		   (get(0, M - 1) == player) + 
		   (get(0, M - 1) == player) + 
		   (get(0, M - 1) == player); 
}

int Board::captured_stables(Player player) {
		
	int sum = 0;
	bool tune_on_clock[4] = {true, true, true, true};
	bool tune_on_anitclock[4] = {true, true, true, true};
		
	if (get(0, 0) == player) {
		int i = 0;
		while (i < M && get(0, i) == player) {
			i++;
		}

		sum+=i;
		if (i == M) tune_on_clock[1] = false;

		i = 0;
		while (i < M && get(i, 0) == player) {
			i++;
		}

		sum+=i;
		if (i == M) tune_on_anitclock[3] = false;
	}

	if (get(0, M - 1) == player) {

		if (tune_on_anitclock[1]) {
			int i = 0;
			while (i < M && get(i, M - 1) == player) {
				i++;
			}

			sum+=i;
			if (i == M) tune_on_clock[2] = false;
		}

		if (tune_on_clock[1]) {
			int i = M - 1;
			while (i >= 0 && get(0, i) == player) {
				i--;
			}
			sum += (M - i) - 1;
		}
	}

	if (get(M - 1, M - 1) == player) {

		if (tune_on_anitclock[2]) {
			int i = M - 1;
			while (i >= 0 && get(M - 1, i) == player) i--;

			sum += (M - i) - 1;
			if (i == -1) tune_on_clock[3] = false;
		}

		if (tune_on_clock[2]) {
			int i = M - 1;
			while (i >= 0 && get(i, M - 1) == player) i--;

			sum += (M - i) - 1;
		}
	}

	if (get(M - 1, 0) == player) {

		if (tune_on_anitclock[3]) {
			int i = M - 1;
			while (i >= 0 && get(i, 0) == player) i--;

			sum += (M - i) - 1;
		}

		if (tune_on_clock[3]) {
			int i = 0;
			while (i < M && get(M - 1, i) == player) i--;

			sum += i;
		}
	}

	return sum;
}

bool Board::stable_row_col(Player player, int x, int y) {
	return get(x, y) != oposite(player) && 
		get(x - 1, y - 1) != oposite(player) && 
		get(x - 1, y) != oposite(player) && 
		get(x, y - 1) != oposite(player) && 
		(get(x - 1, y + 1) != oposite(player) || get(x + 1, y - 1) != oposite(player));
}

int Board::captured_stables2(Player player) {

	int stables = 0;
	bool direction = false; // false - row, true - column
	for (int k = 0; k < 4; k++) {
		for (int i = 0; i < M; i++) {
			for (int j = i; j < M; j++) {
				if (direction) {
					if (stable_row_col(player, ((k == 2 || k == 3) ? (M - 1) : 2*i) - i, ((k == 1 || k == 3) ? (M - 1) : 2*j) - j)) stables++;
					else break;
				} else {
					if (stable_row_col(player, ((k == 2 || k == 3) ? (M - 1) : 2*j) - j, ((k == 1 || k == 3) ? (M - 1) : 2*i) - i)) stables++;
					else break;
				}
			}

			if (direction) i--;

			direction = !direction;
		}
	}

	return stables;
}

int Board::captured_stables3(Player player) {

	return (get(0, 1) == player) + 
		   (get(1, 1) == player) + 
		   (get(1, 0) == player) + 
		   
		   (get(M - 2, 0) == player) + 
		   (get(M - 2, 1) == player) + 
		   (get(M - 1, 1) == player) + 

		   (get(M - 1, M - 2) == player) + 
		   (get(M - 2, M - 2) == player) + 
		   (get(M - 2, M - 1) == player) + 

		   (get(1, M - 1) == player) + 
		   (get(1, M - 2) == player) + 
		   (get(0, M - 2) == player);
}

int Board::captured_edges(Player player) {
		
	int sum = 0;
		
	for (int i = 0; i < M; i++) {
		sum += (get(0, i) == player) + (get(i, 0) == player) + (get(0, M - 1) == player) + (get(M - 1, 0) == player);
	}

	return sum - captured_corenrs(player);
}

void Board::do_move(Field move, Player player) {

	last_last_move = last_move;
	last_move = move;

	if (move.valid == false) return;
	
	set_pawn(move, player); 
	std::vector<Field> to_beat;

	for (auto& k : dirs) {
		Field field = move;
		field += k;

		while (get(field) == oposite(player)) {
			Field f = field;
			to_beat.push_back(f);
			field += k;
		}

		if (get(field) == player) {
			for (auto& l: to_beat) {
				set_pawn(l, player);
			}
		}

		to_beat.clear();
	}
}

int Board::number_of_blocks() {

	int sum = 0;
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < M; j++) {
			sum += (get(j, i) != none);
		}
	}
	return sum;
}

void Board::undo_move(std::pair<u_int128_t, std::pair<Field, Field>>& last_board) {
	
	this->board = last_board.first;
	this->last_last_move = last_board.second.first;
	this->last_move = last_board.second.second;
}

void Board::load_white_ranks() {
	white_ranks.resize(61, std::vector<std::vector<double>>(M, std::vector<double>(M, 0.0)));
	std::ifstream file("tables_white.txt");

	for (int i = 1; i <= 60; i++) {
		for (size_t y = 0; y < M; y++) {
			for (size_t x = 0; x < M; x++) {
				file >> white_ranks[i][y][x];
			}
		}
	}

	file.close();
}

void Board::load_black_ranks() {
	black_ranks.resize(61, std::vector<std::vector<double>>(M, std::vector<double>(M, 0.0)));
	std::ifstream file("tables_black.txt");

	for (int i = 1; i <= 60; i++) {
		for (size_t y = 0; y < M; y++) {
			for (size_t x = 0; x < M; x++) {
				file >> black_ranks[i][y][x];
			}
		}
	}

	file.close();
}

void Board::print() {
	std::string letters = ".ox";
	std::cerr << "INTERNAL!\n";
	for (int y = 0; y < M; y++) {
		for (int x = 0; x < M; x++) {
			std::cerr << letters[get(x, y)];
		}
		std::cerr << '\n';
	}
	std::cerr << "\n\n";
}

int Board::result() {
	int res = 0;
	for (int y = 0; y < M * M; y++) {
		Player b = get(y%M, y/M);
		if (b == white) res--;
		else if (b == black) res++;
	}

	return res;
}

bool Board::terminal() {
	if (fields_empty()) return true;
	if (no < 2) return false;
	return (last_move.valid == false) && (last_last_move.valid == false);
}

int Board::sum_of_pawns(Player player) {
	int sum = 0;
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < M; j++) {
			sum += (get(j, i) == player);
		}
	}
	return sum;
}

double Board::prepared_boards(Player player, int num) {

	double res = 0;
	num += this->no;
	num++;

	if (num > 60) return 0;

	for (int y = 0; y < M; y++) {
		for (int x = 0; x < M; x++) {
			if (get(x, y) == player) res += (player == white ? white_ranks[num][y][x] : black_ranks[num][y][x]);
			if (get(x, y) == oposite(player)) res -= (player == black ? white_ranks[num][y][x] : black_ranks[num][y][x]);
		}
	}

	return res;
}