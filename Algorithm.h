#pragma once
#include "DataSet.h"
#include "Constants.h"

using namespace std;

class Algorithm
{
public:
	Algorithm(DataSet* d, vector<vector<double>> thresholds);
	virtual void get_solutions(vector<uint> & seeds) = 0;
	void sum_f(const vector<uint> seeds, const vector<Constraint> & ctrts, vector<uint>& done_ctr, double & sum_f_old, double & sum_f_new);  
	// done_ctr : save constraint ids that >= 1-alpha
	// sum_f_old : sum all f_i in constraints
	// sum_f_new : sum only f_i that < 1-alpha

	void setConstraints(const vector<Constraint>& cons);
	virtual uint get_no_queries();

protected:
	ALG_0 alg_0_type;
	DataSet* data;
	Common * common_instance;
	vector<vector<double>> thresholds;
	vector<Constraint> constr;
	uint no_queries;
};

