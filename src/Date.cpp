#include "Date.hpp"

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
