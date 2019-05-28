#include "sampler.h"

template<class T>
std::pair<int, T> sampler::weighted_sampling(std::vector<T>& cum_weights) {
	T sum = cum_weights.back();
	T random_weight = this->generate_random_int(1, sum);
	int lo = 0, hi = (int)cum_weights.size();
	while (lo < hi) {
		int mid = (lo + hi) >> 1;
		if (cum_weights[mid] < random_weight) {
			lo = mid + 1;
		}else {
			hi = mid;
		}
	}
	return std::make_pair(lo, random_weight);
}