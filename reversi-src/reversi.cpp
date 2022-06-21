#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <unistd.h>
#include "bots.hpp"

sf::RenderWindow* window;
bool legit = false;
bool displayResults = true;
bool clicked = true;

Player player = white;
Board board = Board();
Bot* current_bot;

sf::Vector2f margin(310.f, 120.f);
sf::Vector2f deltaX(70.f, 0);
sf::Vector2f deltaY(0, 70.f);

sf::Vector2i mousePositionWindow;
sf::Vector2f mousePositionView;

class HumanBot : public Bot {

public:

	HumanBot(Player player) {
		this->player = player;
		this->type = HumanBotType;
	}

	~HumanBot() {}

	Field make_move(Board& board) override {
		board.captured_corenrs(this->player);
		return Field(0, 0, false);
	}
};

void updateMousePositions() {
    mousePositionWindow = sf::Mouse::getPosition(*window);
    mousePositionView = window->mapPixelToCoords(mousePositionWindow);
}

void pollEvents() {
	sf::Event ev;
	while (window->pollEvent(ev)) {
		switch (ev.type) {
			case sf::Event::Closed:
				window->close();
				break;
			case sf::Event::MouseButtonReleased:
				if (current_bot->type == HumanBotType) clicked = true;
			default:
				break;
		}
	}
	
}

void AI(Board& board, Player& player, Bot* bot) {

	current_bot = bot;

	if (bot->type != HumanBotType) {

		usleep(200000);
		Field move = bot->make_move(board);
		board.no++;
		board.do_move(move, player);
		player = oposite(player);

	} else {
		if (clicked == true) {
			clicked = false;
			sf::RectangleShape field = sf::RectangleShape(sf::Vector2f(70.f, 70.f));
			std::vector<Field> moves; board.moves(player, moves);
			for (auto& k : moves) {
				field.setPosition(margin.x + deltaX.x * k.x, margin.y + deltaY.y * k.y);
				if (field.getGlobalBounds().contains(mousePositionView)) {
					board.no++;
					board.do_move(k, player);
					player = oposite(player);
				}
			}
		}
	}
	
}

void gameStep(Board& board, Player& player, Bot* botWhite, Bot* botBlack) {

	if (board.terminal()) return;

	if (player == white) AI(board, player, botWhite);
	else if (player == black) AI(board, player, botBlack);
}

void drawBoard(Board& board) {

	sf::RectangleShape background(sf::Vector2f(70.f, 70.f));

	background.setFillColor(sf::Color(1, 144, 0, 255));
	background.setOutlineColor(sf::Color::Black);
	background.setOutlineThickness(4.0f);

	for (int i = 0; i < M; i++) {
		for (int j = 0; j < M; j++) {
			background.setPosition(sf::Vector2f(margin.x + deltaX.x * j, margin.y + deltaY.y * i));
			window->draw(background);
		}
	}

	if (current_bot->type == HumanBotType) {
		sf::RectangleShape displayMoves(background);
		displayMoves.setOutlineThickness(0);
		displayMoves.setFillColor(sf::Color(255, 0, 0, 100));
		std::vector<Field> moves;
		board.moves(player, moves);
		for (auto& move : moves) {
			displayMoves.setPosition(sf::Vector2f(margin.x + deltaX.x * move.x, margin.y + deltaY.y * move.y));
			window->draw(displayMoves);
		}
	}

	sf::CircleShape pawn (30.0f);

	for (int i = 0; i < M; i++) {
		for (int j = 0; j < M; j++) {
			pawn.setPosition(sf::Vector2f(margin.x + deltaX.x * j + 4.f, margin.y + deltaY.y * i + 4.f));
			if (board.get(j, i) == white) {
				pawn.setFillColor(sf::Color::White);
				window->draw(pawn);
			} else if (board.get(j, i) == black) {
				pawn.setFillColor(sf::Color::Black);
				window->draw(pawn);
			}
		}
	}
}

Bot* create_bot(Player player, int op, Level lvl) {
	switch (op) {
		case 0: return new HumanBot(player);
		case 1: return new RandomBot(player);
		case 2: return new GreedyBot(player, lvl);
		case 3: return new MobileBot(player, lvl);
		case 4: return new MobileExpanisiveBot(player, lvl);
		case 5: return new MobileExpanisivePlusBot(player, lvl);
		case 6: return new MobileExpanisivePlusPlusBot(player, lvl);
		case 7: return new ExperimentatorBot(player, lvl);
		default: exit(EXIT_FAILURE); break;
	}

	return NULL;
}

int main() {

	srand(time(NULL));

	Level lvl = VeryEasy;
	int op, op2;

	std::cout << "Choose your White: \n";
	std::cout << "0. Human Player\n";
	std::cout << "1. Random Player - Complete idiot (Very Easy)\n";
	std::cout << "2. Greedy Player -  Really likes many pawns (Easy)\n";
	std::cout << "3. Mobile player - Loves mobility, does not like to be surrounded (Medium)\n";
	std::cout << "4. MobileExpanisive Player - Loves mobility and takes strategic positions (Hard)\n";
	std::cout << "5. MobileExpanisive+ Player - Loves mobility and takes more strategic positions (Hard)\n";
	std::cout << "6. MobileExpanisive++ Player - Loves mobility and takes even more strategic positions (Very Hard)\n";
	std::cout << "7. Experimentator Player - Played a lot before and now has a lot of experience (Easy (yet))\n";
	std::cin >> op;

	if (op >= 2) {
		std::cout << "Choose bot difficulty level: \n";
		std::cout << "1. UltraEasy\n";
		std::cout << "2. VeryEasy\n";
		std::cout << "3. Easy\n";
		std::cout << "4. Medium\n";
		std::cout << "5. Hard\n";
		std::cout << "6. Very Hard\n";

		std::cin >> op2;
		if (op2 <= 6 && op2 > 0) lvl = (Level)op2;
		else return EXIT_FAILURE;
	}

	Bot* white_bot = create_bot(white, op, lvl);

	std::cout << "Choose your Black: \n";
	std::cout << "0. Human Player\n";
	std::cout << "1. Random Player - Complete idiot (Very Easy)\n";
	std::cout << "2. Greedy Player -  Really likes many pawns (Easy)\n";
	std::cout << "3. Mobile player - Loves mobility, does not like to be surrounded (Medium)\n";
	std::cout << "4. MobileExpanisive Player - Loves mobility and takes strategic positions (Hard)\n";
	std::cout << "5. MobileExpanisive+ Player - Loves mobility and takes more strategic positions (Hard)\n";
	std::cout << "6. MobileExpanisive++ Player - Loves mobility and takes even more strategic positions (Very Hard)\n";
	std::cout << "7. Experimentator Player - Played a lot before and now has a lot of experience (Easy (yet))\n";
	std::cin >> op;

	if (op >= 2) {
		std::cout << "Choose bot difficulty level: \n";
		std::cout << "1. UltraEasy\n";
		std::cout << "2. VeryEasy\n";
		std::cout << "3. Easy\n";
		std::cout << "4. Medium\n";
		std::cout << "5. Hard\n";
		std::cout << "6. Very Hard\n";

		std::cin >> op2;
		if (op2 <= 6 && op2 > 0) lvl = (Level)op2;
		else return EXIT_FAILURE;
	}

	Bot* black_bot = create_bot(black, op, lvl);
	current_bot = white_bot;

	sf::VideoMode videoMode = sf::VideoMode(1200, 800);
	window = new sf::RenderWindow(videoMode, "Reversi", sf::Style::Titlebar | sf::Style::Close);
	window->setVerticalSyncEnabled(true);
	window->setFramerateLimit(60);

	board.load_white_ranks();
	board.load_black_ranks();

	while (window->isOpen()) {

		window->clear(sf::Color(sf::Color::White));
		updateMousePositions();
		pollEvents();
		gameStep(board, player, white_bot, black_bot);
		drawBoard(board);
		window->display();

		if (board.terminal() && displayResults) {
			if (board.result() > 0) std::cout << "Black wins!\n";
			else std::cout << "White wins!\n";
			displayResults = false; 
		}
	}

	//delete white_bot;
	//delete black_bot;

	return 0;
}