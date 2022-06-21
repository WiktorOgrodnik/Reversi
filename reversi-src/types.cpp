#include "types.hpp"

Player oposite(Player player) {
	switch (player) {
		case white:
			return black;
		case black:
			return white;
		default:
			return none;
	}
}