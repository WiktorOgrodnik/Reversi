#include <iostream>
#include <fstream>
#include "bots.hpp"

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

int play_the_match(Board& board, Player& st_player, Bot* alg1, Bot* alg2, int limit = M*M) {
	Player player = st_player;
	
	int k = 0;
	while (!board.terminal() && k++ < limit) {
		gameStep(board, player, alg1, alg2);
	}

	return board.result();
}

int main() {

	srand(time(NULL));

	int games = 10;
	double delta = 0.05;

	std::string file_name;
	std::cin >> file_name;
	Bot* player1 = new MobileExpanisivePlusPlusBot(white, Medium);
	Bot* player2 = new RandomBot(black);

	for (int k = 1; k < 61; k++) {

		std::vector<std::vector<double>> winner_fileds(M, std::vector<double>(M, 0));

		for (int t = 0; t < games; t++) {

			Board board = Board();
			Player player = white;

			play_the_match(board, player, player1, player2, k);

			for (int i = 0; i < games; i++) {

				Board boardCopy = board;
				Player playerCopy = player;
				int res = play_the_match(boardCopy, playerCopy, player1, player2);

				for (size_t y = 0; y < M; y++) {
					for (size_t x = 0; x < M; x++) {
						if (board.get(y, x) == white) winner_fileds[y][x] += (res < 0 ? delta : -delta);
						else if (board.get(y, x) == black) winner_fileds[y][x] += (res < 0 ? delta : -delta);
					}
				}
			}

			std::cout << "Progress " << (double)(t*100)/games << "% - " << k << "\n";
		}

		std::ofstream ofs(file_name, std::ofstream::out | std::ofstream::app);

		for (size_t y = 0; y < M; y++) {
			for (size_t x = 0; x < M; x++) {
				ofs << winner_fileds[y][x] << " ";
			}
			ofs << "\n";
		}

		ofs << "\n";
		ofs.close();

		winner_fileds.clear();
	}

	delete player1;
	delete player2;

	return 0;
}