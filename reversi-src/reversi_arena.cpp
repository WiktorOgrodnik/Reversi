#include <iostream>
#include <algorithm>
#include <thread>
#include <future>
#include "bots.hpp"

#define SURVIVOR_BOTS 2
#define COMPETING_BOTS 12
#define THREADS 2
#define GAMES 12

bool compare (const std::pair<int, HeuristicBot*>& a, std::pair<int, HeuristicBot*>& b) {
	return a.first > b.first;
}

void AI(Board& board, Player& player, Bot* bot) {

	Field move = bot->make_move(board);
	board.no++;
	board.do_move(move, player);
	player = oposite(player);
}

void gameStep(Board& board, Player& player, Bot* botWhite, Bot* botBlack) {

	if (board.terminal()) return;

	if (player == white) AI(board, player, botWhite);
	else if (player == black) AI(board, player, botBlack);
}

int nop() {
	return 0;
}

int play_the_match(Player st_player, Bot* alg1, Bot* alg2) {
	
	Board board = Board();
	board.load_black_ranks();
	board.load_white_ranks();
	
	while (!board.terminal()) {
		gameStep(board, st_player, alg1, alg2);
	}

	return board.result();
}

int main() {

	srand(time(NULL));

	std::pair<int, HeuristicBot*> competing_bots[COMPETING_BOTS];

	Bot* random_bot = new RandomBot(black);
	Bot* boss_bot = new GeneticalExternalBot(black, Medium);

	for (int i = 0; i < COMPETING_BOTS; i++) {
		std::vector<double> val = std::vector<double>(7);
		for (int i = 0; i < 7; i++) val[i] = (double)((rand() % 20) - 5);
		competing_bots[i] = std::make_pair(0, new CustomBot(white, Medium, val));
	}

	for (int k = 0; ; k++) {

		for (int i = 0; i < COMPETING_BOTS; i++) {

			competing_bots[i].second->set_player(white);

			for (int j = 0; j < COMPETING_BOTS; j+=THREADS) {
				
				competing_bots[j].second->set_player(black);

				std::future<int> res[THREADS];

				for (int l = 0; l < THREADS; l++) {
					if (i == j + l)
						res[l] = std::async(&nop);
					else
						res[l] = std::async(&play_the_match, white, competing_bots[i].second, competing_bots[j].second);
				}

				for (int l = 0; l < THREADS; l++) {
					int p_res = res[l].get();
					if (p_res < 0) {competing_bots[i].first++; competing_bots[j].first--;}
					else if (p_res > 0) {competing_bots[i].first--; competing_bots[j].first++;}
				}
			}

			for (int j = 0; j < GAMES; j+=THREADS) {
				
				std::future<int> res[THREADS];
				
				for (int l = 0; l < THREADS; l++) {
					res[l] = std::async(&play_the_match, white, competing_bots[i].second, random_bot);
				}

				for (int l = 0; l < THREADS; l++) {
					int p_res = res[l].get();
					if (p_res < 0) {competing_bots[i].first++;}
					else if (p_res > 0) {competing_bots[i].first--;}
				}
			}

			for (int j = 0; j < GAMES; j+=THREADS) {
				
				std::future<int> res[THREADS];
				
				for (int l = 0; l < THREADS; l++) {
					res[l] = std::async(&play_the_match, white, competing_bots[i].second, boss_bot);
				}

				for (int l = 0; l < THREADS; l++) {
					int p_res = res[l].get();
					if (p_res < 0) {competing_bots[i].first++;}
					else if (p_res > 0) {competing_bots[i].first--;}
				}
			}

			std::cout << ((double)i/COMPETING_BOTS) * 100.0 << "%\n";
		}

		std::sort(competing_bots, competing_bots + COMPETING_BOTS, compare);

		competing_bots[0].second->save_settings(k + 1);

		for (int i = 0; i < SURVIVOR_BOTS; i++) {
			competing_bots[i].first = 0;
		}

		for (int i = SURVIVOR_BOTS; i < COMPETING_BOTS; i = i + 2) {
		 	delete competing_bots[i].second;
			delete competing_bots[i + 1].second;

			int index1 = rand() % SURVIVOR_BOTS;
			int index2 = (index1 + (rand() % (SURVIVOR_BOTS - 1)) + 1) % SURVIVOR_BOTS;

			auto t = HeuristicBot::crossover_op(competing_bots[index1].second, competing_bots[index2].second);
			competing_bots[i] = std::make_pair(0, t.first);
			competing_bots[i + 1] = std::make_pair(0, t.second);

			t.first->mutate();
			t.second->mutate();
		}
	}

	return EXIT_SUCCESS;
}