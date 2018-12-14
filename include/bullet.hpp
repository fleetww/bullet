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
 * Date and time functions and static data structures
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
#define SELECTED_DAYNUM(i) (i + dayoffset)
#define DAYOFWEEK(Y,M,D) (weeknames[day_of_week(Y,M,D)])
#define DAYOFMONTH(M) (monthnames[M-1])

static int year, monthnum, daynum;

static struct tm *currenttime;


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
 * User Interface (UI)
 */

void init_ncurses();


enum MenuState {
	Month,
	Day
} static currmenu;

static WINDOW *monthwin;
static int monthcursor, currmonthnum;
static bool monthmenudirty;
void month_menu_draw();
void month_menu_update(int input);
void month_menu_cursor_up();
void month_menu_cursor_down();

static WINDOW *daywin;
static int daycursor, currdaynum, dayoffset;
static bool daymenudirty;
void day_menu_draw();
void day_menu_update(int input);
void day_menu_cursor_up();
void day_menu_cursor_down();

static WINDOW *infowin;
void info_win_draw();

#define KEY_ESC 27
#define KEY_INTR 3

void exit_handler();
