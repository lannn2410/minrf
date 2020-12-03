#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
//#include <time.h>
#include <chrono>
#include <iomanip> // setprecision
#include <string>
#if defined(_WIN32)
#include<direct.h>
#else 
#include <sys/stat.h>
#include <sys/types.h>
#endif


#include "Constants.h"
#include "Common.h"
#include "DataSet.h"
#include "Alg1.h"
#include "AlgR.h"
#include "Greedy0.h"
#include "ThrGreedy0.h"
#include "Degree.h"
#include "Random0.h"
#include "Separate.h"
#include "Disjoint.h"
#include "RandomGreedy0.h"
#include "AccumulateThr0.h"

using namespace std;

//vector<double> T = { 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9 };
//vector<double> T = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };
vector<double> T = { 0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.007, 0.008, 0.009 };

vector<uint> R = { 2, 3, 4, 5, 6, 7, 8, 9 };
//vector<uint> R = { 5, 6, 7, 8, 9 };

// alg_0 for social
//vector<ALG_0> alg_0_type = { ALG_0::RAND_GREEDY };
vector<ALG_0> alg_0_type = { ALG_0::RAND_GREEDY, ALG_0::GREEDY, ALG_0::THRESHOLD, ALG_0::RANDOM, ALG_0::SEPARATE };
//vector<ALG_0> alg_0_type = { ALG_0::ACC_THR};
// alg_0 for movies
//vector<ALG_0> alg_0_type = {ALG_0::GREEDY, ALG_0::THRESHOLD, ALG_0::SEPARATE};

void run_and_save_alg(Algorithm * alg, ofstream & sol_f, ofstream & query_f, ofstream & time_f) {
	vector<uint> seeds;

	auto begin = chrono::high_resolution_clock::now();
	alg->get_solutions(seeds);
	auto end = chrono::high_resolution_clock::now();
	auto dur = end - begin;
	auto runtime = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

	uint no_queries = alg->get_no_queries();

	cout << seeds.size() << "\t" << no_queries << endl;

	sol_f << seeds.size() << ",";
	query_f << no_queries << ",";
	time_f << runtime << ",";
}

void run_algs(DataSet & data, const uint & r, const vector<vector<double>> & threshold, ofstream & sol_f, ofstream & query_f, ofstream & time_f, CHANGE change) {
	for (auto a : alg_0_type) {
		if (!(change == CHANGE::R && (a == ALG_0::RANDOM))) {
			Algorithm * alg;
			if (r == 0 && change == CHANGE::T) {
				switch (a)
				{
				case ALG_0::GREEDY:
					alg = new Greedy0(&data, threshold);
					break;
				case ALG_0::THRESHOLD:
					alg = new ThrGreedy0(&data, threshold);
					break;
				case ALG_0::DEGREE:
					alg = new Degree(&data, threshold);
					break;
				case ALG_0::RANDOM:
					alg = new Random0(&data, threshold);
					break;
				case ALG_0::SEPARATE:
					alg = new Separate(&data, threshold);
					break;
				case ALG_0::RAND_GREEDY:
					alg = new RandomGreedy0(&data, threshold);
					break;
				case ALG_0::ACC_THR:
					alg = new AccumulateThr0(&data, threshold);
					break;
				default:
					alg = new Greedy0(&data, threshold);
					break;
				}
			}
			else if (r == 1 && change == CHANGE::T)
				alg = new Alg1(&data, threshold, a);
			else
				alg = new AlgR(&data, threshold, a, r);
			run_and_save_alg(alg, sol_f, query_f, time_f);
			delete alg;
		}
		
	}

	if (r == 1 && change == CHANGE::T) { // run with alg_r
		for (auto a : alg_0_type) {
			//if (a == ALG_0::GREEDY || a == ALG_0::THRESHOLD) {
			if (a == ALG_0::RAND_GREEDY || a == ALG_0::GREEDY || a == ALG_0::THRESHOLD) {
				Algorithm* alg = new AlgR(&data, threshold, a, 1);
				run_and_save_alg(alg, sol_f, query_f, time_f);
				delete alg;
			}
		}
	}

	// run disjoint
	if (r > 0) {
		Algorithm* alg = new Disjoint(&data, threshold, r);
		run_and_save_alg(alg, sol_f, query_f, time_f);
		delete alg;
	}
}

void run_experiment(uint n, string file_name, DATA_TYPE d_type, CHANGE change = CHANGE::T, double t = 0.5, uint r = 1, uint no_users = 4, string rate_file = "", uint no_dimensions = 1000) {
	long folder_prefix = time(NULL);
	string re_folder = "result/" + to_string(folder_prefix) + "_" + file_name + "_";
	re_folder.append((change == CHANGE::T) ? "T" : "R");

	Constants::D_TYPE = d_type;

	DataSet data;
	if (d_type == DATA_TYPE::Social) {
		vector<uint> disjoint_group = { 3,2 }; // {black, white, latino}, {male, female}
		//data.read_network_from_file(n, "data/" + file_name, disjoint_group);
		data.read_sampled_graph(n, disjoint_group); // for fast and consistent result
	}
	else {
		data.read_movie_dataset(n, no_dimensions, no_users, "data/" + file_name, "data/" + rate_file);
	}

#if defined(_WIN32)
	_mkdir(re_folder.c_str());
#else 
	mkdir(re_folder.c_str(), 0777); // notice that 777 is different than 0777
#endif

	string sol_output_file = re_folder + "/result.csv";
	string query_output_file = re_folder + "/query.csv";
	string time_output_file = re_folder + "/runtime.csv";
	ofstream sol_writefile(sol_output_file), query_writefile(query_output_file), time_writefile(time_output_file);

	if (sol_writefile.is_open() && query_writefile.is_open() && time_writefile.is_open()) {
		// create header
		string header = "t,r,";
		if (r == 0 && change == CHANGE::T) {
			header.append("rand_gr,gr,thr,random,separate");
		} else if (r == 1 && change == CHANGE::T) {
			header.append("alg1_rand_gr,alg1_gr,alg1_thr,random,separate,algR_rand_gr,algR_gr,algR_thr,disjoint");
		}
		else { // change R
			header.append("rand_gr,greedy,separate,disjoint");
		}
		sol_writefile << header << endl;
		query_writefile << header << endl;
		time_writefile << header << endl;

		vector<vector<double>> threshold;
		if (change == CHANGE::R) threshold = data.generateThreshold(t);

		// content
		if (change == CHANGE::T) {
			for (double t : T) {
				cout << "--------- t = " << t << " -----------" << endl;
				threshold = data.generateThreshold(t);
				sol_writefile << t << "," << r  << ",";
				query_writefile << t << "," << r << ",";
				time_writefile << t << "," << r << ",";
				run_algs(data, r, threshold, sol_writefile, query_writefile, time_writefile, change);
				sol_writefile << endl;
				query_writefile << endl;
				time_writefile << endl;
			}
		}
		else {
			for (uint r : R) {
				cout << "--------- r = " << r << " -----------" << endl;
				sol_writefile << t << "," << r << ",";
				query_writefile << t << "," << r << ",";
				time_writefile << t << "," << r << ",";
				run_algs(data, r, threshold, sol_writefile, query_writefile, time_writefile, change);
				sol_writefile << endl;
				query_writefile << endl;
				time_writefile << endl;
			}
		}
		
		sol_writefile.close();
		query_writefile.close();
		time_writefile.close();
	}
}


void print_help() {
	cout << "Options: " << endl;
	cout << "-h <print help>" << endl
		<< "-t <data type, 0: social, 1: movie>" << endl
		<< "-a <alpha value> # default: 0.1" << endl
		<< "-T <T value> # default: 0.1" << endl
		<< "-r <r value> # default: 1" << endl
		<< "-a0 <algorithm for r=0 (run alone if r=0, in combination with algr if r > 0), 0: randgr, 1: greedy, 2: thresholdgr, 3: sep>  # default: 0" << endl
		<< "-a1 <algorithm for r=1 (only valid if r=1), 0: alg1, 1: algr, 2: disjoint>  # default: 0" << endl
		<< "-ar <algorithm for r>1, 0: algr, 1: disjoint>  # default: 0" << endl
		<< "-p <number of threads to running algorithms> # default: 4" << endl;
}

struct Args
{
	DATA_TYPE type = DATA_TYPE::Social;
	int r = 1, a0 = 0, a1 = 0, ar = 0, p = 4;
	double alpha = 0.1, T = 0.1;
	bool is_help = false;
};

Args parseArgs(int argc, char** argv) {
	Args re;
	int i = 1;
	while (i <= argc - 1) {
		string arg = argv[i];
		if (arg == "-h") {
			re.is_help = true;
			break;
		}
		if (i + 1 >= argc) break;
		string s_val = argv[i + 1];
		if (s_val == "-h") { 
			re.is_help = true; 
			break;
		}
		std::string::size_type sz;
		if (arg == "-t" || arg == "-r" || arg == "-a0" || arg == "-a1" || arg == "-ar" || arg == "-p") {
			int val = std::stoi(s_val, &sz);
			if (arg == "-t") {
				re.type = val == 0 ? DATA_TYPE::Social : DATA_TYPE::Movie;
			}
			else if (arg == "-r") {
				re.r = val;
			}
			else if (arg == "-a0") {
				re.a0 = val;
			}
			else if (arg == "-a1") {
				re.a1 = val;
			}
			else if (arg == "-ar") {
				re.ar = val;
			}
			else if (arg == "-p") {
				re.p = val;
			}

		}
		else {
			double val = std::stod(s_val, &sz);

			if (arg == "-a") {
				re.alpha = val;
			}
			else if (arg == "-T") {
				re.T = val;
			}
		}
		
		i += 2;
	}

	return re;
}

ALG_0 getAlg0Type(int a) {
	switch (a)
	{
	case 0:
		return ALG_0::RAND_GREEDY;
		break;
	case 1:
		return ALG_0::GREEDY;
		break;
	case 2:
		return ALG_0::THRESHOLD;
		break;
	case 3:
		return ALG_0::SEPARATE;
		break;
	default:
		return ALG_0::RAND_GREEDY;
		break;
	}
}

void run_command(const Args &r) {
	if (r.is_help) {
		print_help();
		return;
	}
	Constants::D_TYPE = r.type;
	DataSet data;
	if (r.type == DATA_TYPE::Social) {
		vector<uint> disjoint_group = { 3,2 }; // {black, white, latino}, {male, female}
		data.read_network_from_file(4039, "data/facebook_combined.txt", disjoint_group); // hard code
	}
	else {
		data.read_movie_dataset(10381, 1129, 4, "data/movies.csv", "data/ratings.csv"); // hard code
	}

	vector<vector<double>> threshold = data.generateThreshold(r.T);
	double sol = 0.0;
	omp_set_num_threads(r.p);
	Algorithm * alg;
	if (r.r == 0) {
		switch (getAlg0Type(r.a0))
		{
		case ALG_0::GREEDY:
			alg = new Greedy0(&data, threshold);
			break;
		case ALG_0::THRESHOLD:
			alg = new ThrGreedy0(&data, threshold);
			break;
		case ALG_0::SEPARATE:
			alg = new Separate(&data, threshold);
			break;
		case ALG_0::RAND_GREEDY:
			alg = new RandomGreedy0(&data, threshold);
			break;
		default:
			alg = new RandomGreedy0(&data, threshold);
			break;
		}
	}
	else if (r.r == 1) {
		switch (r.a1) {
		case 0: 
			alg = new Alg1(&data, threshold, getAlg0Type(r.a0));
			break;
		case 1:
			alg = new AlgR(&data, threshold, getAlg0Type(r.a0), r.r);
			break;
		case 2:
			alg = new Disjoint(&data, threshold, r.r);
			break;
		default:
			alg = new Alg1(&data, threshold, getAlg0Type(r.a0));
			break;
		}
	}
	else {
		switch (r.ar) {
		case 0:
			alg = new AlgR(&data, threshold, getAlg0Type(r.a0), r.r);
			break;
		case 1:
			alg = new Disjoint(&data, threshold, r.r);
			break;
		default:
			alg = new AlgR(&data, threshold, getAlg0Type(r.a0), r.r);
			break;
		}
	}

	vector<uint> seeds;
	alg->get_solutions(seeds);
	int no_queries = alg->get_no_queries();
	delete alg;

	cout << "Seed size: " << seeds.size() << endl
		<< "Number of queries: " << no_queries << endl;
}



int main(int argc, char* argv[]) {
	srand(time(NULL));
	//omp_set_num_threads(Constants::NUM_THREADS);
	//DataSet d;
	//d.generate_samples_from_file(4039, "data/facebook_combined.txt", { 3,2 }, 1000);
	//run_experiment(4039, "facebook_combined.txt", DATA_TYPE::Social, CHANGE::T, 0, 0);
	//run_experiment(4039, "facebook_combined.txt", DATA_TYPE::Social, CHANGE::T, 0, 1);
	//run_experiment(4039, "facebook_combined.txt", DATA_TYPE::Social, CHANGE::R, 0.6);
	//run_experiment(4039, "facebook_combined.txt", DATA_TYPE::Social, CHANGE::R, 0.05);
	//run_experiment(103, "as19990829.txt", DATA_TYPE::Social, R_TYPE::R_1);
	//run_experiment(10381, "movies.csv", DATA_TYPE::Movie, CHANGE::T, 0.002, 0, 4, "ratings.csv", 1129);
	//run_experiment(10381, "movies.csv", DATA_TYPE::Movie, CHANGE::T, 0.002, 1, 4, "ratings.csv", 1129);
	//run_experiment(10381, "movies.csv", DATA_TYPE::Movie, CHANGE::R, 0.002, 1, 4, "ratings.csv", 1129);
	//run_experiment(10381, "movies.csv", DATA_TYPE::Movie, CHANGE::T, 0.002, 2, 4, "ratings.csv", 1129);
	//run_experiment(100, "random.txt", DATA_TYPE::Social, CHANGE::R);

	run_command(parseArgs(argc, argv));
	return 1;
}