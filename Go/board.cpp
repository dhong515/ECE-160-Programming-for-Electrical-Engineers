#include "gstring.h"
#include <iostream>
#include <list>
#include <vector>
#include <stack>

class board {
	private:
		int i;
		int j;
		char ** arr;
		list<gstring> strings;
		int player1_score = 0;
		float player2_score = 0;
	public:
		board(int);
		~board();
		int remove_string(list<gstring>::iterator, point&);
		point addStone(char, int, int);
		point addStoneNoPoints(char, int, int);
		void print();
		void count_territories();
		bool isSingleCapture(point, char);
		void addKomi();
		bool isIllegalMove(int, int, char); 
		void p1add1();
		void p2add1();
		bool addHandicap(int);
		void checkStrings(bool);
};

board::board(int size) {
	i = size;
	j = size;
	arr = new char * [size];
	for(int ii = 0; ii < size; ii++) {
		arr[ii] = new char [size];
	}
	for (int ii = 0; ii < i; ii++) {
                for (int jj = 0; jj < j; jj++) {
                        arr[ii][jj] = ' ';
                }
        }
}

board::~board() {
	for (int ii = 0;  ii < i; ii++) {
		delete [] arr[ii];
	}
	delete [] arr;
}

int board::remove_string(list<gstring>::iterator it, point& ptFlag) {
	point error(-1, -1); //set an error flag to match up what gstring.pop() returns if no points remain
        point pt(0, 0);
	int pointsCount = 0;
	point norm(-2, -2); //set a norm point to match what ptFlag should be before any points get removed -> this is part of the ko rule implementation
        while((pt = (*it).pop()) != error) {
        	if(ptFlag == norm) { //if one and only one stone gets removed, set ptFlag to that value. If more than one stone is removed, set ptFlag to error. Part of ko.
			ptFlag = pt;
		}
		else {
			ptFlag = error;
		}
		pointsCount++; //for each point in the string removed add a point
		int xx = pt.getx();
                int yy = pt.gety();
                arr[yy][xx] = ' ';
		//Go through every possible adjacenecy:
		//For each one check first if it is even part of the board (chck first to avoid segfaults)
		//Then, if it is, check that spot on the board to see if it is occupied by a stone the opposing color to the string being removed
		//If so, add a liberty back to the string containing that adjacency corresponding to the point being removed from the string
                if (yy+1 < j) {
                	if(arr[yy+1][xx] != ' ' && arr[yy+1][xx] != (*it).getcol()) {
                        	point pt2(xx, yy+1);
                                list<gstring>::iterator it1;
                                for(it1 = strings.begin(); it1 != strings.end(); it1++) {
                                	if((*it1).includes(pt2)) {
                                        	(*it1).addLib(pt);
                                                break;
                                        }
                               	}
                        }
                }
                if (yy-1 >= 0) {
                        if(arr[yy-1][xx] != ' ' && arr[yy-1][xx] != (*it).getcol()) {
                                point pt2(xx, yy-1);
                                list<gstring>::iterator it1;
                                for(it1 = strings.begin(); it1 != strings.end(); it1++) {
                                        if((*it1).includes(pt2)) {
                                                (*it1).addLib(pt);
                                                break;
                                        }
                                }
                        }
                }
                if (xx+1 < i) {
                        if(arr[yy][xx+1] != ' ' && arr[yy][xx+1] != (*it).getcol()) {
                                point pt2(xx+1, yy);
                                list<gstring>::iterator it1;
                                for(it1 = strings.begin(); it1 != strings.end(); it1++) {
                                        if((*it1).includes(pt2)) {
                                                (*it1).addLib(pt);
                                                break;
                                        }
                                }
                        }
                }
                if (xx-1 >= 0) {
                        if(arr[yy][xx-1] != ' ' && arr[yy][xx-1] != (*it).getcol()) {
                                point pt2(xx-1, yy);
                                list<gstring>::iterator it1;
                                for(it1 = strings.begin(); it1 != strings.end(); it1++) {
                                        if((*it1).includes(pt2)) {
                                               (*it1).addLib(pt);
                                                break;
                                        }
                                }
                        }
                }
        }
	return pointsCount;
}

point board::addStone(char c, int m, int n) {
	int pointsCount = 0;
	point ptFlag(-2, -2); //create point ptFlag to store single point removed through the stone added, if applicable -> part of ko rule implementation
	point norm(-2, -2);
	point error(-1, -1); //create an error flag point
	arr[m][n] = c;
	point p(n, m); //x then y, as opposed to row then col
	list<gstring>::iterator it, it1;
	vector<int> to_fuse;
	int ii = -1;
	for(it = strings.begin(); it != strings.end(); it++) { //check through all of the strings
		ii++;
		int flag = (*it).add(p, c);
		if (flag == -1) {
			cout << "Error! Space not free. Ending" << endl; //if this ever triggers, there's a complete mismatch between global and local data.
			exit(1); //Under such circumstances, the program is so fucked up that it should end immediately
		}
		else if(flag == 1) { //If the flag was 1, this means the stone was added on to this string
			to_fuse.push_back(ii); //we store this string in to_fuse in case two or more strings were connected by this point
			if (m+1 < i) { //for each adjacent point, check if in board then check to see if it is proper to add this as a liberty to the string
				if(arr[m+1][n] == ' ') {
					point q(n, m+1);
					(*it).addLib(q);
				}
			}
		        if (m-1 >= 0) {
                                if(arr[m-1][n] == ' ') {
                                        point q(n, m-1);
                                        (*it).addLib(q);
                                }
                        }
			if (n+1 < j) {
                                if(arr[m][n+1] == ' ') {
                                        point q(n+1, m);
                                        (*it).addLib(q);
                                }
                        }
			if (n-1 >= 0) {
                                if(arr[m][n-1] == ' ') {
                                        point q(n-1, m);
                                        (*it).addLib(q);
                                }
                        }
                }

		else if(flag == 3) { //if the flag was 3, the string lost all of its liberties and should be removed and have its size added to the pointsCount
			pointsCount += remove_string(it, ptFlag);
		}
	}
	if (to_fuse.size() == 0) { //if no strings were extended by p, create a new string for p, and use the standard adjacency checker to find liberties
		gstring s(c, p);
		if (m+1 < i) {
                       	if(arr[m+1][n] == ' ') {
                               	point q(n, m+1);
                                s.addLib(q);
                        }
                }
                if (m-1 >= 0) {
                        if(arr[m-1][n] == ' ') {
                                point q(n, m-1);
                                s.addLib(q);
                      	}
                }
                if (n+1 < j) {
                        if(arr[m][n+1] == ' ') {
                                point q(n+1, m);
                                s.addLib(q);
                        }
                }
                if (n-1 >= 0) {
                        if(arr[m][n-1] == ' ') {
                                point q(n-1, m);
                                s.addLib(q);
                        }
		}
		strings.push_front(s);
	}
	else if (to_fuse.size() > 1) { //if two strings were both extended by the same move, they must be concatenated
		int size = to_fuse.size() - 1;
		it = strings.begin();
		advance(it, to_fuse[size]); //merge strings that shared a liberty that has now been filled by a stone of the same color as the strings in question
		for (int iii = size - 1; iii >= 0; iii--) { //starting at the back to avoid invalidating the coordinate in to_fuse, fuse the strings together
			it1 = strings.begin();
			advance(it1, to_fuse[iii]);
		        (*it).print();	
			(*it) = (*it) + (*it1);
			strings.erase(it1);			
		}
	}
	if(c == 'b') {
		player1_score += pointsCount;
	}
	else if(c == 'w') {
		player2_score += pointsCount;
	}
	strings.remove_if(should_die); //if the gstring should die, we kill it
	if(ptFlag == norm) { //if no strings got removed, ptFlag should still be an error
		ptFlag = error;
	} 
	return ptFlag;
}

point board::addStoneNoPoints(char c, int m, int n) {
        point ptFlag(-2, -2); //create point ptFlag to store single point removed through the stone added, if applicable -> part of ko rule implementation
        point norm(-2, -2);
        point error(-1, -1); //create an error flag point
        arr[m][n] = c;
        point p(n, m); //x then y, as opposed to row then col
        list<gstring>::iterator it, it1;
        vector<int> to_fuse;
        int ii = -1;
        for(it = strings.begin(); it != strings.end(); it++) { //check through all of the strings
                ii++;
                int flag = (*it).add(p, c);
                if (flag == -1) {
                        cout << "Error! Space not free. Ending" << endl; //if this ever triggers, there's a complete mismatch between global and local data.
                        exit(1); //Under such circumstances, the program is so fucked up that it should end immediately
                }
                else if(flag == 1) { //If the flag was 1, this means the stone was added on to this string
                        to_fuse.push_back(ii); //we store this string in to_fuse in case two or more strings were connected by this point
                        if (m+1 < i) { //for each adjacent point, check if in board then check to see if it is proper to add this as a liberty to the string
                                if(arr[m+1][n] == ' ') {
                                        point q(n, m+1);
                                        (*it).addLib(q);
                                }
                        }
                        if (m-1 >= 0) {
                                if(arr[m-1][n] == ' ') {
                                        point q(n, m-1);
                                        (*it).addLib(q);
                                }
                        }
                        if (n+1 < j) {
                                if(arr[m][n+1] == ' ') {
                                        point q(n+1, m);
                                        (*it).addLib(q);
                                }
                        }
                        if (n-1 >= 0) {
                                if(arr[m][n-1] == ' ') {
                                        point q(n-1, m);
                                        (*it).addLib(q);
                                }
                        }
                }

                else if(flag == 3) { //if the flag was 3, the string lost all of its liberties and should be removed and have its size added to the pointsCount
                        remove_string(it, ptFlag);
                }
        }
        if (to_fuse.size() == 0) { //if no strings were extended by p, create a new string for p, and use the standard adjacency checker to find liberties
                gstring s(c, p);
                if (m+1 < i) {
                        if(arr[m+1][n] == ' ') {
                                point q(n, m+1);
                                s.addLib(q);
                        }
                }
                if (m-1 >= 0) {
                        if(arr[m-1][n] == ' ') {
                                point q(n, m-1);
                                s.addLib(q);
                        }
                }
                if (n+1 < j) {
                        if(arr[m][n+1] == ' ') {
                                point q(n+1, m);
                                s.addLib(q);
                        }
                }
                if (n-1 >= 0) {
                        if(arr[m][n-1] == ' ') {
                                point q(n-1, m);
                                s.addLib(q);
                        }
                }
                strings.push_front(s);
        }
        else if (to_fuse.size() > 1) { //if two strings were both extended by the same move, they must be concatenated
                int size = to_fuse.size() - 1;
                it = strings.begin();
                advance(it, to_fuse[size]); //merge strings that shared a liberty that has now been filled by a stone of the same color as the strings in question
                for (int iii = size - 1; iii >= 0; iii--) { //starting at the back to avoid invalidating the coordinate in to_fuse, fuse the strings together
                        it1 = strings.begin();
                        advance(it1, to_fuse[iii]);
                        (*it).print();
                        (*it) = (*it) + (*it1);
                        strings.erase(it1);
                }
        }
        strings.remove_if(should_die); //if the gstring should die, we kill it
        if(ptFlag == norm) { //if no strings got removed, ptFlag should still be an error
                ptFlag = error;
        }
        return ptFlag;
}


void board::print() {
	cout << "Board of size " << i << endl;
	for(int ii = 0; ii < i-1; ii++){
		for(int jj = 0; jj < j-1; jj++) {
			cout << arr[ii][jj] << '-';
		}
		cout << arr[ii][j-1] << endl;
		for(int jj = 0; jj < j; jj++) {
                	cout << "| ";
        	}
		cout<<endl;
	}
	for(int jj = 0; jj < j-1; jj++) {
                cout << arr[i-1][jj] << '-';
        }
	cout << arr[i-1][j-1] << endl<<endl;
	cout << "Player 1: " << player1_score << endl << "Player 2: " << player2_score << endl;
}


void board::count_territories() {
	list<countString> territories; //create a list type to store each block of territory
	for(int ii = 0; ii < i; ii++) {
		for(int jj = 0; jj < j; jj++) { //iterate through the board
			if(arr[ii][jj] == ' ') { //if the spot is open it should begin a new countString
				countString counter;
				point p(jj, ii);
				stack<point> st;
				st.push(p);
				while(!st.empty()) { //execute a depth-first search for empty connected tiles
					point pp = st.top();
					int x = pp.getx();
					int y = pp.gety();
					counter.add(pp);
					arr[y][x] = '~'; //tilde acts  as a marker that this spot is already being counted as territory
					st.pop();
					if(x-1 >= 0) {
						if(arr[y][x-1] == ' ') {
							point temp(x-1, y);
							st.push(temp);
						}
						else if(arr[y][x-1] != '~'){
							counter.add_adj(arr[y][x-1]);
						}
					}
                                        if(y+1 < j) {
                                                if(arr[y+1][x] == ' ') {
                                                        point temp(x, y+1);
                                                        st.push(temp);
                                                }
						else if(arr[y+1][x] != '~'){
                                                       	counter.add_adj(arr[y+1][x]);
                                                }
                                        }
                                        if(y-1 >= 0) {
                                                if(arr[y-1][x] == ' ') {
                                                        point temp(x, y-1);
                                                        st.push(temp);
                                                }
						else if(arr[y-1][x] != '~'){
                                                        counter.add_adj(arr[y-1][x]);
                                                }
                                        }
                                        if(x+1 < i) {
                                                if(arr[y][x+1] == ' ') {
                                                        point temp(x+1, y);
                                                        st.push(temp);
                                                }
						else if(arr[y][x+1] != '~'){
                                                        counter.add_adj(arr[y][x+1]);
                                                }
                                        }
				}
				territories.push_front(counter); //counter is a block of empty territory, it should be added to the list of such blocks
			}
		}
	}
	list<countString>::iterator it; //create an iterator for our list of territory blocks
	for(it = territories.begin(); it != territories.end(); it++) { //for each block of territory
		int pointsCount = 0;
		point pt(0, 0);
		point error(-1, -1);
		while((pt = (*it).pop()) != error) { //remove all the points in it, clearing the tilde space-marker from the board and adding a point each time
			pointsCount++;
			int x = pt.getx();
			int y = pt.gety();
			arr[y][x] = ' ';
		}
		if((*it).get_adj() == 'b') { //if the territory was surrounded entirely by black tiles, add the points to black
			player1_score += pointsCount;
		}
		else if((*it).get_adj() == 'w') { //if the territory was surrounded entirely by white tiles, add the points to white
			player2_score += pointsCount;
		}
	}
	//Having tallied up all points from territories in addition to all points from string capture, declare a winner to end the game
	if(player2_score > player1_score) {
		cout << "White wins!!!" << endl;
	}
	else if(player1_score > player2_score) {
		cout << "Black wins!!!" << endl;
	}
	else{
		cout << "The game is a tie." << endl; //May happen in case of a handicap, but otherwise will not happen because of non-integer komi
	}
}

bool board::isSingleCapture(point p, char c){
	int numStoneCaptured=0;
	list<gstring>::iterator it;
	for(it = strings.begin(); it != strings.end(); it++) {
		if((*it).isSingleLiberty(p,c)){ //if placing this stone will kill a given string
			numStoneCaptured+=(*it).getSize(); //add to the number of stones captured the size of the string
		}
	}
	return (numStoneCaptured==1); //if the number of stones captured is exactly 1, return true. Else return false.
}

void board::addKomi(){
	player2_score+=7.5;
}

bool board:: isIllegalMove(int m ,int n, char col){

if (m < 0 || m >= i || n < 0 || n >= j) {
                cout << "Error: out of bounds" << endl;
		return true;
        }
else if (arr[m][n] != ' ') {
                cout << "Error: space not free" << endl;
		return true;
        }

else { //check to make sure the move wouldn't kill a string
	bool spaceAvailable = false;
	bool connectedSameColor = false;
	//check to see if there are adjacent empty spaces and if there are adjacent stones of the same color
	if(m-1 >= 0) {
		if(arr[m-1][n] == ' ') {
			spaceAvailable = true;
		}
		else if(arr[m-1][n] == col) {
			connectedSameColor = true;
		}
	}
        if(m+1 < i) {
                if(arr[m+1][n] == ' ') {
                        spaceAvailable = true;
                }
                else if(arr[m+1][n] == col) {
                        connectedSameColor = true;
		}
        }
        if(n-1 >= 0) {
                if(arr[m][n-1] == ' ') {
                        spaceAvailable = true;
                }
                else if(arr[m][n-1] == col) {
                        connectedSameColor = true;
		}
        }
        if(n+1 < i) {
                if(arr[m][n+1] == ' ') {
                        spaceAvailable = true;
                }
                else if(arr[m][n+1] == col) {
                        connectedSameColor = true;
        	}
	}
	if(spaceAvailable) { //if there are adjacent empty spaces, this move can't possibly a suicide, so it can be played
		return false;
	}	
	else {
		//check to see if this move will remove a string of the opposing color -- if so, it'll make space and is a legal move
		point move(n, m);
		if(m-1 >= 0) {
                        if(arr[m-1][n] != col && arr[m-1][n] != ' ') {
                                list<gstring>::iterator it;
                                point adjacency(n, m-1);
                                for(it = strings.begin(); it != strings.end(); it++) {
                                        if((*it).includes(adjacency)) {
                                                if((*it).isSingleLiberty(move, 'n')) {
                                                        //cout << "Will remove piece in m-1 position" << endl;
							return false;
                                                }
                                        }
                                }
                        }
                }
                if(m+1 < i) {
                        if(arr[m+1][n] != col && arr[m+1][n] != ' ') {
                                list<gstring>::iterator it;
                                point adjacency(n, m+1);
                                for(it = strings.begin(); it != strings.end(); it++) {
                                        if((*it).includes(adjacency)) {
                                                if((*it).isSingleLiberty(move, 'n')) {
                                                        //cout<<"Will remove piece in m+1 position" << endl;
							return false;
                                                }
                                        }
                                }
                        }
                }
                if(n-1 >= 0) {
                        if(arr[m][n-1] != col && arr[m][n-1] != ' ') {
                                list<gstring>::iterator it;
                                point adjacency(n-1, m);
                                for(it = strings.begin(); it != strings.end(); it++) {
                                        if((*it).includes(adjacency)) {
                                                if((*it).isSingleLiberty(move, 'n')) {
                                                        //cout << "Will remove piece in n-1 position" << endl;
							return false;
                                                }
                                        }
                                }
                        }
                }
                if(n+1 < i) {
                        if(arr[m][n+1] != col && arr[m][n+1] != ' ') {
                                list<gstring>::iterator it;
                                point adjacency(n+1, m);
                                for(it = strings.begin(); it != strings.end(); it++) {
                                        if((*it).includes(adjacency)) {
                                                if((*it).isSingleLiberty(move, 'n')) {
                                                        //cout << "Will remove piece in n+1 position" << endl;
							return false;
                                                }
					}
				}
			}
		}
	}
	if(!connectedSameColor) { 
		//if there are no adjacent spaces or stones of the same color, and the move will not kill a string of the opposing color, it is an illegal suicide
		cout << "Error: That space would have no liberties!" << endl;
		return true;
	}
	else {
		//if there are adjacent stones of the same color, then check to make sure that this move will not leave them liberty-less.
		point move(n, m);
		if(m-1 >= 0) {
                	if(arr[m-1][n] == col) {
				list<gstring>::iterator it;
				point adjacency(n, m-1);
				for(it = strings.begin(); it != strings.end(); it++) {
					if((*it).includes(adjacency)) {
						if(!(*it).isSingleLiberty(move, 'n')) {
							return false;
						}
					}
				}
                	}
        	}
        	if(m+1 < i) {
                	if(arr[m+1][n] == col) {
                        	list<gstring>::iterator it;
                                point adjacency(n, m+1);
                                for(it = strings.begin(); it != strings.end(); it++) {
                                        if((*it).includes(adjacency)) {
                                                if(!(*it).isSingleLiberty(move, 'n')) {
                                                        return false;
                                                }
                                        }
                                }
                	}
        	}
        	if(n-1 >= 0) {
                	if(arr[m][n-1] == col) {
                                list<gstring>::iterator it;
                                point adjacency(n-1, m);
                                for(it = strings.begin(); it != strings.end(); it++) {
                                        if((*it).includes(adjacency)) {
                                                if(!(*it).isSingleLiberty(move, 'n')) {
                                                        return false;
                                                }
                                        }
                                }
			}
        	}
        	if(n+1 < i) {
                	if(arr[m][n+1] == col) {
                                list<gstring>::iterator it;
                                point adjacency(n+1, m);
                                for(it = strings.begin(); it != strings.end(); it++) {
                                        if((*it).includes(adjacency)) {
                                                if(!(*it).isSingleLiberty(move, 'n')) {
                                                        return false;
                                                }
                                        }
                                }
                        }
        	}
	}
	cout << "Error: that stone would remove the last liberty from a string of your color!" << endl;
	return true;	
}
}

void board::p1add1() {
	player1_score++;
}

void board::p2add1() {
	player2_score++;
}

bool board::addHandicap(int size) {
	int d;
	cout<<"How many handicap stones?";
	cin >> d;
	//for the various size/handicap number pairs, due to the lack of pattern simply we have a brute-force hardcoding of the positions where black stones must be added
	if(size==9){
		while(d != 1 && d!=2 && d!=3 && d!=4 && d!=5){
			cout<<"Invalid number! Try agian.";
           		cin >> d;
		}
		if(d==2){
			addStone('b',6,2);
			addStone('b',2,6);
		}
		else if(d==3){
			addStone('b',6,2);
                	addStone('b',2,6);
			addStone('b',6,6);
		}
		else if(d==4){
			addStone('b',6,2);
                	addStone('b',2,6);
                	addStone('b',6,6);
			addStone('b',2,2);
		}
		else if (d == 5){
			addStone('b',6,2);
                	addStone('b',2,6);
			addStone('b',6,6);
			addStone('b',2,2);
			addStone('b',4,4);
		}
	}
	else if (size==13){
		while(d!=1&& d!=2 && d!=3 && d!=4 && d!=5 && d!=6 && d!=7 && d!=8 && d!=9){
			cout<<"Invalid number! Try agian.";
			cin >> d;
		}
		if(d==2){
			addStone('b',10,3);
			addStone('b',3,10);
		}
		else if(d==3){
			addStone('b',9,3);
			addStone('b',3,9);
			addStone('b',9,9);
		}
		else if(d==4){
			addStone('b',9,3);
			addStone('b',3,9);
			addStone('b',9,9);
			addStone('b',3,3);
		}
		else if(d==5){
			addStone('b',9,3);
			addStone('b',3,9);
			addStone('b',9,9);
			addStone('b',3,3);
			addStone('b',6,6);
		}
		else if(d==6){
			addStone('b',9,3);
			addStone('b',3,9);
			addStone('b',9,9);
			addStone('b',3,3);
			addStone('b',3,6);
			addStone('b',9,6);
		}
		else if(d==7){
			addStone('b',9,3);
			addStone('b',3,9);
			addStone('b',9,9);
			addStone('b',3,3);
			addStone('b',3,6);
			addStone('b',9,6);
			addStone('b',6,6);
		}
		else if(d==8){
			addStone('b',9,3);
			addStone('b',3,9);
			addStone('b',9,9);
			addStone('b',3,3);
			addStone('b',3,6);
			addStone('b',9,6);
			addStone('b',6,3);
			addStone('b',6,9);
		}
		else if (d == 9){
			addStone('b',9,3);
			addStone('b',3,9);
			addStone('b',9,9);
			addStone('b',3,3);
			addStone('b',3,6);
			addStone('b',9,6);
			addStone('b',6,3);
			addStone('b',6,9);
			addStone('b',6,6);
		}
	}
	else{
		while(d!= 1 && d!=2 && d!=3 && d!=4 && d!=5 && d!=6 && d!=7 && d!=8 && d!=9){
			cout<<"Invalid number! Try agian.";
			cin >> d;
		}
		if(d==2){
			addStone('b',15,3);
			addStone('b',3,15);
		}
		else if(d==3){
			addStone('b',15,3);
			addStone('b',3,15);
			addStone('b',15,15);
		}
		else if(d==4){
			addStone('b',15,3);
			addStone('b',3,15);
			addStone('b',15,15);
			addStone('b',3,3);
		}
		else if(d==5){
			addStone('b',15,3);
			addStone('b',3,15);
			addStone('b',15,15);
			addStone('b',3,3);
			addStone('b',9,9);
		}
		else if(d==6){
			addStone('b',15,3);
			addStone('b',3,15);
			addStone('b',15,15);
			addStone('b',3,3);
			addStone('b',3,9);
			addStone('b',15,9);
		}       
		else if(d==7){
			addStone('b',15,3);
			addStone('b',3,15);
			addStone('b',15,15);
			addStone('b',3,3);
			addStone('b',3,9);
			addStone('b',15,9);
			addStone('b',9,9);
		}       
		else if(d==8){
			addStone('b',15,3);
			addStone('b',3,15);
			addStone('b',15,15);
			addStone('b',3,3);
			addStone('b',3,9);
			addStone('b',9,3);
			addStone('b',15,9);
			addStone('b',9,15);
		}       
		else if(d == 9){   
			addStone('b',15,3);
			addStone('b',3,15);
			addStone('b',15,15);
			addStone('b',3,3);
			addStone('b',3,9);
			addStone('b',9,3);
			addStone('b',15,9);
			addStone('b',9,15);
			addStone('b',9,9);	
		}
	}	
	return (d==0||d==1);
}

void board::checkStrings(bool turn) {
	list<gstring>::iterator it;
	for(it = strings.begin(); it != strings.end(); it++) {
		point ptFlag(-1, -1);
		int m, n;
		point error(-1, -1);
		char decision, col;
		cout << "For the ";
		(*it).print();
		cout << "Do both players agree on whether or not this string is dead? (y/n): ";
		cin >> decision;
		if(decision == 'y') {
			cout << "Is this string dead? (y/n): ";
			cin >> decision;
			if(decision == 'y') {//Removes a given colored string if both players agree that the string is dead and if the string is actually dead and adds the opposing player's score by the length of the string that was captured.
				char col = (*it).getcol();
				int points = remove_string(it, ptFlag);
				if(col == 'b') {
					player2_score += points;
				}
				else if(col == 'w') {
					player1_score += points;
				}
				print();
			}
		}
		else {	
			while(1){
				cout<<"Do you yet agree(y/n)";//At the start of each turn, ask the players if they are yet in agreement. 
				cin >> decision;
				if(decision == 'y') { //If they are and the string is actually dead, remove the string.
					cout << "Is it dead? (y/n): ";
					cin >> decision;
					if(decision == 'y') {
						remove_string(it, ptFlag);
					}
					break;
				}
				if(turn){
					col='b';
				}
				else{
					col='w';
				}

				do{
					cout << "Enter an x coordinate: ";
					cin >> n;
					cout << "Enter a y coordinate: ";
					cin >> m;
				}
				while(isIllegalMove(m, n, col));
				point move(n,m); //x then y, rather than row then column
				if(ptFlag != error){ //if one and only one stone was captured last turn, then we must check for a ko
					if(move==ptFlag){ //A ko only happens if a stone enters the place of the single stone captured last turn
						if(isSingleCapture(ptFlag, col)){ //and furthermore can only happen if placing that stone only captures 1 stone in turn
							cout<<"That would be a ko. You cannot do that!"<<endl; //if all of those conditions are met, the move is a ko
							continue; //continue to the next iteration of the loop, don't toggle the turn bool, thus restart the current turn
						}
					}
				}
				ptFlag = addStoneNoPoints(col, m, n); //make the point and store any single stone captured in ptFlag for next turn's ko check
				print();
				turn=!turn;

			}
		}
	}
}


int main() {
	int size;
	do {
		cout<<"Choose size board (enter 9 for 9X9, 13 for 13X13, and 19 for 19X19): ";
		cin >> size;
	} while(size!=9 && size!=13 && size!=19);
	board b(size);
	point ptFlag(0, 0);
	int m;
        int n;
        char col;
	char c;
	bool turn = true;
	bool passLastTurn;
	point error(-1,-1);
	cout<<"Would black like to implement the handicap rule?(y/n)";
	cin >> c;
	if(c=='y'){
		turn = b.addHandicap(size);
	}
	else {
		b.addKomi();
	}
	b.print();
	while(1) {
		cout << "Would you like to pass? (y/n): ";
		cin >> col;
		if(col == 'y') { //check for two passes in a row for endgame
			if(turn) {
				b.p2add1();
			}
			else {
				b.p1add1();
			}
			if(passLastTurn) {
                        	break;
			}
			else {
				passLastTurn = true;
				turn = !turn;
				continue;
			}
		}
		else {
			passLastTurn = false;
		}
		if(turn){
			col='b';
		}
		else{
			col='w';
		}

		do{
			cout << "Enter an x coordinate: ";
			cin >> n;
			cout << "Enter a y coordinate: ";
			cin >> m;
                }
        	while(b.isIllegalMove(m, n, col));
		point move(n,m); //x then y, rather than row then column
		if(ptFlag != error){ //if one and only one stone was captured last turn, then we must check for a ko
			if(move==ptFlag){ //A ko only happens if a stone enters the place of the single stone captured last turn
				if(b.isSingleCapture(ptFlag, col)){ //and furthermore can only happen if placing that stone only captures 1 stone in turn
					cout<<"That would be a ko. You cannot do that!"<<endl; //if all of those conditions are met, the move is a ko and is forbidden
					continue; //and we continue to the next iteration of the loop without toggling the turn bool to restart the current player's turn
				}
			}
		}
		ptFlag = b.addStone(col, m, n); //if the move goes through all legality checks, we make it and store any single stone captured in ptFlag for next turn's ko
		b.print();
		turn=!turn;

	}
	b.checkStrings(turn);
	b.count_territories();
	b.print();
	return 0;
}

		       
