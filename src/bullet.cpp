#include "bullet.hpp"

using namespace std;
using namespace pugi;

int main(int argc, char *argv[]) {
	init_date();
	int res = init_bullet_journal();
	if (res) {
		return res;
	}
	init_ncurses();

	int lastmin = -1;
	while (1) {
		int ch = getch();
		if (ch == 'q') break;

		time_t currenttimet = time(NULL);
		currenttime = localtime(&currenttimet);
		if (currenttime->tm_min != lastmin) {
			lastmin = currenttime->tm_min;
			info_win_draw();
		}

		switch (currmenu) {
			case Month:
				month_menu_update(ch);
				break;
			case Day:
				day_menu_update(ch);
				break;
		}
	}

	return 0;
}

char leap_year(int year) {
	char leap = (year % 4 == 0);
	if (year % 100 == 0 && year % 400 != 0) {
		leap = 0;
	}
	return leap;
}

char day_of_week(int y, int m, int d) {
	int daystotal = d;
	for (int year = 1; year <= y; year++) {
		int max_month = (year < y ? 12 : m - 1);
		for (int month = 1; month <= max_month; month++) {
			daystotal += NUM_DAYS(year, month);
		}
	}
	return daystotal % 7;
}

void init_date() {
	time_t currenttimet = time(NULL);
	currenttime = localtime(&currenttimet);
	year = currenttime->tm_year + 1900;
	monthnum = currenttime->tm_mon + 1;
	daynum = currenttime->tm_mday;
}

bool file_exists(const std::string& path) {
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

bool make_bullet_dir(const std::string& path) {
	return !mkdir(path.c_str(), 0777);
}

bool make_bullet_journal(const std::string& path, int year) {
	string xmlbase = "<calendar year=\"" + to_string(year) + "\"><month></month><month></month><month></month><month></month><month></month><month></month><month></month><month></month><month></month><month></month><month></month><month></month></calendar>";

	xml_parse_result result = bulletdoc.load_buffer(xmlbase.c_str(), xmlbase.length());
	if (!result) {
		return false;
	}

	xml_node calendar = bulletdoc.child("calendar");
	xml_node month = calendar.child("month");
	char leapyear = leap_year(year);

	for (int m = 1; m <= 12; m++) {
		int numdays = daytab[leapyear][m];
		for (int d = 0; d < numdays; d++) {
			xml_node day = month.append_child("day");
		}
		month = month.next_sibling("month");
	}
	return bulletdoc.save_file(path.c_str(),"\t", format_indent | format_no_empty_element_tags);
}

int init_bullet_journal() {
	struct passwd *pw = getpwuid(getuid());
	bulletdirpath = ((string) pw->pw_dir) + "/.bullet";
	journalname = to_string(year) + ".xml";
	journalpath = bulletdirpath + "/" + journalname;

	//handle .bullet dir not existing, as well as year.xml not existing
	if (!file_exists(journalpath)) {
		cerr << "Bullet Journal (~/.bullet/" << journalname
			<< ") does not exists, creating now\n";
		if (!file_exists(bulletdirpath)) {
			cerr << "Bullet Journal Directory (~/.bullet) does not exist, creating now\n";
			if (!make_bullet_dir(bulletdirpath)) {
				cerr << "Failed to create Bullet Journal Directory\n";
				return 2;
			}
		}
		if (!make_bullet_journal(journalpath, year)) {
			cerr << "Failed to create Bullet Journal\n";
			return 3;
		}
	} else {
		xml_parse_result result = bulletdoc.load_file(journalpath.c_str());
		if (!result) {
			cerr << "Failed to load Bullet Journal\n";
			return 1;
		}
	}
	return 0;
}

void month_menu_draw() {
	for (int m = 0; m < 12; m++) {
		mvwprintw(monthwin, m, 0, monthnames[m].c_str());
	}

	mvwchgat(monthwin, currmonthnum - 1, 0, DAYOFMONTH(currmonthnum).length(),
			A_UNDERLINE, 0, NULL);
	mvwchgat(monthwin, monthcursor, 0, monthnames[monthcursor].length(),
			(currmenu == Month) ? A_BLINK|A_STANDOUT : A_STANDOUT, 0, NULL);

	wrefresh(monthwin);
}

void month_menu_update(int input) {
	switch (input) {
		case 'l':
			if (monthcursor != (currmonthnum - 1)) {
				monthcursor = currmonthnum - 1;
			}
			currmenu = Day;
			monthmenudirty = true;
			daymenudirty = true;
			break;
		case 'k':
			month_menu_cursor_up();
			break;
		case 'j':
			month_menu_cursor_down();
			break;
	}

	if (monthmenudirty)
		month_menu_draw();
	monthmenudirty = false;
}

void month_menu_cursor_up() {
	if (monthcursor > 0) {
		monthcursor--;
		monthmenudirty = true;
	}
}

void month_menu_cursor_down() {
	if (monthcursor < 11) {
		monthcursor++;
		monthmenudirty = true;
	}
}

void day_menu_draw() {
	for (int i = 0; i < 12; i++) {
		int slcdaynum = SELECTED_DAYNUM(i);
		mvwprintw(daywin, i, 0, "%d ", slcdaynum);

		int len = (slcdaynum > 9) ? 2 : 1;
		if (slcdaynum == currdaynum) {
			mvwchgat(daywin, i, 0, len, A_UNDERLINE, 0, NULL);
		}
		if (i == daycursor) {
			mvwchgat(daywin, i, 0, len, (currmenu == Day) ? A_BLINK|A_STANDOUT : A_STANDOUT, 0, NULL);
		}
	}
	wrefresh(daywin);
}

void day_menu_update(int input) {
	switch (input) {
		case 'h':
			if (SELECTED_DAYNUM(daycursor) != currdaynum) {
				if (currdaynum > 12) {
					dayoffset = currdaynum - 11;
					daycursor = 11;
				} else {
					dayoffset = 1;
					daycursor = currdaynum - 1;
				}
			}
			currmenu = Month;
			daymenudirty = true;
			monthmenudirty = true;
			break;
		case 'k':
			day_menu_cursor_up();
			break;
		case 'j':
			day_menu_cursor_down();
			break;
	}

	if (daymenudirty)
		day_menu_draw();
	daymenudirty = false;
}

void day_menu_cursor_up() {
	if (daycursor > 0) {
		daycursor--;
		daymenudirty = true;
	} else if (SELECTED_DAYNUM(daycursor) > 1) {
		dayoffset--;
		daymenudirty = true;
	}
}

void day_menu_cursor_down() {
	if (daycursor < 11) {
		daycursor++;
		daymenudirty = true;
	} else if (SELECTED_DAYNUM(daycursor) < NUM_DAYS(year, monthnum)){
		dayoffset++;
		daymenudirty = true;
	}
}

void info_win_draw() {

	string currstring = DAYOFWEEK(currdaynum,currmonthnum,year) + " " +
		DAYOFMONTH(currmonthnum) + " " + to_string(currdaynum) + ", " + to_string(year);

	int hour = currenttime->tm_hour;
	bool isAM = hour >= 12;
	hour = (hour > 12) ? hour - 12 : hour;

	int min = currenttime->tm_min;
	string timestring = to_string(hour) + ":" + ((min < 10) ? "0" : "");
	timestring += to_string(min) + (isAM ? " AM" : " PM");
	string nowstring = DAYOFWEEK(daynum,monthnum,year) + " " + DAYOFMONTH(monthnum);
	nowstring += " " + to_string(daynum) + ", " + to_string(year) + " " + timestring;

	wmove(infowin, 0, 0);
	wclrtoeol(infowin);
	mvwprintw(infowin, 0, 0, "Selected: %s\n", currstring.c_str());
	wclrtoeol(infowin);
	wprintw(infowin, "Today: %s\n", nowstring.c_str());
	wrefresh(infowin);
}

void exit_handler() {
	delwin(monthwin);
	delwin(daywin);
	endwin();
}

void init_ncurses() {
	initscr();
	//start_color();
	raw();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	noecho();
	curs_set(0);

	refresh();

	currmenu = Day;

	//Create Menus
	monthwin = newwin(13, 10, 0, 0);
	currmonthnum = monthnum;
	monthcursor = monthnum - 1;
	month_menu_draw();

	daywin = newwin(13, 4, 0, 12);
	currdaynum = daynum;
	daycursor = (currdaynum < 12) ? currdaynum - 1 : 11;
	dayoffset = (currdaynum < 13) ? 1 : currdaynum - 11;
	day_menu_draw();

	infowin = newwin(LINES-13, COLS, 13, 0);
	info_win_draw();

	mvhline(12, 0, 0, COLS);
	mvhline(15, 0, 0, COLS);
	mvhline(12, 10, ACS_BTEE, 1);
	mvvline(0, 10, 0, 12);

	atexit(exit_handler);
}

