#include <iostream>
#include <thread>
#include <future>
#include "bots.hpp"

#define THREADS 5

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

int play_the_match(Player st_player, Bot* alg1, Bot* alg2) {
	
	Board board = Board();
	board.load_black_ranks();
	board.load_white_ranks();
	
	Player player = st_player;

	while (!board.terminal()) {
		gameStep(board, player, alg1, alg2);
	}

	return board.result();
}

int main() {

	srand(time(NULL));

	Bot* bot_white = new MobileExpanisivePlusPlusBot(white, VeryHard);
	Bot* bot_black = new RandomBot(black);

	int wins = 0;
	int games = 1000;

	for (int i = 0; i < games; i+=THREADS) {

		std::future<int> res[THREADS];

		for (int i = 0; i < THREADS; i++) {
			res[i] = std::async(&play_the_match, white, bot_white, bot_black);
		}

		for (int i = 0; i < THREADS; i++) {
			wins += res[i].get() < 0;
		}

		std::cout << ((double)i/games) * 100 << "%\n";
	}

	std::cout << "Success rate: " << ((double)wins/games) * 100 << "%\n";

	delete bot_white;
	delete bot_black;

	return 0;
}