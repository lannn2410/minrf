#include "Separate.h"
#include "Greedy0.h"

Separate::Separate(DataSet* d, vector<vector<double>> thresholds) : Algorithm(d, thresholds)
{
	alg_0_type = ALG_0::SEPARATE;
	alg = new Greedy0(d, thresholds);
}

Separate::~Separate()
{
	delete alg;
}

void Separate::get_solutions(vector<uint>& seeds)
{
	vector<uint> init_seeds = seeds;
	for (Constraint c : constr) {
		vector<Constraint> single_con = { c };
		alg->setConstraints(single_con);
		vector<uint> stmp(init_seeds);
		alg->get_solutions(stmp);
		seeds = common_instance->set_merge(seeds, stmp);
	}
}

uint Separate::get_no_queries()
{
	return no_queries + alg->get_no_queries();
}
