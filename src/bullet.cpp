#include "Date.hpp"
#include "Journal.hpp"
#include "MonthMenu.hpp"
#include <iostream>
#include <ncurses.h>

using namespace std;

int main(int argc, char *argv[]) {
	initscr();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);

	refresh();
	MonthMenu month_menu(1);
	month_menu.draw();
	getch();

	refresh();

	endwin();
}



int journalTest(int argc, char *argv[]) {
	int year = 1900, month = 1, day = 1;
	if (argc > 1) {
		year = stoi(argv[1]);
		if (argc > 2) {
			month = stoi(argv[2]);
		}
		if (argc > 3) {
			day = stoi(argv[3]);
		}
	} else {
		time_t nSec = time(NULL);
		struct tm *time = localtime(&nSec);
		year = time->tm_year + 1900;
		month = time->tm_mon + 1;
		day = time->tm_mday;
	}

	bool success;
	Journal journal(year, success);
	if (!success) {
		cerr << "Failed to initialize Journal\n";
		return 1;
	}
}
