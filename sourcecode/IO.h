#pragma once
#include <string>
#include <fstream>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif
#ifdef _WIN32
#include <io.h> 
#define access    _access_s
#else
#include <unistd.h>
#endif


#include "utills.h"


namespace IO {
	extern std::string input_address;
	extern std::string output_address;
	extern std::string input_file_name;
	extern FILE* fout;
	extern FILE* fin;
	double get_file_size();
	void create_folder();
	bool check_if_file_exists();
}

