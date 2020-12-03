#include "Alg1.h"
#include "Greedy0.h"
#include "ThrGreedy0.h"
#include "Degree.h"
#include "Random0.h"
#include "Separate.h"
#include "RandomGreedy0.h"
#include "AccumulateThr0.h"
#include <algorithm>
#include <iostream>

Alg1::Alg1(DataSet* d, vector<vector<double>> thresholds, ALG_0 type0) : Algorithm(d, thresholds)
{
	alg_0_type = ALG_0::DEGREE; // so sum_f can be parallel
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

Alg1::~Alg1()
{
	delete alg0;
}

void Alg1::get_solutions(vector<uint>& seeds)
{
	alg0->get_solutions(seeds);
	vector<uint> seeds_copy(seeds);

	for (uint e : seeds_copy) {
		//cout << "Robust check " << e << endl;
		// inititate new set of constraints
		vector<Constraint> new_cons;
		for (int i = 0; i < thresholds.size(); ++i) {
			for (int ii = 0; ii < thresholds[i].size(); ++ii) {
				Constraint con({e}, i, ii, thresholds[i][ii]);
				new_cons.push_back(con);
			}
		}

		// check for satisfaction of removal of e
		vector<uint> done_cons;
		double sum_f_old = 0.0, sum_f_new = 0.0;
		sum_f(seeds, new_cons, done_cons, sum_f_old, sum_f_new);
		if (done_cons.size() < new_cons.size()) { // remove e causes some constraints violated
			// filter out unviolated constraint
			for (uint c : done_cons) new_cons.erase(new_cons.begin() + c);

			// rerun alg0 to add new seed
			alg0->setConstraints(new_cons);
			alg0->get_solutions(seeds);
		}
	}
}

uint Alg1::get_no_queries()
{
	return no_queries + alg0->get_no_queries();
}
