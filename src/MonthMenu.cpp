#include "MonthMenu.hpp"
#include "Date.hpp"
#include <ncurses.h>

MonthMenu::MonthMenu(const int &currMonth) {
	win = newwin(nlines, ncols, x, y);
	currnum = currMonth;
	cursor = currMonth - 1;
	active = false;
}

MonthMenu::~MonthMenu() {
	delwin(win);
}

void MonthMenu::draw() {
	for (int m = 0; m < 12; m++) {
		mvwprintw(win, m, 0, monthnames[m].c_str());
	}

	mvwchgat(win, currnum - 1, 0, DAYOFMONTH(currnum).length(),
			A_UNDERLINE, 0, NULL);
	mvwchgat(win, cursor, 0, monthnames[cursor].length(),
			(active) ? A_BLINK|A_STANDOUT : A_STANDOUT, 0, NULL);

	wrefresh(win);
}
