#include "Greedy0.h"
#include <iostream>

Greedy0::Greedy0(DataSet* d, vector<vector<double>> thresholds) : Algorithm(d, thresholds)
{
	alg_0_type = ALG_0::GREEDY;
}

void Greedy0::get_solutions(vector<uint>& seeds)
{
	uint n = data->get_n();
	vector<bool> checked(n, false);
	for (uint e : seeds) checked[e] = true;

	while (!constr.empty() && seeds.size() < n) {
		uint max_id = -1;
		double max_f_old = -1;
		vector<uint> max_sat_cons;

		// get the one with maximum marginal gain
		#pragma omp parallel for
		for (int i = 0; i < n; ++i) {
			if (!checked[i]) {
				double sum_f_new = 0.0, sum_f_old = 0.0;
				vector<uint> sat_cons;
				vector<uint> seed_tmp(seeds);
				common_instance->insert_sorted(seed_tmp, i);
				sum_f(seed_tmp, constr, sat_cons, sum_f_old, sum_f_new);
				if (sum_f_old > max_f_old) {
					#pragma omp critical
					{
						if (sum_f_old > max_f_old) {
							max_f_old = sum_f_old;
							max_id = i;
							max_sat_cons = sat_cons;
						}
					}
				}
			}
		}

		//cout << max_id << endl;

		common_instance->insert_sorted(seeds, max_id);
		checked[max_id] = true;
		if (!max_sat_cons.empty()) { // remove satisfied constraints
			for (uint c : max_sat_cons) constr.erase(constr.begin() + c);
		}
	}
}
