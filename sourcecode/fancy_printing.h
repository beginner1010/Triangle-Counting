#pragma once
#include <iostream>
#include <string>
#include <iomanip>

#include "utills.h"

namespace print {
	void done_work_percentage(double done_work, double total_work);
	void reading_graph_fancy_text(bool done, double &last_printted_dot, int max_fancy_text_width, int& n_dots, double& cur_time, double& total_work, double& done_work);
	void statistics_table(int n, int m, int maximum_degree);
	void print_header();
}