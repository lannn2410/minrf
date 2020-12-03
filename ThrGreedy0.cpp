#include "ThrGreedy0.h"
#include "Constants.h"

ThrGreedy0::ThrGreedy0(DataSet* d, vector<vector<double>> thresholds) : Algorithm(d, thresholds)
{
	alg_0_type = ALG_0::THRESHOLD;
}

void ThrGreedy0::get_solutions(vector<uint>& seeds)
{
	uint n = data->get_n();
	vector<bool> checked(n, false);
	for (uint e : seeds) checked[e] = true;

	// initiate r
	double r = 0.0;
	double dummy1 = 0.0;

	#pragma omp parallel for
	for (int i = 0; i < n; ++i) {
		if (!checked[i]) {
			vector<uint> vtmp(seeds);
			common_instance->insert_sorted(vtmp, i);
			double j = 0.0;
			vector<uint> dummy2;
			sum_f(vtmp, constr, dummy2, j, dummy1);
			if (j > r) {
				#pragma omp critical
				{
					if (j > r) r = j;
				}
			}
		}
	}

	// get initial sum_f of seeds
	double init_f = 0.0;
	vector<uint> dummy2;
	sum_f(seeds, constr, dummy2, init_f, dummy1);
	r -= init_f;

	int sel_e = -1;
	while (!constr.empty()) {
		bool stop = false;
		vector<uint> done_cons;
		double curr_f = 0;

		#pragma omp parallel for
		for (int e = 0; e < n; ++e) {
			if (!checked[e] && !stop) {
				vector<uint> stmp(seeds);
				common_instance->insert_sorted(stmp, e);
				vector<uint> sat_cons;
				double sum_f_old = 0.0, sum_f_new = 0.0;
				sum_f(stmp, constr, sat_cons, sum_f_old, sum_f_new);
				if ((sum_f_old - init_f) >= r) { // if e can be added
					#pragma omp critical
					{
						if (!stop) { // stop for loop
							stop = true;
							sel_e = e;
							done_cons = sat_cons;
							curr_f = sum_f_new;
						}
					}
				}
			}
		}

		if (stop) {
			common_instance->insert_sorted(seeds, sel_e);
			checked[sel_e] = true;
			if (!done_cons.empty()) { // remove satisfied constraints
				for (uint c : done_cons) constr.erase(constr.begin() + c);
			}
			init_f = curr_f;
		}
		else {
			r *= (1.0 - Constants::GAMMA);
			sel_e = -1;
		}
			

		//if (!checked[e]) {
		//	vector<uint> stmp(seeds);
		//	insert_sorted(stmp, e);
		//	vector<uint> sat_cons;
		//	double sum_f_old = 0.0, sum_f_new = 0.0;
		//	sum_f(stmp, constr, sat_cons, sum_f_old, sum_f_new);
		//	if ((sum_f_old - init_f) >= r) { // if e can be added
		//		insert_sorted(seeds, e);
		//		checked[e] = true;
		//		if (!sat_cons.empty()) { // remove satisfied constraints
		//			for (uint c : sat_cons) constr.erase(constr.begin() + c);
		//		}
		//		init_f = sum_f_new;
		//	}
		//}
		//

		//e = (e + 1) % n;
		//if (e == 0) {
		//	r *= (1.0 - Constants::GAMMA);
		//} 
	}
}
