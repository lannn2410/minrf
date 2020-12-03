#pragma once
#include "Algorithm.h"
class Disjoint :
	public Algorithm
{
public:
	Disjoint(DataSet* d, vector<vector<double>> thresholds, uint r);
	~Disjoint();
	void get_solutions(vector<uint>& seeds);;
	uint get_no_queries();
private:
	uint r;
	Algorithm* alg;
};

