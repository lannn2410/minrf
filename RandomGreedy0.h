#pragma once
#include "Algorithm.h"
class RandomGreedy0 :
	public Algorithm
{
public:
	RandomGreedy0(DataSet* d, vector<vector<double>> thresholds);
	void get_solutions(vector<uint> & seeds);
	~RandomGreedy0();
};

