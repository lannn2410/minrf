#pragma once
#include "Algorithm.h"
class Greedy0 : public Algorithm
{
public:
	Greedy0(DataSet* d, vector<vector<double>> thresholds);
	void get_solutions(vector<uint> & seeds);
};

