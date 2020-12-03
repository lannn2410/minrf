#include "Random0.h"
#include <algorithm>
Random0::Random0(DataSet* d, vector<vector<double>> thresholds) : Algorithm(d, thresholds)
{
	alg_0_type = ALG_0::RANDOM;
	uint n = d->get_n();
	for (int i = 0; i < n; ++i) random_order.push_back(i);
	random_shuffle(random_order.begin(), random_order.end());
}

void Random0::get_solutions(vector<uint>& seeds)
{
	uint n = data->get_n();
	uint min_e = n;
	#pragma omp parallel for
	for (int e = 1; e <= n; ++e) {
		if (e < min_e) {
			vector<uint> sub_v(random_order.begin(), random_order.begin() + e);
			sort(sub_v.begin(), sub_v.end());
			vector<uint> seed_tmp = common_instance->set_merge(sub_v, seeds);
			double dummy1 = 0.0, dummy2 = 0.0;
			vector<uint> sat_cons;
			sum_f(seed_tmp, constr, sat_cons, dummy1, dummy2);
			if (sat_cons.size() == constr.size()) {
				#pragma omp critical
				{
					if (e < min_e) min_e = e;
				}
			}
		}
	}

	// append to seeds
	vector<uint> sub_v(random_order.begin(), random_order.begin() + min_e);
	sort(sub_v.begin(), sub_v.end());
	seeds = common_instance->set_merge(sub_v, seeds);

	//while (!constr.empty()) {
	//	uint sel = Common::getInstance()->randomInThread(omp_get_thread_num()) % n;
	//	while (checked[sel]) sel = Common::getInstance()->randomInThread(omp_get_thread_num()) % n;
	//	insert_sorted(seeds, sel);
	//	checked[sel] = true;
	//	double dummy1 = 0.0, dummy2 = 0.0;
	//	vector<uint> sat_cons;
	//	sum_f(seeds, constr, sat_cons, dummy1, dummy2);
	//	if (!sat_cons.empty()) { // remove satisfied constraints
	//		for (uint c : sat_cons) constr.erase(constr.begin() + c);
	//	}
	//}
}
