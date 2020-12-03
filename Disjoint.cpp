#include "Disjoint.h"
#include "Greedy0.h"


Disjoint::Disjoint(DataSet * d, vector<vector<double>> thresholds, uint r) : Algorithm(d, thresholds), r(r)
{
	alg_0_type = ALG_0::DISJOINT;
	alg = new Greedy0(d, thresholds);
}

Disjoint::~Disjoint()
{
	delete alg;
}

void Disjoint::get_solutions(vector<uint>& seeds)
{
	alg->get_solutions(seeds);
	for (int j = 0; j < r; ++j) {
		vector<Constraint> new_cons;
		// create new constraints
		for (int i = 0; i < thresholds.size(); ++i) {
			for (int ii = 0; ii < thresholds[i].size(); ++ii) {
				Constraint con(seeds, i, ii, thresholds[i][ii]);
				new_cons.push_back(con);
			}
		}
		alg->setConstraints(new_cons);
		alg->get_solutions(seeds);
	}
}

uint Disjoint::get_no_queries()
{
	return no_queries + alg->get_no_queries();
}
