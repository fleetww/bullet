#include "bullet.hpp"
#include "Date.hpp"
#include "Journal.hpp"
#include "MonthMenu.hpp"
#include <iostream>
#include <ncurses.h>
#include <sys/select.h>
#include <stdlib.h>
#include <errno.h>

using namespace std;

int main(int argc, char *argv[]) {

	int year, month, day;
	int res = init_date(argc, argv, year, month, day);
	if (res) {
		return res;
	}

	bool success;
	Journal journal(year, success);
	if (!success) {
		cerr << "Failed to initialize Journal\n";
		return 1;
	}

	init_ncurses();

	MonthMenu month_menu(month);
	month_menu.draw();

	fd_set rfds;
	int retval = 0;
	while (true) {
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		retval = select(1, &rfds, NULL, NULL, NULL);

		if (retval == -1 && errno != EINTR) {
			perror("select()");
			return 1;
		}

		int ch = getch();
		if (ch == 'q') {
			break;
		}

		month_menu.update(ch);
		month_menu.draw();
	}

	return 0;
}

int init_date(int argc, char *argv[], int &year, int &month, int &day) {
	year = 1900, month = 1, day = 1;

	if (argc > 1) {
		year = stoi(argv[1]);
		if (argc > 2) {
			month = stoi(argv[2]);
			if (month < 1 || 12 < month) {
				cerr << "Invalid month\n";
				return 1;
			}
		}
		if (argc > 3) {
			day = stoi(argv[3]);
			if (day < 1 || NUM_DAYS(year, month) < day) {
				cerr << "Invalid day\n";
				return 1;
			}
		}
	} else {
		time_t nSec = time(NULL);
		struct tm *time = localtime(&nSec);
		year = time->tm_year + 1900;
		month = time->tm_mon + 1;
		day = time->tm_mday;
	}

	return 0;
}

void init_ncurses() {
	initscr();
	keypad(stdscr, TRUE);
	raw();
	noecho();
	curs_set(0);
	refresh();
	atexit(exit_handler);
}

void exit_handler() {
	endwin();
}
