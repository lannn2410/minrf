#pragma once
#include "Algorithm.h"
class Degree :
	public Algorithm
{
public:
	Degree(DataSet* d, vector<vector<double>> thresholds);
	void get_solutions(vector<uint>& seeds);

private:
	vector<uint> degree, sorted_idx; // ids is sorted in decreasing order of their out-degree
};

