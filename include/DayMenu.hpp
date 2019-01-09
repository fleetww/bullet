#include <ncurses.h>

class DayMenu {
	public:
		DayMenu(int currDay);
		~DayMenu();
		void draw();
		void update(int input);
		int get_curr_num();
		int get_cursor();
		bool active;
	private:
		int currnum, cursor;
		int nlines = 12, ncols = 9;
		int x = 10, y = 0;
		WINDOW *win;
		void move_cursor(int dt);
};
