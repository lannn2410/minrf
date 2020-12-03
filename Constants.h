#pragma once
#include <string>

using namespace std;

enum class INF_MODEL { IC, LT, NEIGHBOR };
enum class ALG_0 {GREEDY, THRESHOLD, DEGREE, RANDOM, SEPARATE, DISJOINT, RAND_GREEDY, ACC_THR};
enum class DATA_TYPE {Social, Movie};
enum class CHANGE {T, R};

class Constants
{
public:
	static const int NUM_THREADS = 70;
	static const int NO_SAMPLES = 100;
	static double ALPHA, GAMMA;
	static INF_MODEL IM_MODEL;
	static DATA_TYPE D_TYPE;
	static string SAMPLE_FOLDER;
};

