#pragma once
#include <random>

namespace sampler {
	extern std::uniform_int_distribution<long long> int_ran;
	extern std::uniform_real_distribution<long double> dbl_ran;
	extern std::mt19937_64 gen;
	void reset();

	long long generate_random_int(long long A, long long B);

	double generate_random_dbl(double A, double B);
	void reseed(int seed = 0);

	template<typename T>
	std::pair<int, T > weighted_sampling(const std::vector<T>& cum_weights) {
		T sum = cum_weights.back();
		T random_weight = generate_random_dbl(1, sum); // generating random doubles with floating point should not harm the method. The floating points are ignored in Graph::get_wedge
		int lo = 0, hi = (int)cum_weights.size();
		while (lo < hi) {
			int mid = (lo + hi) >> 1;
			if (cum_weights[mid] < random_weight) {
				lo = mid + 1;
			}
			else {
				hi = mid;
			}
		}
		return std::make_pair(lo, random_weight);
	}
}