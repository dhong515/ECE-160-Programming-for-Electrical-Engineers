#include <set>

using namespace std;

class point {
        private:
                int x;
                int y;
        public:
                bool operator== (const point& p);
                point(int, int);
                friend bool operator< (const point p1, const point p2);
                friend bool operator != (const point& p1, const point p2);
                void print() const;
		int getx() const;
		int gety() const;
};

class gstring {
        protected:
                set <point> members;
                set <point> liberties;
                char color;
        public:
                int add(point, char);
                void addLib(point);
                point pop();
                gstring operator+ ( const gstring&);
		gstring();
                gstring(char);
                gstring(char, point);
                void print();
		char getcol();
		bool includes(point);
		friend bool should_die(const gstring&);
		bool isSingleLiberty(point, char);
                int getSize();
};

bool should_die(const gstring&);

class countString : public gstring {
private:
        char adjacent_color;
public:
        //inline countString();
	countString() : gstring(' '), adjacent_color(' ') { }
        inline countString(point);
        void add(point);
        void add_adj(char);
        char get_adj();
        void print();
};


