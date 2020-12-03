#pragma once
#include "Algorithm.h"
#include "Constants.h"
class AlgR :
	public Algorithm
{
public:
	AlgR(DataSet* d, vector<vector<double>> thresholds, ALG_0 type0, uint r);
	~AlgR();
	void get_solutions(vector<uint>& seeds);
	uint get_no_queries();
private:
	void enum_add_constr(const vector<uint>& seeds, const vector<uint>& unordered_seeds, uint checked_seed_idx, uint checked_enum_idx, vector<uint>& enum_idx, vector<Constraint>& cons);

	Algorithm* alg0;
	uint r;
};

