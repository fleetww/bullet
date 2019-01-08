#include <ncurses.h>

class	MonthMenu {
	public:
		MonthMenu(const int &currMonth);
		~MonthMenu();
		void draw();
		void update(int input);
		int get_curr_num();
		int get_cursor();
		bool active;
	private:
		int currnum, cursor;
		int nlines = 12, ncols = 9;
		int x = 0, y = 0;
		WINDOW *win;
};
