make: gstring.o board.o
	g++ -I. -o a gstring.o board.o

gstring.o: gstring.cpp gstring.h
	g++ -I. -c gstring.cpp

board.o: board.cpp gstring.h
	g++ -I. -c board.cpp

clean:
	rm -f *.exe *.o
