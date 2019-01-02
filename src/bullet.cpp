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

	fd_set rfds;
	struct timeval tv;
	int retval = 0;
	while (1) {
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		retval = select(1, &rfds, NULL, NULL, &tv);
		if (retval == -1 && errno != EINTR) {
			perror("select()");
		}
		time_win_draw();

		if (!FD_ISSET(0, &rfds)) {
			continue;
		}
		int ch = getch();
		if (ch == 'q') {
			break;
		}

		switch (currmenu) {
			case Month:
				month_menu_update(ch);
				break;
			case Day:
				day_menu_update(ch);
				break;
			case Task:
				task_menu_update(ch);
				break;
			case Edit:
				edit_prompt_update(ch);
				break;
		}
	}

	exit(0);
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
		int maxmonth = (year < y ? 12 : m - 1);
		for (int month = 1; month <= maxmonth; month++) {
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

bool make_bullet_journal(int year) {
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
	bulletdirty = true;
	return true;
}

int init_bullet_journal() {
	struct passwd *pw = getpwuid(getuid());
	bulletdirpath = ((string) pw->pw_dir) + "/.bullet/journals";
	journalname = to_string(year) + ".xml";
	journalpath = bulletdirpath + "/" + journalname;

	//handle .bullet dir not existing, as well as year.xml not existing
	if (!file_exists(journalpath)) {
		cerr << "Bullet Journal (~/.bullet/journals/" << journalname
			<< ") does not exists, creating now\n";
		if (!file_exists(bulletdirpath)) {
			cerr << "Bullet Journal Directory (~/.bullet) does not exist, creating now\n";
			if (!make_bullet_dir(bulletdirpath)) {
				cerr << "Failed to create Bullet Journal Directory\n";
				return 2;
			}
		}
		if (!make_bullet_journal(year)) {
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
	calendar = bulletdoc.child("calendar");
	return 0;
}

void save_bullet_journal() {
	if (!bulletdirty) {
		return;
	}

	ofstream file(journalpath, ofstream::trunc);
	if (file.fail()) {
		cerr << "Could not open file for saving\n";
	}
	bulletdoc.save(file);
	file.close();

	bulletdirty = false;
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
		case 10:
		case 'l':
			month_to_day_menu();
			break;
		case 'k':
			month_menu_cursor_up();
			break;
		case 'j':
			month_menu_cursor_down();
			break;
	}
}

void month_menu_cursor_up() {
	if (monthcursor == 0) {
		return;
	}
	monthcursor--;
	if ((monthcursor + 1) == currmonthnum) {
		if (currdaynum < dayoffset) {
			dayoffset = currdaynum;
		} else if ((dayoffset + 11) < currdaynum) {
			dayoffset = currdaynum - 11;
		}
		daycursor = currdaynum - dayoffset;
	} else {
		dayoffset = 1;
		daycursor = 0;
	}
	month_menu_draw();
	day_menu_draw();
}

void month_menu_cursor_down() {
	if (monthcursor == 11) {
		return;
	}
	monthcursor++;
	if ((monthcursor + 1) == currmonthnum) {
		if (currdaynum < dayoffset) {
			dayoffset = currdaynum;
		} else if ((dayoffset + 11) < currdaynum) {
			dayoffset = currdaynum - 11;
		}
		daycursor = currdaynum - dayoffset;
	} else {
		dayoffset = 1;
		daycursor = 0;
	}
	month_menu_draw();
	day_menu_draw();
}

void month_to_day_menu() {
	currmenu = Day;
	month_menu_draw();
	day_menu_draw();
}

void day_menu_draw() {
	werase(daywin);
	int slcdaynum, len;
	for (int i = 0; i < 12; i++) {
		slcdaynum = SELECTED_DAYNUM(i);
		mvwprintw(daywin, i, 0, "%d", slcdaynum);

		len = (slcdaynum > 9) ? 2 : 1;
		if ((monthcursor + 1) == currmonthnum && slcdaynum == currdaynum) {
			mvwchgat(daywin, i, 0, len, A_UNDERLINE, 0, NULL);
		}
	}
	slcdaynum = SELECTED_DAYNUM(daycursor);
	len = (slcdaynum > 9) ? 2 : 1;
	int effect = (currmenu == Day) ? A_BLINK|A_STANDOUT : A_STANDOUT;
	mvwchgat(daywin, daycursor, 0, len, effect, 0, NULL);
	wrefresh(daywin);
}

void day_menu_update(int input) {
	switch (input) {
		case 'h':
			day_to_month_menu();
			break;
		case 'k':
			day_menu_cursor_up();
			break;
		case 'j':
			day_menu_cursor_down();
			break;
		case 10:
			select_date();
			break;
		//TODO add cancel functionality
	}
}

void day_menu_cursor_up() {
	if (daycursor > 0) {
		daycursor--;
		day_menu_draw();
	} else if (SELECTED_DAYNUM(daycursor) > 1) {
		dayoffset--;
		day_menu_draw();
	}
}

void day_menu_cursor_down() {
	if (daycursor < 11) {
		daycursor++;
		day_menu_draw();
	} else if (SELECTED_DAYNUM(daycursor) < NUM_DAYS(year, monthcursor + 1)){
		dayoffset++;
		day_menu_draw();
	}
}

void day_to_month_menu() {
	if ((monthcursor + 1) == currmonthnum && SELECTED_DAYNUM(daycursor) != currdaynum) {
		if (currdaynum < dayoffset) {
			dayoffset = currdaynum;
		} else if ((dayoffset + 11) < currdaynum) {
			dayoffset = currdaynum - 11;
		}
		daycursor = currdaynum - dayoffset;
	}
	currmenu = Month;
	day_menu_draw();
	month_menu_draw();
}

void time_win_draw() {
	init_date();
	char buffer[38];
	strftime(buffer, 38, "%A %B %d, %Y %I:%M %p", currenttime);

	werase(timewin);
	mvwprintw(timewin, 0, 0, "Today's Date:\n%s", buffer);
	mvwchgat(timewin, 1, 0, strlen(buffer), A_BOLD|A_UNDERLINE, 0, NULL);
	wrefresh(timewin);
}

void info_win_draw() {
	string currstring = DAYOFWEEK(year,currmonthnum,currdaynum) + " " +
		DAYOFMONTH(currmonthnum) + " " + to_string(currdaynum) + ", " + to_string(year);

	werase(infowin);
	mvwprintw(infowin, 0, 0, "Selected Date:\n%s", currstring.c_str());
	mvwchgat(infowin, 1, 0, currstring.size(), A_BOLD|A_UNDERLINE, 0, NULL);
	wrefresh(infowin);
}

void task_menu_draw() {
	werase(taskwin);

	int t = 0;
	while (SELECTED_TASK(t) < tasks.size() && t < (LINES-14)) {
		mvwprintw(taskwin, t, 0, "%s", tasks[SELECTED_TASK(t)].text().get());
		t++;
	}

	if (!tasks.empty()) {
		int len = strlen(tasks[SELECTED_TASK(taskcursor)].text().as_string());
		mvwchgat(taskwin, taskcursor, 0, len, A_UNDERLINE|A_BOLD, 0, NULL);
	}
	wrefresh(taskwin);
}

void task_menu_update(int input) {
	switch (input) {
		case ':':
			currmenu = Day;
			day_menu_draw();
			break;
		case 'k':
			task_menu_cursor_up();
			break;
		case 'j':
			task_menu_cursor_down();
			break;
		case 10:
			if (tasks.size() > 0)
				select_task();
			break;
		case 4: //Ctrl-d
			delete_task();
			break;
		case 14: //Ctrl-n
			append_task();
			break;
		case 19: //Ctrl-s
			save_bullet_journal();
			break;
	}
}

void task_menu_cursor_up() {
	if (SELECTED_TASK(taskcursor) <= 0) {
		return;
	} else if (taskcursor > 0) {
		taskcursor--;
	} else {
		taskoffset--;
	}
	task_menu_draw();
}

void task_menu_cursor_down() {
	if (SELECTED_TASK(taskcursor) >= tasks.size()-1) {
		return;
	} else if ((taskcursor+1) < (LINES-14)) {
		taskcursor++;
	} else {
		taskoffset++;
	}
	task_menu_draw();
}

void delete_task() {
	if (tasks.empty()) {
		return;
	}
	daynode.remove_child(tasks[SELECTED_TASK(taskcursor)]);
	tasks.erase(tasks.begin()+SELECTED_TASK(taskcursor));
	task_menu_cursor_up();
	bulletdirty = true;
	task_menu_draw();
}

void append_task() {
	xml_node task = daynode.append_child("task");
	tasks.push_back(task);
	currtasknum = tasks.size() - 1;

	//scroll task menu to new task
	if (tasks.size() > (LINES-14)) {
		taskoffset = tasks.size() - (LINES-14);
		taskcursor = (LINES-15);
	} else {
		taskoffset = 0;
		taskcursor = currtasknum;
	}

	editcursor = 0;
	editbuffer.clear();
	task.text() = "";
	currmenu = Edit;
	bulletdirty = true;
	task_menu_draw();
	edit_prompt_draw();
	makingnewtask = true;
}

void select_task() {
	currtasknum = SELECTED_TASK(taskcursor);
	currmenu = Edit;

	editbuffer = tasks[currtasknum].text().get();
	editcursor = strlen(tasks[currtasknum].text().get());
	edit_prompt_draw();
}

void edit_prompt_draw() {
	curs_set(1);
	wmove(taskwin, taskcursor, 0);
	wclrtoeol(taskwin);
	mvwprintw(taskwin, taskcursor, 0, "%s", editbuffer.c_str());
	mvwchgat(taskwin, taskcursor, 0, editbuffer.size(), A_STANDOUT, 0, NULL);
	wmove(taskwin, taskcursor, editcursor);
	wrefresh(taskwin);
}

void edit_prompt_update(int input) {
	switch (input) {
		case KEY_ESC:
			edit_prompt_cancel();
			break;
		case KEY_BACKSPACE:
			edit_prompt_bs();
			break;
		case 10:
			edit_prompt_finish();
			break;
		case KEY_LEFT:
			edit_cursor_left();
			break;
		case KEY_RIGHT:
			edit_cursor_right();
			break;
		default:
			if (isprint(input)) {
				edit_prompt_insert(input);
			}
			break;
	}
}

void edit_prompt_cancel() {
	if (makingnewtask) {
		delete_task();
		makingnewtask = false;
	}
	editbuffer.clear();
	currmenu = Task;
	curs_set(0);
	task_menu_draw();
}

void edit_prompt_bs() {
	if (editcursor <= 0) {
		return;
	}
	editbuffer.erase(--editcursor, 1);
	edit_prompt_draw();
}

void edit_prompt_finish() {
	bulletdirty = true;
	tasks[currtasknum].text() = editbuffer.c_str();
	makingnewtask = false;
	edit_prompt_cancel();
}

void edit_cursor_left() {
	if (editcursor > 0) editcursor--;
	edit_prompt_draw();
}

void edit_cursor_right() {
	if (editcursor < editbuffer.size()) editcursor++;
	edit_prompt_draw();
}

void edit_prompt_insert(int input) {
	editbuffer.insert(editcursor++, 1, (char)input);
	edit_prompt_draw();
}

void cache_tasks() {
	xml_node month = calendar.child("month");
	for (int m = 1; m != currmonthnum; m++) {
		month = month.next_sibling("month");
	}
	daynode = month.child("day");
	for (int d = 1; d != currdaynum; d++) {
		daynode = daynode.next_sibling("day");
	}

	tasks.clear();
	for (xml_node task : daynode.children("task")) {
		tasks.push_back(task);
	}
}

void select_date() {
	currmonthnum = monthcursor + 1;
	currdaynum = SELECTED_DAYNUM(daycursor);
	cache_tasks();

	currmenu = Task;

	month_menu_draw();
	day_menu_draw();
	info_win_draw();
	task_menu_draw();
}

void exit_handler() {
	delwin(monthwin);
	delwin(daywin);
	delwin(taskwin);
	delwin(infowin);
	delwin(timewin);
	endwin();
	save_bullet_journal();
}

void init_ncurses() {
	initscr();
	//start_color();
	raw();
	keypad(stdscr, TRUE);
	//nodelay(stdscr, TRUE);
	noecho();
	curs_set(0);

	refresh();

	currmenu = Task;

	//Create interface
	monthwin = newwin(12, 9, 0, 0);
	currmonthnum = monthnum;
	monthcursor = monthnum - 1;
	month_menu_draw();


	daywin = newwin(12, 2, 0, 10);
	currdaynum = daynum;
	daycursor = (currdaynum <= 12) ? currdaynum - 1 : 11;
	dayoffset = (currdaynum < 13) ? 1 : currdaynum - 11;
	day_menu_draw();

	timewin = newwin(2, 37, 0, 13);
	time_win_draw();

	infowin = newwin(2, 37, 3, 13);
	info_win_draw();

	mvvline(0, 9, 0, 12);
	refresh(); //makes daymenu not blank, don't know why
	mvvline(0, 12, 0, 12);
	mvhline(12, 0, 0, COLS);
	mvhline(12, 9, ACS_BTEE, 1);
	mvhline(12, 12, ACS_BTEE, 1);

	taskcursor = 0;
	currtasknum = 0;
	taskoffset = 0;
	taskwin = newwin(LINES-14, COLS, 13, 0);
	cache_tasks();
	task_menu_draw();

	refresh();

	atexit(exit_handler);
}

