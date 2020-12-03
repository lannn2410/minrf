#include "AlgR.h"
#include "Greedy0.h"
#include "ThrGreedy0.h"
#include "Degree.h"
#include "Random0.h"
#include "Separate.h"
#include "RandomGreedy0.h"
#include "AccumulateThr0.h"
#include <algorithm>

AlgR::AlgR(DataSet* d, vector<vector<double>> thresholds, ALG_0 type0, uint r) : Algorithm(d, thresholds), r(r)
{
	alg_0_type = ALG_0::DEGREE; // sum_f need to be parallel
	switch (type0)
	{
	case ALG_0::GREEDY:
		alg0 = new Greedy0(d, thresholds);
		break;
	case ALG_0::THRESHOLD:
		alg0 = new ThrGreedy0(d, thresholds);
		break;
	case ALG_0::DEGREE:
		alg0 = new Degree(d, thresholds);
		break;
	case ALG_0::RANDOM:
		alg0 = new Random0(d, thresholds);
		break;
	case ALG_0::SEPARATE:
		alg0 = new Separate(d, thresholds);
		break;
	case ALG_0::RAND_GREEDY:
		alg0 = new RandomGreedy0(d, thresholds);
		break;
	case ALG_0::ACC_THR:
		alg0 = new AccumulateThr0(d, thresholds);
		break;
	default:
		alg0 = new Greedy0(d, thresholds);
		break;
	}
}

AlgR::~AlgR()
{
	delete alg0;
}

void AlgR::get_solutions(vector<uint>& seeds)
{
	vector<uint> old_seeds(seeds);
	alg0->get_solutions(seeds);
	for (int t = 0; t < r; ++t) {
		if (seeds.size() > old_seeds.size()) {
			// enumerate and add constraints
			vector<Constraint> cons;

			// unordered_seeds has order that <new_elements> then <old_seeds>
			vector<uint> unordered_seeds = common_instance->set_minus(seeds, old_seeds);
			unordered_seeds.insert(unordered_seeds.end(), old_seeds.begin(), old_seeds.end());

			uint hmm = 0; // number of elements to consider as the 1st element in combination of set of size r of seeds
			if (seeds.size() <= r) hmm = 1;
			else if (old_seeds.size() < r) hmm = seeds.size() - r + 1;
			else hmm = seeds.size() - old_seeds.size();

			// the first element index should be of newly added elements
			#pragma omp parallel for
			for (int i = 0; i < hmm; ++i) {
				vector<uint> enum_idx(r, -1); // store the index of all set of size r
				enum_idx[0] = i;
				enum_add_constr(seeds, unordered_seeds, i, 0, enum_idx, cons);
			}

			old_seeds = seeds;

			if (cons.size() > 0) {
				// cut and solve constraint by half
				uint ntmp = cons.size() > 2 ? cons.size() / 2 : cons.size();
				vector<Constraint> cons_tmp(cons.begin(), cons.begin() + ntmp);
				alg0->setConstraints(cons_tmp);
				alg0->get_solutions(seeds);
				if (ntmp < cons.size()) {
					cons_tmp = vector<Constraint>(cons.begin() + ntmp, cons.end());
					alg0->setConstraints(cons_tmp);
					alg0->get_solutions(seeds);
				}
				/*alg0->setConstraints(cons);
				alg0->get_solutions(seeds);*/
			}
			else break;
		}
		else break;
	}
}

uint AlgR::get_no_queries()
{
	return no_queries + alg0->get_no_queries();
}

void AlgR::enum_add_constr(const vector<uint>& seeds, const vector<uint>& unordered_seeds, uint checked_seed_idx, uint checked_enum_idx, vector<uint>& enum_idx, vector<Constraint>& cons)
{
	uint remain = seeds.size() - checked_seed_idx - 1;
	uint need = r - checked_enum_idx - 1;

	if ((remain >= need && need > 0) || (seeds.size() < r && remain > 0)) {
		uint upper = seeds.size() > r ? seeds.size() - need + 1 : checked_enum_idx + 2;
		for (int i = checked_seed_idx + 1; i < upper; ++i) {
			enum_idx[checked_enum_idx + 1] = i;
			enum_add_constr(seeds, unordered_seeds, i, checked_enum_idx + 1, enum_idx, cons);
		}
	}
	else {
		if (checked_enum_idx == r - 1 || seeds.size() < r) {
			vector<uint> exclude_set;
			for (int i = 0; i <= checked_enum_idx; ++i) exclude_set.push_back(unordered_seeds[enum_idx[i]]);
			sort(exclude_set.begin(), exclude_set.end());


			vector<Constraint> new_cons;
			// create new constraints
			for (int i = 0; i < thresholds.size(); ++i) {
				for (int ii = 0; ii < thresholds[i].size(); ++ii) {
					Constraint con(exclude_set, i, ii, thresholds[i][ii]);
					new_cons.push_back(con);
				}
			}

			vector<uint> done_cons;
			double dummy1 = 0.0, dummy2 = 0.0;
			sum_f(seeds, new_cons, done_cons, dummy1, dummy2);
			if (done_cons.size() < new_cons.size()) { // add new violated constraints to overall 
				for (uint c : done_cons) new_cons.erase(new_cons.begin() + c);
				#pragma omp critical
				{
					cons.insert(cons.end(), new_cons.begin(), new_cons.end());
				}
			}
		}
		
	}
}

