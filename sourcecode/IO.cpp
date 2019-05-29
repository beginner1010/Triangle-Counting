#include "IO.h"


namespace IO {
	std::string input_address;
	std::string output_address;
	std::string input_file_name;
	FILE* fout;
	FILE* fin;
	double get_file_size() {
		std::ifstream in(input_address.c_str(), std::ifstream::ate | std::ifstream::binary);
		return (double)in.tellg();
	}
	void create_folder() {
		std::string folder_name = constants::folder_algo_name[settings::chosen_algo];
#ifdef _WIN32
		::_mkdir("./output");
		::_mkdir(("./output/" + IO::input_file_name).c_str());
		::_mkdir(("./output/" + IO::input_file_name + "/" + folder_name).c_str());
#else
		::mkdir("./output", 0777);
		::mkdir(("./output/" + IO::input_address).c_str(), 0777);
		::mkdir(("./output/" + IO::input_address + "/" + folder_name).c_str(), 0777);
#endif
		std::string file_name = "";
		file_name += settings::max_time == -1.0 ? "" : "t=" + helper_functions::to_str((int)settings::max_time);
		file_name += settings::exp_repeatition == -1 ? "" : "rep=" + helper_functions::to_str(settings::exp_repeatition);
		file_name = folder_name == "exact" ? "exact" : file_name;
		IO::output_address = "output/" + IO::input_file_name + "/" + folder_name + "/" + file_name + ".txt";
		return;
	}

	bool check_if_file_exists() {
		if (access(IO::input_address.c_str(), 0) != 0) {
			std::cerr << " No such file exists!" << std::endl;
			return false;
		}
		return true;
	}
 
}

