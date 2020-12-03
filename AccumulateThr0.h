#pragma once
#include "Algorithm.h"
class AccumulateThr0 :
	public Algorithm
{
public:
public:
	AccumulateThr0(DataSet* d, vector<vector<double>> thresholds);
	~AccumulateThr0();
	void get_solutions(vector<uint>& seeds);
};

