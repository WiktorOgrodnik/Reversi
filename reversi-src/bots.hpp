#ifndef BOTS_HPP
#define BOTS_HPP

#define NMULTS 7

#include "types.hpp"
#include "field.hpp"
#include "board.hpp"
#include <limits>
#include <iomanip>
#include <fstream>
#include <iostream>

enum BotType {
	HumanBotType, RandomBotType, HeuristicBotType
};

class Bot {

protected:

	Player player;

public:

	BotType type;

	virtual Field make_move(Board& board) = 0;
	virtual ~Bot() = 0;
};

class HeuristicBot : public Bot {

private:

protected:

	std::vector<double> multiplikators = std::vector<double>(NMULTS, 0.0);

	Level depth_of_search;

	double heruristic(Board& board, int moves) {

		return multiplikators[0] * (board.captured_corenrs(player) - board.captured_corenrs(oposite(player))) + 
			   multiplikators[1] * (board.captured_stables3(player) - board.captured_stables3(oposite(player))) + 
			   multiplikators[2] * (board.captured_edges(player) - board.captured_edges(oposite(player))) +
			   multiplikators[3] * board.how_many_moves(player) + 
			   multiplikators[4] * board.potential_mobility(player) +
			   multiplikators[5] * board.prepared_boards(player, moves) + 
			   multiplikators[6] * board.sum_of_pawns(player);
 
	}

	std::pair<double, Field> alpha_beta_search(Board& board, int depth, int revdepth, double alpha, double beta, bool maximizing_player, Player player);

public:

	static std::pair<HeuristicBot*, HeuristicBot*> crossover_op(HeuristicBot* parentA, HeuristicBot* parentB);
	
	void vector_part_left(int index, std::vector<double>& ret) {
		ret.resize(index);
		for (int i = 0; i < index; i++) ret[i] = multiplikators[i];
	}

	void vector_part_right(int index, std::vector<double>& ret) {
		ret.resize(NMULTS - index);
		for (int i = index; i < NMULTS; i++) ret[i - index] = multiplikators[i];
	}

	void mutate() {

		if (rand() % 100 < 65) {

			for (int i = 0; i < NMULTS; i++) {
				double t = (double)(rand() % 10000)/10000.0 - 0.5;
				multiplikators[i] += t;
			}
		}
	}

	void print_settings() {
		std::cout << "Bot statistic: \n";
		std::cout << "\tcorners_multiplicator  -> " << std::fixed << std::setprecision(5) << multiplikators[0] << '\n';
		std::cout << "\tstables_multiplicator  -> " << std::fixed << std::setprecision(5) << multiplikators[1] << '\n'; 
		std::cout << "\tedges_multiplicator    -> " << std::fixed << std::setprecision(5) << multiplikators[2] << '\n'; 
		std::cout << "\tmoves_multiplicator    -> " << std::fixed << std::setprecision(5) << multiplikators[3] << '\n'; 
		std::cout << "\tmobility_multiplicator -> " << std::fixed << std::setprecision(5) << multiplikators[4] << '\n'; 
		std::cout << "\tprepared_multiplicator -> " << std::fixed << std::setprecision(5) << multiplikators[5] << '\n'; 
		std::cout << "\tgreedy_multiplicator   -> " << std::fixed << std::setprecision(5) << multiplikators[6] << '\n'; 
		std::cout << "\n\n";
	}

	void save_settings(int generation) {
		std::ofstream ofs("genetic.txt", std::ofstream::out | std::ofstream::app);

		ofs << generation << ". ";
		for (auto& k: multiplikators) {
			ofs << k << " ";
		}

		ofs << "\n";
		ofs.close();
	}

	void set_player(Player player) {
		this->player = player;
	}

	~HeuristicBot() {}

	Field make_move(Board& board) {
		return alpha_beta_search(board, (int)depth_of_search, 0, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), true, this->player).second;
	}
};

class RandomBot: public Bot {

	Field random_move(Board& board);

public:

	RandomBot(Player player) {
		this->player = player;
		this->type = RandomBotType;
	}

	~RandomBot() {}

	Field make_move(Board& board) override {
		return random_move(board);
	}
};

class CheckerBot: public Bot {

	Field wait_for_move();

public:

	CheckerBot(Player player) {
		this->player = player;
		this->type = HumanBotType;
	}

	~CheckerBot() {}

	Field make_move(Board& board) override {

		board = board;

		return wait_for_move();
	}
};

class GreedyBot : public HeuristicBot {

public:

	GreedyBot(Player player, Level level) {
		this->player = player;
		this->depth_of_search = level;
		this->type = HeuristicBotType;

		this->multiplikators[6] = 1.0;
	}
};

class MobileBot : public HeuristicBot {

public:

	MobileBot(Player player, Level level) {
		this->player = player;
		this->depth_of_search = level;
		this->type = HeuristicBotType;

		this->multiplikators[3] = 6.0;
		this->multiplikators[4] = 1.0;
	}
};

class MobileExpanisiveBot : public HeuristicBot {

public:

	MobileExpanisiveBot(Player player, Level level) {
		this->player = player;
		this->depth_of_search = level;
		this->type = HeuristicBotType;

		this->multiplikators[0] = 10.0;
		this->multiplikators[3] = 4.0;
		this->multiplikators[4] = 1.0;
	}
};

class MobileExpanisivePlusBot : public HeuristicBot {

public:

	MobileExpanisivePlusBot(Player player, Level level) {
		this->player = player;
		this->depth_of_search = level;
		this->type = HeuristicBotType;

		this->multiplikators[0] = 12.0;
		this->multiplikators[2] = 8.0;
		this->multiplikators[3] = 6.0;
		this->multiplikators[4] = 1.0;
	}
};

class MobileExpanisivePlusPlusBot : public HeuristicBot {

public:

	MobileExpanisivePlusPlusBot(Player player, Level level) {
		this->player = player;
		this->depth_of_search = level;
		this->type = HeuristicBotType;

		multiplikators[0] = 14.0;
		multiplikators[1] = 12.0;
		multiplikators[2] = 8.0;
		multiplikators[3] = 6.0;
		multiplikators[4] = 1.0;
	}
};

class ExperimentatorBot : public HeuristicBot {

public:

	ExperimentatorBot(Player player, Level level) {
		this->player = player;
		this->depth_of_search = level;
		this->type = HeuristicBotType;

		this->multiplikators[5] = 1.0;
	}
};

class GeneticalBot : public HeuristicBot {

public:

	GeneticalBot(Player player, Level level) {
		this->player = player;
		this->depth_of_search = level;
		this->type = HeuristicBotType;

		this->multiplikators[0] = 5.41;
		this->multiplikators[1] = 5.49;
		this->multiplikators[2] = 5.15;
		this->multiplikators[3] = 4.45;
		this->multiplikators[4] = -0.02;
		this->multiplikators[5] = 2.47;
		this->multiplikators[6] = 1.21;
	}
};

class GeneticalExternalBot : public HeuristicBot {

public:

	GeneticalExternalBot(Player player, Level level) {
		this->player = player;
		this->depth_of_search = level;
		this->type = HeuristicBotType;

		this->multiplikators[0] = 1.9059; 
		this->multiplikators[1] = 1.9138;
		this->multiplikators[2] = 1.6365;
		this->multiplikators[3] = 0.0976;
		this->multiplikators[4] = -2.8218;
		this->multiplikators[5] = -0.2529;
		this->multiplikators[6] = -0.3444;
	}
};

class FournalBot : public HeuristicBot {

public:

	FournalBot(Player player, Level level) {
		this->player = player;
		this->depth_of_search = level;
		this->type = HeuristicBotType;

		this->multiplikators[0] = 5.0;
		this->multiplikators[1] = 3.0;
		this->multiplikators[4] = 1.1;
	}
};

class CustomBot : public HeuristicBot {

public:

	CustomBot(Player player, Level level, std::vector<double>& c_mults) {
		this->player = player;
		this->depth_of_search = level;
		this->type = HeuristicBotType;

		multiplikators = c_mults;
	}
};

#endif