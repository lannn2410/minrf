#pragma once
#include "Algorithm.h"
#include "Constants.h"
class Alg1 :
	public Algorithm
{
public:
	Alg1(DataSet* d, vector<vector<double>> thresholds, ALG_0 type0);
	~Alg1();
	void get_solutions(vector<uint>& seeds);
	uint get_no_queries();
private:
	Algorithm * alg0;
};

