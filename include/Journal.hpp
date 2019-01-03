#include <vector>
#include "pugixml.hpp"
class Journal {
	public:
		Journal(int nyear, bool& success);
		~Journal();
		void set_date(int month, int day);
		int size() { return tasks.size();}
		void append(const std::string& str);
		std::string get(int i);
		void set(int i, const std::string& str);
		void remove(int i);
	private:
		int year;
		bool dirty = false;
		std::string name, path, dirpath, rootdir;
		pugi::xml_document xmldoc;
		pugi::xml_node calendar, daynode;
		std::vector<pugi::xml_node> tasks;
		static bool file_exists(const std::string& path);
		static bool make_dir(const std::string& path);
		bool generate_journal();
		bool save_file();
};
