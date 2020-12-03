#include "Algorithm.h"
#include "Constants.h"

#include <algorithm>    // std::set_difference, std::sort
#include <numeric>

Algorithm::Algorithm(DataSet* d, vector<vector<double>> thresholds) : data(d), thresholds(thresholds), no_queries(0)
{
	// initiate original constraints set
	for (int i = 0; i < thresholds.size(); ++i) {
		for (int ii = 0; ii < thresholds[i].size(); ++ii) {
			Constraint con(vector<uint>(), i, ii, thresholds[i][ii]);
			constr.push_back(con);
		}
	}

	common_instance = Common::getInstance();
}

void Algorithm::sum_f(const vector<uint> seeds, const vector<Constraint>& cstrts, vector<uint> & done_ctr, double& sum_f_old, double& sum_f_new)
{
	// estimate f for each constraint
	for (int c = cstrts.size() - 1; c >= 0; --c) {
		Constraint con = cstrts[c];
		double f = 0.0;


		vector<uint> s_tmp = con.exclude_set.size() > 0 ? common_instance->set_minus(seeds, con.exclude_set) : seeds;
		
		if (Constants::D_TYPE == DATA_TYPE::Social) {
			if (Constants::IM_MODEL != INF_MODEL::NEIGHBOR) {
				//for (int i = 0; i < Constants::NO_SAMPLES; ++i) {
				//	uint tmp = Constants::IM_MODEL == INF_MODEL::LT ? data->sample_network_lt(s_tmp, con.group_kind, con.sub_group_kind)
				//		: data->sample_network_ic(s_tmp, con.group_kind, con.sub_group_kind);
				//	f += tmp;
				//}
				//f /= Constants::NO_SAMPLES; // average over sample
				f = data->get_from_sampled_graphs(s_tmp, con.group_kind, con.sub_group_kind);
			}
			else
				f = data->sample_network_neighbor(s_tmp, con.group_kind, con.sub_group_kind);
		}
		else {
			f = data->get_f_user(s_tmp, con.sub_group_kind);
		}
		
		
		f /= con.T; // normalize
		f = f > 1.0 ? 1.0 : f; // get min

		sum_f_old += f;
		if (f >= (1.0 - Constants::ALPHA))
			done_ctr.push_back(c);
		else
			sum_f_new += f;
		
		#pragma omp critical
		{
			++no_queries;
		}
	}
}

void Algorithm::setConstraints(const vector<Constraint>& cons)
{
	constr = cons;
}

uint Algorithm::get_no_queries()
{
	return no_queries;
}

