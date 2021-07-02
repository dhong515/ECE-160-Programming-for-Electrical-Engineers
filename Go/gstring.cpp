#include <iostream>
#include <set>
#include "gstring.h"

using namespace std;

bool point::operator== (const point& p) {
	if (x == p.x && y == p.y) 
		return true;
	else
		return false;
}
inline point::point(int xx, int yy) : x(xx), y(yy) {};
void point::print() const {
	cout << "(" << x << ", " << y << ")";
}

bool operator != (const point& p1, const point p2) {
	if (p1.x != p2.x || p1.y != p2.y) {
        	return true;
        }
        else {
                return false;
        }
}


int point::getx() const {
	return x;
}

int point::gety() const {
	return y;
}

bool operator< (const point p1, const point p2) {
	if (p1.x < p2.x)
        	return true;
        else if (p1.x > p2.x)
                return false;
        else if (p1.y < p2.y)
        	return true;
        else
                return false;
}


bool gstring::isSingleLiberty(point p, char c){

return ((liberties.size()==1)&&(c!= color)&&(liberties.find(p)!=liberties.end()));

}

int gstring::getSize(){

return members.size();

}

bool gstring::includes(point p) {
	if (members.find(p) == members.end()) {
		return 0;
	}
	else {
		return 1;
	}
}

char gstring::getcol() {
	return color;
}

gstring::gstring(char c) {
	color = c;
}

gstring::gstring(char c, point p) {
	color = c;
	members.insert(p);
}

void gstring::print() {
	set<point>::iterator it;
	cout << "String of color " << color << " containing members: ";
	for(it = members.begin(); it != members.end(); it++) {
		(*it).print();
		cout << ", ";
	}
	cout << "and liberties: ";
	for(it = liberties.begin(); it != liberties.end(); it++) {
		(*it).print();
		cout << ", ";
	}
	cout << endl;
}
		
int gstring::add(point p, char c) {
	set<point>::iterator it;
        it = liberties.find(p);
	if(members.find(p) != members.end()) {
		return -1; //Error! Trying to place a stone where a stone already is!!!
	}
	else if(it == liberties.end()) {
		return 0; //point is not adjacent to this string
	}
	else {
		liberties.erase(it);
		if (color == c) {
			members.insert(p);
			return 1;
		}
		else {
			if(liberties.empty())
				return 3;
			else
				return 2;
		}
	}
}

void gstring::addLib(point p) {
	liberties.insert(p);
}

point gstring::pop() {
	point p(-1, -1);
	if (!members.empty()) {
		set<point>::iterator it = members.begin();
		p = *it;
		members.erase(it);
	}
	return p;
}

gstring gstring::operator+ (const gstring& right) {
	set<point>::iterator it;
	if (color != right.color) {
		cout << "Error! Trying to concatenate two gstrings of differing color!" << endl;
		return *this;
	}
	else {
		gstring result(color);
		result.members = members;
		result.liberties = liberties;
		for(it = right.liberties.begin(); it != right.liberties.end(); it++) {
			result.liberties.insert(*it);
		}
		for(it = right.members.begin(); it != right.members.end(); it++) {
			result.members.insert(*it);
		}
		return result;
	}
}

bool should_die(const gstring& str) {
	if(str.members.empty() && str.liberties.empty()) {
		return 1;
	}
	else {
		return 0;
	}
}

inline countString::countString(point a) : gstring(' ', a), adjacent_color(' ') {}

void countString::add(point p) {
	members.insert(p);
}

void countString::add_adj(char col) {
	if (adjacent_color == ' ') {
		adjacent_color = col;
	}
	else if(adjacent_color != col) {
		adjacent_color = 'n';
	}
}

char countString::get_adj() {
	return adjacent_color;
}

void countString::print() {
	cout << "This territory belongs to color " << adjacent_color << " and contains points: ";
	set<point>::iterator it;
	for(it = members.begin(); it != members.end(); it++) {
                (*it).print();
                cout << ", ";
        }
	cout << endl;
}


