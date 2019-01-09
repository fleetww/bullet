#include "DayMenu.hpp"
#include <ncurses.h>

DayMenu::DayMenu(int currDay) {
	win = newwin(nlines, ncols, x, y);
	currnum = currDay;
	cursor = currDay - 1;
	active = false;
}

DayMenu::~DayMenu() {
	delwin(win);
}

void DayMenu::draw() {
	werase(win);
	int slcdaynum, len;
	for (int i = 0; i < 12; i++) {
		slcdaynum = 0; // TODO find currently selected number
		mvwprintw(win, i, 0, "%d", slcdaynum);

	}
}

void DayMenu::update(int input) {

}

void DayMenu::move_cursor(int dt) {

}

int DayMenu::get_curr_num() {
	return currnum;
}

int DayMenu::get_cursor() {
	return cursor;
}
