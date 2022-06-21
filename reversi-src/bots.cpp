#include "bots.hpp"
#include <cstdlib>

Bot::~Bot() {}

std::pair<double, Field> HeuristicBot::alpha_beta_search(Board& board, int depth, int revdepth, double alpha, double beta, bool maximizing_player, Player player) {
	
	if (depth == 0 || board.terminal()) {
		return std::make_pair(heruristic(board, revdepth + 1), Field(-1, -1, false));
	}

	std::vector<Field> moves;
	board.moves(player, moves);
		
	if (maximizing_player) {
		double max_eval = std::numeric_limits<double>::lowest();
		Field best_move = Field(-1, -1, false);

		for (auto move : moves) {
					
			if (move.valid == false) continue;

			auto pre_board = board.copy();
			board.do_move(move, player);
			double eval_a = alpha_beta_search(board, depth - 1, revdepth + 1, alpha, beta, false, oposite(player)).first;
			board.undo_move(pre_board);

			if (eval_a > max_eval) {
				max_eval = eval_a;
				best_move = move;
			}

			alpha = std::max(alpha, eval_a);
			if (beta <= alpha) break;
		}

		return std::make_pair(max_eval, best_move);

	} else {
		double min_eval = std::numeric_limits<double>::max();
		Field best_move = Field(-1, -1, false);

		for (auto move : moves) {
					
			if (move.valid == false) continue;

			auto pre_board = board.copy();
			board.do_move(move, player);
			double eval_a = alpha_beta_search(board, depth - 1, revdepth + 1, alpha, beta, true, oposite(player)).first;
			board.undo_move(pre_board);

			if (eval_a < min_eval) {
				min_eval = eval_a;
				best_move = move;
			}

			beta = std::min(beta, eval_a);
			if (beta <= alpha) break;
		}

		return std::make_pair(min_eval, best_move);
	}
}

Field CheckerBot::wait_for_move() {

	int x, y;
	std::cin >> x >> y;

	return Field(x, y, x >= 0 && y >= 0);
}

std::pair<HeuristicBot*, HeuristicBot*> HeuristicBot::crossover_op(HeuristicBot* parentA, HeuristicBot* parentB) {

	int pivot = rand() % (NMULTS - 1) + 1;
	// 1 -> 0 000000
	// 2 -> 00 00000
	// 3 -> 000 0000
	// 4 -> 0000 000
	// 5 -> 00000 00
	// 6 -> 000000 0

	std::vector<double> child1, child2, child1F, child1S, child2F, child2S;

	parentA->vector_part_left(pivot, child1F); parentB->vector_part_right(pivot, child1S);
	parentB->vector_part_right(pivot,child2F); parentA->vector_part_right(pivot, child2S);

	for (int i = 0; i < pivot; i++) {child1.push_back(child1F[i]); child2.push_back(child2F[i]);}
	for (int i = pivot; i < NMULTS; i++) {child1.push_back(child1S[i - pivot]); child2.push_back(child2S[i - pivot]);}
	
	HeuristicBot* c1 = new CustomBot(white, Medium, child1);
	HeuristicBot* c2 = new CustomBot(white, Medium, child2);

	return std::make_pair(c1, c2);
}

Field RandomBot::random_move(Board& board) {
	std::vector<Field> ms;
	board.moves(this->player, ms);
	if (!ms.empty()) return ms[rand() % ms.size()];
	return Field(-1, -1, false);
}