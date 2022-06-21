#ifndef FIELD_HPP
#define FIELD_HPP

struct Field {
	int x, y;
	bool valid;

	Field& operator+= (Field& other);
	Field operator+ (const Field& other) const;

	Field() {}
	Field(int _x, int _y, bool _valid) : x(_x), y(_y), valid(_valid) {}
};

struct CustomCompare {
    bool operator()(const Field& a, const Field& b) const {
        if (a.y < b.y) return true;
		else if (a.y == b.y && a.x < b.x) return true;
		return false; 
    }
};

#endif