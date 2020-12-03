#include "Common.h"
#include <stdlib.h>
#include <algorithm>    // std::set_difference, std::sort
#include <numeric>

Common* Common::instance = nullptr;

Common::Common()
{
	seed = new int[10000];
	for (int i = 0; i < 10000; i++) {
		seed[i] = rand();
	}
}

Common::~Common()
{
}

Common* Common::getInstance()
{
	if (!instance)
		instance = new Common();
	return instance;
}

unsigned Common::randomInThread(int thread_id)
{
	unsigned tmp = seed[thread_id % 10000];
	tmp = tmp * 17931 + 7391;
	seed[thread_id % 10000] = tmp;
	return tmp;
}

vector<uint> Common::set_minus(const vector<uint>& A, const vector<uint>& B)
{
	vector<uint> tmp(A.size());
	vector<uint>::iterator it = set_difference(A.begin(), A.end(), B.begin(), B.end(), tmp.begin());
	tmp.resize(it - tmp.begin());
	return tmp;
}

void Common::insert_sorted(vector<uint> & A, const uint& a)
{
	A.insert(std::upper_bound(A.begin(), A.end(), a), a);
}

vector<uint> Common::set_merge(const vector<uint>& A, const vector<uint>& B)
{
	vector<uint> tmp(A.size() + B.size());
	vector<uint>::iterator it = set_union(A.begin(), A.end(), B.begin(), B.end(), tmp.begin());
	tmp.resize(it - tmp.begin());
	return tmp;
}