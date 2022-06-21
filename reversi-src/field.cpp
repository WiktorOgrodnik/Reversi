#include "field.hpp"

Field& Field::operator+= (Field& other) {
	this->x += other.x;
	this->y += other.y;

	return *this;
}

Field Field::operator+ (const Field& other) const {
	return Field(this->x + other.x, this->y + other.y, true);
}