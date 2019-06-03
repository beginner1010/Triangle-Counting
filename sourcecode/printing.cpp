#include "printing.h"

namespace print {
	void done_experiments(const int& iter_exp) {
		fprintf(stderr, " %d out of %d experiments is finished.", iter_exp - 1, settings::exp_repeatition);
	}

	void done_work_percentage(double done_work, double total_work, std::string text) {
		fprintf(stderr, "\t[%7.3f%% of %s is done]", mmin(100.0, ((double)done_work / total_work * 100)), text.c_str());
	}

	void print_dots(int& n_dots) {
		for (int dots = 0; dots < n_dots; dots++)
			std::cerr << '.';
		for (int dots = n_dots; dots < 4; dots++)
			std::cerr << ' '; 
		n_dots = (n_dots + 1) % 4;
	}

	void message_with_dots(double &last_print_time, int& n_dots, double& cur_time, double total_work, double done_work, const std::string text) {
		if (cur_time - last_print_time > 1.0) {
			clear_line();
			std::cerr << " " << text ;
			print_dots(n_dots);
			done_work_percentage(done_work, total_work, text);
			std::cerr.flush();
			last_print_time = cur_time;
		}
	}

	void finished_work(const double& cur_time, const std::string text) {
		std::cerr << " " << text << " in " << cur_time << " secs." << std::endl;
		std::cerr.flush();
	}

	void clear_line() {
		std::cerr << "\r" << std::string(100, ' ') << "\r" ;
	}

	void statistics_table(int n, int m, int maximum_degree) {
		std::cerr << " See the statistics below :" << std::endl;
		
		int width = (5 * 2 - 1) + 12 + 15 + 10;
		std::string hline = " " + std::string(width, '-');
		std::cerr << hline << std::endl;
		std::cerr << " |" << std::setw(12) << "#edges" << " |" << std::setw(15) << "#vertices" << " |" << std::setw(12) << "max degree" << " |" << std::endl;
		std::cerr << hline << std::endl;
		std::cerr << " |" << std::setw(12) << m << " |" << std::setw(15) << n << " |" << std::setw(12) << maximum_degree << " |" << std::endl;
		std::cerr << hline << std::endl;
		std::cerr.flush();
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
		else if (settings::chosen_algo == ONE_SHOT_EDGE_WEDGE_SAMPLING) {
			fprintf(IO::fout, "itr, algo, prob, bfly, time");
		}
		else if (settings::chosen_algo == LOCAL_WEDGE_SAMPLING) {
			fprintf(IO::fout, "itr, algo, preproc time, #sampled wedge, bfly, time");
		}
		else if (settings::chosen_algo == LOCAL_REVISITED_WEDGE_SAMPLING) {
			fprintf(IO::fout, "itr, algo, preproc time, #sampled wedge, bfly, time");
		}
		fprintf(IO::fout, "\n");
		std::fflush(IO::fout);
	}

	void print_result(std::vector <double> res) {
		if (settings::chosen_algo == EXACT) {
			fprintf(IO::fout, "Exact, %.2f, %.4f", res[0], res[1]);
		}
		else if (settings::chosen_algo == ONE_SHOT_DOULION) {
			fprintf(IO::fout, "%d, Doulion, %.2f, %.2f, %.4f", (int)(res[0] + 1e-6), settings::p, res[1], res[2]);
		}
		else if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
			fprintf(IO::fout, "%d, Colorful, %d, %.2f, %.4f", (int)(res[0] + 1e-6), settings::n_colors, res[1], res[2]);
		}
		else if (settings::chosen_algo == ONE_SHOT_EDGE_WEDGE_SAMPLING) {
			fprintf(IO::fout, "%d, EWSamp, %.2f, %.2f, %.4f", (int)(res[0] + 1e-6), settings::p, res[1], res[2]);
		}
		else if (settings::chosen_algo == LOCAL_WEDGE_SAMPLING) {
			fprintf(IO::fout, "%d, Wedge Samp, %.2f, %d, %.2f, %.4f", (int)(res[0] + 1e-6), res[1], (int)(res[2] + 1e-6), res[3], res[4]);
		}
		else if (settings::chosen_algo == LOCAL_REVISITED_WEDGE_SAMPLING) {
			fprintf(IO::fout, "%d, Rev Wedge Samp, %.2f, %d, %.2f, %.4f", (int)(res[0] + 1e-6), res[1], (int)(res[2] + 1e-6), res[3], res[4]);
		}
		fprintf(IO::fout, "\n");
		std::fflush(IO::fout);
	}
}