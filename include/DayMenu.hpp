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
		int nlines, ncols;
		int x, y;
		WINDOW *win;
		void move_cursor(int dt);
};
