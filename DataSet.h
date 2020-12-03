#pragma once
#include <string>
#include <vector>
#include <map>
#include "Common.h"

using namespace std;

class DataSet
{
public:
	// read social network
	bool read_network_from_file(int no_nodes, string file, vector<uint> disjoint_groups); 
	void generate_samples_from_file(int no_nodes, string file, vector<uint> disjoint_groups, uint no_samples); // generate sampled graphs
	void generate_graph(int no_nodes, double p);
	void read_sampled_graph(int no_nodes, vector<uint> disjoint_groups);
	// disjoint group: assume constraint include <black, white, latino> and <men, women> => disjoint_group = {3,2}
	// if threshold for latino is 80 then thresholds[0][2] = 80
	uint sample_network_ic(const vector<uint> & seeds, const uint & group_kind, const uint & sub_group_kind);
	uint sample_network_lt(const vector<uint> & seeds, const uint & group_kind, const uint & sub_group_kind);
	uint sample_network_neighbor(const vector<uint> & seeds, const uint & group_kind, const uint & sub_group_kind);
	double get_from_sampled_graphs(const vector<uint> & seeds, const uint & group_kind, const uint & sub_group_kind);
	uint get_n();
	vector<uint> getDegrees();


	// read movie dataset
	bool read_movie_dataset(int no_movies, int no_dimensions, int no_users, string movie_file, string user_file);
	double get_cosine_similarity(const vector<double> & A, const vector<double> & B);
	double get_f_user(const vector<uint> & seeds, const uint & user_id);
	
	vector<vector<double>> generateThreshold(double p);

private:
	void clear(); // clear dataset
	uint n; // size of V

	// social network dataset
	void generate_sample(uint no_samples);
	bool is_directed;
	map<uint, uint> mapNodeId; // actual id -> order id
	vector<vector<uint>> neighbors;
	vector<vector<uint>> groups; // id -> groups belong to, e.g <black, women> = {0,1}
	vector<vector<uint>> count_sub_group; // count number of nodes belong to a sub_group
	vector<Adj> sample_graphs;

	// movie dataset
	uint no_users;
	vector<vector<double>> similarity;
	vector<vector<double>> movie2user; // measure similar (preference) score of user to a movie: user -> movie id -> score
	vector<vector<uint>> user_favorate_movies;
	vector<double> max_threshold; // maximum possible threshold each user can reach 
};

