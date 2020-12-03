#include "RandomGreedy0.h"
#include <algorithm>


RandomGreedy0::RandomGreedy0(DataSet * d, vector<vector<double>> thresholds) : Algorithm(d, thresholds)
{
	alg_0_type = ALG_0::RAND_GREEDY;
}

void RandomGreedy0::get_solutions(vector<uint>& seeds)
{
	uint n = data->get_n();
	vector<bool> checked(n, false);
	for (uint e : seeds) checked[e] = true;

	while (!constr.empty() && seeds.size() < n) {
		// random select constraints
		vector<Constraint> sel_cons;
		vector<uint> sel_idx;
		for (int i = 0; i < constr.size(); ++i) sel_idx.push_back(i);
		random_shuffle(sel_idx.begin(), sel_idx.end());
		uint mtmp = sel_idx.size() > 2 ? sel_idx.size() / 2 : sel_idx.size(); // get only half the constraint
		for (int i = 0; i < mtmp; ++i) sel_cons.push_back(constr[sel_idx[i]]);

		// get the one with maximum marginal gain
		uint max_id = -1;
		double max_f_old = -1;
		#pragma omp parallel for
		for (int i = 0; i < n; ++i) {
			if (!checked[i]) {
				double sum_f_new = 0.0, sum_f_old = 0.0;
				vector<uint> sat_cons;
				vector<uint> seed_tmp(seeds);
				common_instance->insert_sorted(seed_tmp, i);
				sum_f(seed_tmp, sel_cons, sat_cons, sum_f_old, sum_f_new);
				if (sum_f_old > max_f_old) {
					#pragma omp critical
					{
						if (sum_f_old > max_f_old) {
							max_f_old = sum_f_old;
							max_id = i;
						}
					}
				}
			}
		}

		// add new e into seeds and remove satisfied constraints
		common_instance->insert_sorted(seeds, max_id);
		checked[max_id] = true;
		vector<uint> sat_cons;
		double dummy1, dummy2;
		sum_f(seeds, constr, sat_cons, dummy1, dummy2);
		if (!sat_cons.empty()) { // remove satisfied constraints
			for (uint c : sat_cons) constr.erase(constr.begin() + c);
		}
	}
}

RandomGreedy0::~RandomGreedy0()
{
}
