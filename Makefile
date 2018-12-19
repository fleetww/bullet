OBJ = build/bullet.o build/pugixml.o
PUGI = src/pugixml.cpp include/pugiconfig.hpp include/pugiconfig.hpp
BLT = src/bullet.cpp include/bullet.hpp

bullet : $(OBJ)
	g++ -g -o bullet -std=c++11 -I./include $(OBJ) -lncurses

build/bullet.o : $(BLT) build/pugixml.o
	g++ -g -c -o build/bullet.o -std=c++11 -I./include src/bullet.cpp

build/pugixml.o : $(PUGI)
	g++ -g -c -o build/pugixml.o -std=c++11 -I./include src/pugixml.cpp

clean:
	rm -f bullet $(OBJ)
