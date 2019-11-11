#include "sampler.h"


namespace sampler {
	std::uniform_int_distribution<long long> int_ran;
	std::uniform_real_distribution<long double> dbl_ran;
	std::mt19937_64 gen;

	void reset() {
		gen.seed(0LL);
	}

	long long generate_random_int(long long A, long long B) {
		decltype(int_ran.param()) new_range(A, B);
		int_ran.param(new_range);
		return int_ran(gen);
	}

	double generate_random_dbl(double A, double B) {
		decltype(dbl_ran.param()) new_range(A, B);
		dbl_ran.param(new_range);
		return dbl_ran(gen);
	}
	void reseed(int seed) {
		gen.seed(seed);
	}
}