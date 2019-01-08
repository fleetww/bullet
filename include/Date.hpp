#include "string"

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

char leap_year(int year);
char day_of_week(int y, int m, int d);

#define NUM_DAYS(Y,M) (daytab[leap_year(Y)][M])
#define DAYOFWEEK(Y,M,D) (weeknames[day_of_week(Y,M,D)])
#define DAYOFMONTH(M) (monthnames[M-1])
