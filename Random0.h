#pragma once
#include "Algorithm.h"
class Random0 :
	public Algorithm
{
public:
	Random0(DataSet* d, vector<vector<double>> thresholds);
	void get_solutions(vector<uint>& seeds);
private:

	vector<uint> random_order;
};

