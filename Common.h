#pragma once

#include <omp.h>
#include <vector>

using namespace std;

typedef unsigned int uint;

struct Constraint
{
	vector<uint> exclude_set; // ids are sorted in increasing order
	uint group_kind, sub_group_kind;
	double T;
	Constraint(vector<uint> exs, uint gk, uint sgk, double t) : exclude_set(exs), group_kind(gk), sub_group_kind(sgk), T(t) {};
};

typedef vector<vector<uint>> Adj;

class Common
{
public:
	Common();
	~Common();

	static Common* getInstance();
	unsigned randomInThread(int thread_id);
	vector<uint> set_minus(const vector<uint>& A, const vector<uint>& B);
	void insert_sorted(vector<uint> & A, const uint& a);
	vector<uint> set_merge(const vector<uint> & A, const vector<uint> &B);

private:
	static Common* instance;
	int* seed;
};

