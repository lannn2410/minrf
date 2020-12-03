#include "Degree.h"
#include <algorithm>
#include <iostream>

Degree::Degree(DataSet* d, vector<vector<double>> thresholds) : Algorithm(d, thresholds)
{
	alg_0_type = ALG_0::DEGREE;
	uint n = data->get_n();
	degree = data->getDegrees();
	for (int i = 0; i < n; ++i) sorted_idx.push_back(i);
	sort(sorted_idx.begin(), sorted_idx.end(), [&](int i, int j) {return degree[i] > degree[j]; });
}

void Degree::get_solutions(vector<uint>& seeds)
{
	uint n = data->get_n();
	
	uint min_e = n;
	#pragma omp parallel for
	for (int e = 1; e <= n; ++e) {
		if (e < min_e) {
			vector<uint> sub_v(sorted_idx.begin(), sorted_idx.begin() + e);
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
	vector<uint> sub_v(sorted_idx.begin(), sorted_idx.begin() + min_e);
	sort(sub_v.begin(), sub_v.end());
	seeds = common_instance->set_merge(sub_v, seeds);

	//uint curr_max = 0;
	//while (!constr.empty()) {
	//	while (curr_max < n && checked[sorted_idx[curr_max]]) ++curr_max;
	//	if (curr_max < n) {
	//		insert_sorted(seeds, curr_max);
	//		checked[curr_max] = true;
	//		double dummy1 = 0.0, dummy2 = 0.0;
	//		vector<uint> sat_cons;
	//		sum_f(seeds, constr, sat_cons, dummy1, dummy2);
	//		if (!sat_cons.empty()) { // remove satisfied constraints
	//			for (uint c : sat_cons) constr.erase(constr.begin() + c);
	//		}
	//	}
	//	else {
	//		cout << "degree fail!" << endl;
	//		break;
	//	}
	//}


}
