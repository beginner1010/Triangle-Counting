#pragma once

#include <iostream>
#include <string>
#include <iomanip>

#include "utills.h"

namespace print {
	void done_experiments(const int& iter_exp);
	void done_work_percentage(double done_work, double total_work, std::string text);
	void print_dots(int& n_dots);
	void message_with_dots(double& last_print_time, int& n_dots, double& cur_time, double total_work, double done_work, const std::string text);
	void finished_work(const double& cur_time, const std::string text);
	void clear_line();
	void statistics_table(int n, int m, int maximum_degree);
	void print_header();
	void print_result(std::vector <double> res);
}