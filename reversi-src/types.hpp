#ifndef TYPES_HPP
#define TYPES_HPP

#define M 8

enum Player {
	none = 0, white = 1, black = 2
};

enum Level {
	UltraEasy = 1, VeryEasy = 2, Easy = 3, Medium = 4, Hard = 5, VeryHard = 6
};

Player oposite(Player player);

#endif