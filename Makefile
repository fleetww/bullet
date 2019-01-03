bullet : build/bullet.o build/Journal.o build/pugixml.o
	g++ -g -o bullet -std=c++11 -I./include build/bullet.o build/Journal.o build/pugixml.o -lncurses

build/bullet.o : src/bullet.cpp include/Journal.hpp
	g++ -g -c -o build/bullet.o -std=c++11 -I./include src/bullet.cpp

build/Journal.o : src/Journal.cpp include/Journal.hpp include/Date.hpp include/pugixml.hpp
	g++ -g -c -o build/Journal.o -std=c++11 -I./include src/Journal.cpp

build/pugixml.o : src/pugixml.cpp include/pugixml.hpp include/pugiconfig.hpp
	g++ -g -c -o build/pugixml.o -std=c++11 -I./include src/pugixml.cpp

clean:
	rm -f bullet build/bullet.o build/Journal.o build/pugixml.o
