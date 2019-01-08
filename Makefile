JOURNAL = include/Journal.hpp
MONTHMENU = include/MonthMenu.hpp
DATE = include/Date.hpp
PUGI = include/pugixml.hpp
BLD = build/bullet.o build/Journal.o build/MonthMenu.o build/Date.o build/pugixml.o

bullet : $(BLD)
	g++ -g -o bullet -std=c++11 -I./include $(BLD) -lncurses

build/bullet.o : src/bullet.cpp $(JOURNAL) $(DATE)
	g++ -g -c -o build/bullet.o -std=c++11 -I./include src/bullet.cpp

build/Journal.o : src/Journal.cpp $(JOURNAL) $(DATE) $(PUGI)
	g++ -g -c -o build/Journal.o -std=c++11 -I./include src/Journal.cpp

build/MonthMenu.o : src/MonthMenu.cpp include/MonthMenu.hpp $(DATE)
	g++ -g -c -o build/MonthMenu.o -std=c++11 -I./include src/MonthMenu.cpp

build/Date.o : src/Date.cpp $(DATE)
	g++ -g -c -o build/Date.o -std=c++11 -I./include src/Date.cpp

build/pugixml.o : src/pugixml.cpp $(PUGI) include/pugiconfig.hpp
	g++ -g -c -o build/pugixml.o -std=c++11 -I./include src/pugixml.cpp

clean:
	rm -f bullet $(BLD)
