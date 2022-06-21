#include <iostream>
#include "bots.hpp"

void play_the_match() {

	bool not_configured = true;
	std::string com; double t1, t2;
	Player me; Bot* me_bot = NULL;

	Board board = Board();
	board.load_black_ranks();
	board.load_white_ranks();
	
	std::cout << "RDY\n";

	while (true) {
		
		std::cin >> com;

		if (com == "UGO") {

			if (not_configured) {
				
				me = white;
				me_bot = new MobileExpanisivePlusPlusBot(me, Hard);
				not_configured = false;

				std::cin >> t1 >> t2;

				Field move = me_bot->make_move(board); board.no++;
				std::cout << "IDO " << move.x << " " << move.y << '\n';
				board.do_move(move, me);
			} else {
				std::cerr << "ERROR: Can not start, when everything is configured\n";
			}

		} else if (com == "HEDID") {

			if (not_configured) {
				me = black;
				me_bot = new MobileExpanisivePlusPlusBot(me, Hard);
				not_configured = false;
			}
			int x, y;
			std::cin >> t1 >> t2 >> x >> y;

			Field move = Field(x, y, x >= 0 && y >= 0);
			board.no++;
			board.do_move(move, oposite(me));

			move = me_bot->make_move(board); board.no++;
			std::cout << "IDO " << move.x << " " << move.y << '\n';
			board.do_move(move, me);

		} else if (com == "ONEMORE") {

			board.restart();

			not_configured = true;
			if (me_bot) delete me_bot;
			me_bot = NULL;

			std::cout << "RDY\n";
		} else if (com == "BYE") {
			if (me_bot) delete me_bot;
			me_bot = NULL;
			break;
		} else {
			std::cerr << "Unknown command\n";
		}
	}
}

int main() {
	srand(time(NULL));
	play_the_match();
	return 0;
}