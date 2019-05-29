#include "fancy_printing.h"

namespace print {
	void done_work_percentage(double done_work, double total_work) {
		fprintf(stderr, "\t[%7.3f%% of file is processed]", ((double)done_work / total_work * 100));
	}
	void reading_graph_fancy_text(bool done, double &last_printted_dot, int max_fancy_text_width, int& n_dots, double& cur_time, double& total_work, double& done_work) {
		if (done == false) {
			if (cur_time - last_printted_dot > 1.0) {
				std::cerr << "\r";
				std::cerr << std::string(max_fancy_text_width, ' ');
				std::cerr << "\r";
				std::cerr << " Processing the input graph";
				for (int dots = 0; dots < n_dots; dots++)
					std::cerr << ".";
				print::done_work_percentage(done_work, total_work);
				n_dots = (n_dots + 1) % 4;
				std::cerr.flush();
				last_printted_dot = cur_time;
			}
		}
		else {
			std::cerr << "\r";
			std::cerr << std::string(max_fancy_text_width, ' ');
			std::cerr << "\r";
			std::cerr << " The input graph is processed in " << cur_time << "(secs). See the statistics below:";
			std::cerr.flush();
		}
	}
	void statistics_table(int n, int m, int maximum_degree) {
		int width = (5 * 2 - 1) + 12 + 15 + 10;
		std::string hline = " " + std::string(width, '-');
		std::cerr << std::endl;
		std::cerr << hline << std::endl;
		std::cerr << " |" << std::setw(12) << "#edges" << " |" << std::setw(15) << "#vertices" << " |" << std::setw(12) << "max degree" << " |" << std::endl;
		std::cerr << hline << std::endl;
		std::cerr << " |" << std::setw(12) << m << " |" << std::setw(15) << n << " |" << std::setw(12) << maximum_degree << " |" << std::endl;
		std::cerr << hline << std::endl;
	}

	void print_header() {
		if (settings::chosen_algo == EXACT) {
			fprintf(IO::fout, "algo, bfly, time");
		}
		else if (settings::chosen_algo == ONE_SHOT_DOULION) {
			fprintf(IO::fout, "itr, algo, prob, bfly, time");
		}
		else if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
			fprintf(IO::fout, "itr, algo, n_colors, bfly, time");
		}

		fprintf(IO::fout, "\n");
	}
}