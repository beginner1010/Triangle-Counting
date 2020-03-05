#include "sampler.h"

std::pair<int, long long > sampler::weighted_sampling(const std::vector<long long>& cum_weights) {
	long long sum = cum_weights.back();
	long long random_weight = this->generate_random_int(1, sum);
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