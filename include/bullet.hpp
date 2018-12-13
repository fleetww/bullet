#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>
#include <ncurses.h>
#include "pugixml.hpp"

/*
 * Date functions and static data structures
 */
static std::string weeknames[7] = {
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"
};

static std::string monthnames[12] = {
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

static char daytab[2][13] = {
	{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

void init_date();
char leap_year(int year);
char day_of_week(int y, int m, int d);
#define NUM_DAYS(X,Z) (daytab[leap_year(X)][Z])

int year, monthnum, daynum;

/*
 * XML related functions and data structures
 */
static pugi::xml_document bulletdoc;
static std::string bulletdirpath, journalname, journalpath;

int init_bullet_journal();
bool file_exists(const std::string& path);
bool make_bullet_dir(const std::string& path);
bool make_bullet_journal(const std::string& path, int year);

/*
 * ncurses related functions and data structures
 */
WINDOW *monthwin;
int monthcursor, currmonthnum;

WINDOW *daywin;
int daycursor, currdaynum, dayoffset;

void init_ncurses();

#define SELECTED_DAYNUM(i) (i + dayoffset)

enum MenuState {
	Month,
	Day
} menu;

bool monthmenudirty;
void month_menu_draw();
void month_menu_update(int input);
void month_menu_cursor_up();
void month_menu_cursor_down();

bool daymenudirty;
void day_menu_draw();
void day_menu_update(int input);
void day_menu_cursor_up();
void day_menu_cursor_down();

#define KEY_ESC 27
#define KEY_INTR 3

void exit_handler();
