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

#define NUM_DAYS(Y,M) (daytab[leap_year(Y)][M])
#define DAYOFWEEK(Y,M,D) (weeknames[day_of_week(Y,M,D)])
#define DAYOFMONTH(M) (monthnames[M-1])

static char daytab[2][13] = {
	{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

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
