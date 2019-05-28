#pragma once
#include <random>

class sampler {
	std::uniform_int_distribution<int> int_ran;
	std::uniform_real_distribution<double> dbl_ran;
	std::mt19937_64 gen;
public:
	sampler() {
		this->gen.seed(0LL);
	}
	template<class U>
	U generate_random_int(U A, U B) {
		decltype(int_ran.param()) new_range(A, B);
		int_ran.param(new_range);
		return int_ran(gen);
	}
	double generate_random_dbl(double A, double B) {
		decltype(dbl_ran.param()) new_range(A, B);
		dbl_ran.param(new_range);
		return dbl_ran(gen);
	}
	void reseed(int seed = 0) {
		this->gen.seed(seed);
	}

	template<class T>
	std::pair<int, T> weighted_sampling(std::vector<T>& weights);
};