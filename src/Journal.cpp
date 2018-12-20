#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <iostream>
#include <fstream>
#include "pugixml.hpp"
#include "Journal.hpp"
#include "Date.hpp"

using namespace std;
using namespace pugi;

Journal::Journal(int year, bool& success) : year{year} {
	success = false;
	struct passwd *pw = getpwuid(getuid());
	rootdir = ((string) pw->pw_dir) + "/.bullet";
	dirpath = rootdir + "/journals";
	name = to_string(year) + ".xml";
	path = dirpath + "/" + name;

	if (!Journal::file_exists(path)) {
		cerr << "Making ~/.bullet/journals/" + name << endl;
		if (!Journal::file_exists(dirpath)) {
			cerr << "Making ~/.bullet/journals" << endl;
			if (!Journal::file_exists(rootdir)) {
				cerr << "Making ~/.bullet\n";
				if (!make_dir(rootdir)) {
					cerr << "Failed to make ~/.bullet\n";
					return;
				}
			}
			if (!make_dir(dirpath)) {
				cerr << "Failed to make ~/.bullet/journals\n";
				return;
			}
		}
		if (!generate_journal()) {
			cerr << "Failed to create Bullet Journal file\n";
			return;
		}

	} else {
		//TODO Validate structure?
		xml_parse_result result = xmldoc.load_file(path.c_str());
		if (!result) {
			cerr << "Failed to load Bullet Journal\n";
			return;
		}
		calendar = xmldoc.child("calendar");
	}
	success = true;
}

Journal::~Journal() {
	if (dirty) {
		save_file();
	}
}

void Journal::set_date(int month, int day) {
	xml_node monthnode = calendar.child("month");
	for (int m = 1; m != month; m++) {
		monthnode = monthnode.next_sibling("month");
	}
	daynode = monthnode.child("day");
	for (int d = 1; d != day; d++) {
		daynode = daynode.next_sibling("day");
	}

	tasks.clear();
	for (xml_node task : daynode.children("task")) {
		tasks.push_back(task);
	}
}

void Journal::append(const std::string& str) {
	xml_node task = daynode.append_child("task");
	tasks.push_back(task);
	task.text() = str.c_str();
	dirty = true;
}

std::string Journal::get(int i) {
	return string(tasks[i].text().get());
}

void Journal::set(int i, const std::string& str) {
	tasks[i].text() = str.c_str();
	dirty = true;
}

void Journal::remove(int i) {
	if (tasks.empty()) {
		return;
	}
	daynode.remove_child(tasks[i]);
	tasks.erase(tasks.begin()+i);
	dirty = true;
}

bool Journal::file_exists(const std::string& path) {
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

bool Journal::make_dir(const std::string& path) {
	return !mkdir(path.c_str(), 0777);
}

bool Journal::generate_journal() {
	calendar = xmldoc.append_child("calendar");
	if (!calendar) {
		return false;
	}
	calendar.append_attribute("year") = year;
	for (int m = 1; m <= 12; m++) {
		xml_node month = calendar.append_child("month");
		if (!month) {
			return false;
		}
		int numdays = NUM_DAYS(year, m);
		for (int d = 1; d <= numdays; d++) {
			xml_node day = month.append_child("day");
			if (!day) {
				return false;
			}
		}
	}
	dirty = true;
	return true;
}

bool Journal::save_file() {
	ofstream file(path, ofstream::trunc);
	if (file.fail()) {
		cerr << "Could not open file for saving\n";
		return false;
	}
	xmldoc.save(file);
	file.close();
	dirty = false;
	return true;
}
