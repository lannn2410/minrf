#pragma once
#include "Algorithm.h"
class ThrGreedy0 :
	public Algorithm
{
public:
	ThrGreedy0(DataSet* d, vector<vector<double>> thresholds);
	void get_solutions(vector<uint>& seeds);
};

