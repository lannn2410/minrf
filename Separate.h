#pragma once
#include "Algorithm.h"


// find minimum seeds to satisfy each constraint, then unite all

class Separate :
	public Algorithm
{
public:
	Separate(DataSet* d, vector<vector<double>> thresholds);
	~Separate();
	void get_solutions(vector<uint>& seeds);
	uint get_no_queries();

private:
	Algorithm* alg;

};

