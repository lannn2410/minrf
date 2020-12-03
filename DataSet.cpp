#include "DataSet.h"
#include <fstream>
#include <queue>
#include <sstream>
#include <math.h>
#include <algorithm>
#include "Constants.h"

bool DataSet::read_network_from_file(int no_nodes, string file, vector<uint> disjoint_groups)
{
	clear();

	n = no_nodes;
	neighbors = vector<vector<uint>>(no_nodes);
	groups = vector<vector<uint>>(no_nodes, vector<uint>(disjoint_groups.size(), 0));
	count_sub_group = vector<vector<uint>>(disjoint_groups.size());
	for (int i = 0; i < disjoint_groups.size(); ++i)
		count_sub_group[i] = vector<uint>(disjoint_groups[i], 0);

	ifstream is(file);
	is.seekg(0, is.end);
	long bufSize = is.tellg();
	is.seekg(0, is.beg);
	int item = 0;

	char* buffer = new char[bufSize];

	is.read(buffer, bufSize);
	is.close();

	std::string::size_type sz = 0;
	long sp = 0;
	uint start_id, end_id;
	bool is_start = true;
	int zero_id = -1;
	uint id = 0;
	uint s_id, e_id; // used to stored ordered id of startId and endId
	uint edge_id = 0;

	while (sp < bufSize) {
		char c = buffer[sp];
		item = item * 10 + c - 48;
		sp++;
		if (sp == bufSize || (sp < bufSize && (buffer[sp] < 48 || buffer[sp] > 57))) {
			while (sp < bufSize && (buffer[sp] < 48 || buffer[sp] > 57))
				sp++;

			if (is_start) {
				start_id = item;
				// if (start_id >= no_nodes) return false;
				is_start = false;
			}
			else {
				end_id = item;
				// if (end_id >= no_nodes) return false;
				is_start = true;

				if (start_id < end_id) // as dataset has duplication a->b, b->a
				{
					s_id = mapNodeId[start_id];
					if (!s_id && start_id != zero_id) { // if start node has not exist
						mapNodeId[start_id] = id;
						s_id = id;

						// assign group
						for (int i = 0; i < disjoint_groups.size(); ++i) {
							uint tmp = rand() % disjoint_groups[i];
							groups[s_id][i] = tmp;
							++count_sub_group[i][tmp];
						}

						if (id == 0)
							zero_id = start_id;
						id++;
					}

					e_id = mapNodeId[end_id];
					if (!e_id && end_id != zero_id) { // if end node has not exist 
						mapNodeId[end_id] = id;
						e_id = id;
						// assign group
						for (int i = 0; i < disjoint_groups.size(); ++i) {
							uint tmp = rand() % disjoint_groups[i];
							groups[e_id][i] = tmp;
							++count_sub_group[i][tmp];
						}
						id++;
					}

					// add neighbor
					neighbors[s_id].push_back(e_id);
					neighbors[e_id].push_back(s_id);
				}
			}

			item = 0;
		}
	}

	generate_sample(Constants::NO_SAMPLES); // generate sample graph

	//generate_txs();
	delete[] buffer;
	return true;
}

void DataSet::generate_samples_from_file(int no_nodes, string file, vector<uint> disjoint_groups, uint no_samples)
{
	string sample_file = Constants::SAMPLE_FOLDER + "sample_";
	string group_file = Constants::SAMPLE_FOLDER + "group.txt";
	clear();
	read_network_from_file(no_nodes, file, disjoint_groups);
	ofstream ofg(group_file);
	if (ofg.is_open()) {
		for (int i = 0; i < no_nodes; ++i) {
			ofg << i << "\t";
			for (uint j : groups[i]) ofg << j << "\t";
			ofg << endl;
		}
	}
	
	generate_sample(no_samples);

	for (uint i = 0; i < no_samples; ++i) {
		ofstream ofs(sample_file + to_string(i) + ".txt");
		if (ofs.is_open()) {
			for (int j = 0; j < no_nodes; ++j) {
				for (uint jj : sample_graphs[i][j]) {
					ofs << j << "\t" << jj << endl;
				}
			}
			ofs.close();
		}
	}
}

void DataSet::generate_graph(int no_nodes, double p)
{
	vector<vector<uint>> adj(no_nodes);
	for (int i = 0; i < no_nodes; ++i) {
		for (int j = i + 1; j < no_nodes; ++j) {
			uint r = rand() % 1000;
			double tmp = ((double)r) / 1000;
			if (tmp <= p) adj[i].push_back(j);
		}
	}

	ofstream f("data/random.txt");
	if (f.is_open()) {
		for (int i = 0; i < no_nodes; ++i) {
			for (auto j : adj[i]) f << i << "\t" << j << endl;
 		}
		f.close();
	}
}

void DataSet::read_sampled_graph(int no_nodes, vector<uint> disjoint_groups)
{
	clear();

	n = no_nodes;
	groups = vector<vector<uint>>(no_nodes, vector<uint>(disjoint_groups.size(), 0));
	count_sub_group = vector<vector<uint>>(disjoint_groups.size());
	for (int i = 0; i < disjoint_groups.size(); ++i)
		count_sub_group[i] = vector<uint>(disjoint_groups[i], 0);

	string file_group = Constants::SAMPLE_FOLDER + "group.txt";
	ifstream ifg(file_group);
	if (ifg.is_open()) {
		uint id, g1, g2;
		while (ifg >> id >> g1 >> g2) {
			groups[id][0] = g1;
			groups[id][1] = g2;
			count_sub_group[0][g1]++;
			count_sub_group[1][g2]++;
		}
		ifg.close();
	}
	
	string file_sample = Constants::SAMPLE_FOLDER + "sample_";
	sample_graphs = vector<Adj>(Constants::NO_SAMPLES, Adj(no_nodes));
	for (int i = 0; i < Constants::NO_SAMPLES; ++i) {
		ifstream ifs(file_sample + to_string(i) + ".txt");
		if (ifs.is_open()) {
			uint u, v;
			while (ifs >> u >> v) {
				sample_graphs[i][u].push_back(v);
			}
			ifs.close();
		}
	}
}

uint DataSet::sample_network_ic(const vector<uint> & seeds, const uint & group_kind, const uint & sub_group_kind)
{
	uint re = 0;
	vector<bool> checked(n, false);
	queue<uint> q;
	for (auto e : seeds) {
		q.push(e);
		checked[e] = true;
	}
	while (!q.empty()) {
		uint e = q.front();
		q.pop();

		if (groups[e][group_kind] == sub_group_kind) ++re;

		for (auto u : neighbors[e]) {
			if (!checked[u]) {
				uint r = Common::getInstance()->randomInThread(omp_get_thread_num()) % (2*neighbors[u].size());
				if (r == 0) {
					q.push(u);
					checked[u] = true;
				}
			}
		}
	}

	return re;
}

uint DataSet::sample_network_lt(const vector<uint> & seeds, const uint & group_kind, const uint & sub_group_kind)
{
	uint re = 0;

	vector<bool> checked(n, false);
	vector<uint> activate_node(n, n+1);
	queue<uint> q;
	for (auto e : seeds) {
		q.push(e);
		checked[e] = true;
	}
	while (!q.empty()) {
		uint e = q.front();
		q.pop();

		if (groups[e][group_kind] == sub_group_kind) ++re;

		for (auto u : neighbors[e]) {
			if (!checked[u]) {
				if (activate_node[u] == n + 1) {
					uint r = Common::getInstance()->randomInThread(omp_get_thread_num()) % (2*neighbors[u].size()); // 1/2 being active
					activate_node[u] = r < neighbors[u].size() ? neighbors[u][r] : n;
				} 

				if (activate_node[u] == e) {
					q.push(u);
					checked[u] = true;
				}
			}
		}
	}

	return re;
}

uint DataSet::sample_network_neighbor(const vector<uint>& seeds, const uint & group_kind, const uint & sub_group_kind)
{
	uint re = 0;

	vector<bool> checked(n, false);
	for (auto e : seeds) {
		if (!checked[e]) {
			++re;
			checked[e] = true;
		}

		for (auto nei : neighbors[e]) {
			if (!checked[nei]) {
				++re;
				checked[nei] = true;
			}
		}
	}
	
	return re;
}

double DataSet::get_from_sampled_graphs(const vector<uint>& seeds, const uint & group_kind, const uint & sub_group_kind)
{
	double re = 0.0;
	for (Adj g : sample_graphs) {
		vector<bool> checked(n, false);
		for (uint e : seeds) {
			if (!checked[e]) {
				checked[e] = true;
				if (groups[e][group_kind] == sub_group_kind) ++re;
				for (uint u : g[e]) {
					checked[u] = true;
					if (groups[u][group_kind] == sub_group_kind) ++re;
				}
			}
		}
	}
	return re / Constants::NO_SAMPLES;
}

uint DataSet::get_n()
{
	return n;
}

vector<uint> DataSet::getDegrees()
{
	vector<uint> degree(n, 0);
	for (int i = 0; i < n; ++i) degree[i] = neighbors[i].size();
	return degree;
}

bool DataSet::read_movie_dataset(int no_movies, int no_dimensions, int no_users, string movie_file, string rate_file)
{
	clear();
	n = no_movies;
	this->no_users = no_users;
	vector<vector<double>> feature(no_movies, vector<double>(no_dimensions, 0.0));
	ifstream f_movie(movie_file), f_rate(rate_file);
	if (f_movie.is_open() && f_rate.is_open()) {
		string m_row, r_row;

		getline(f_movie, m_row); // ignore header
		
		// since id in dataset is not 1,2,3,4
		map<int, int> mapMovieId;
		int id = 1;
		mapMovieId[1] = 0;
		// read to form features of each movie
		while (getline(f_movie, m_row)) {
			vector<string> cell; // movie id, tag id, score
			stringstream ss(m_row);
			string s;
			while (getline(ss, s, ',')) cell.push_back(s);
			int mId = stoi(cell[0]);
			if (!mapMovieId[mId] && mId != 1) {
				mapMovieId[mId] = id;
				++id;
			}
			feature[mapMovieId[mId]][stoi(cell[1]) - 1] = stod(cell[2]);
		}


		// read user's favorite movies
		user_favorate_movies = vector<vector<uint>>(no_users);
		getline(f_rate, r_row); // ignore header
		while (getline(f_rate, r_row)) {
			vector<string> cell; // userId,movieId,rating,timestamp
			stringstream ss(r_row);
			string s;
			while (getline(ss, s, ',')) cell.push_back(s);
			if (stoi(cell[0]) > no_users) break;
			if (stod(cell[2]) >= 4.1) {
				user_favorate_movies[stoi(cell[0]) - 1].push_back(mapMovieId[stoi(cell[1])]);
			}
		}

		// calculate similarity
		similarity = vector<vector<double>>(n, vector<double>(n, 1.0));
		for (int i = 0; i < n; ++i) {
			#pragma omp parallel for
			for (int j = i + 1; j < n; ++j) {
				double sim = get_cosine_similarity(feature[i], feature[j]);
				similarity[i][j] = sim;
				similarity[j][i] = sim;
			}
		}

		// get max threshold for each user
		max_threshold = vector<double>(no_users, 0.0);
		movie2user = vector<vector<double>>(no_users, vector<double>(no_movies, 0.0));
		vector<uint> movie_ids;
		for (int i = 0; i < no_movies; ++i) movie_ids.push_back(i);
		for (int i = 0; i < no_users; ++i) {
			sort(user_favorate_movies[i].begin(), user_favorate_movies[i].end());
			vector<uint> non_fav = Common::getInstance()->set_minus(movie_ids, user_favorate_movies[i]); // list of non favorite movies of user i
			for (uint m1 : non_fav) {
				// first way
				//for (uint m2 : user_favorate_movies[i]) max_threshold[i] += similarity[m1][m2];
				
				// second way
				double max_sim = 0.0;
				for (uint m2 : user_favorate_movies[i]) {
					max_sim = max_sim < similarity[m1][m2] ? similarity[m1][m2] : max_sim;
				}
				movie2user[i][m1] = max_sim;
				max_threshold[i] += max_sim;
			}
		}


		f_movie.close();
		f_rate.close();
	}
	return false;
}

double DataSet::get_cosine_similarity(const vector<double>& A, const vector<double>& B)
{
	int d = A.size();
	double sim = 0, A_len = 0.0, B_len = 0.0;
	for (int i = 0; i < d; ++i) {
		sim += A[i] * B[i];
		A_len += A[i] * A[i];
		B_len += B[i] * B[i];
	}

	sim /= (sqrt(A_len) * sqrt(B_len));
	return sim;
}

double DataSet::get_f_user(const vector<uint>& seeds, const uint & user_id)
{
	double re = 0.0;
	vector<uint> mov = Common::getInstance()->set_minus(seeds, user_favorate_movies[user_id]);
	for (uint m1 : mov) {
		// first way
		// for (uint m2 : user_favorate_movies[user_id]) re += similarity[m1][m2];
		
		// second way
		re += movie2user[user_id][m1];
	}
	return re;
}

vector<vector<double>> DataSet::generateThreshold(double p)
{
	if (Constants::D_TYPE == DATA_TYPE::Social) {
		vector<vector<double>> re(count_sub_group.size());
		for (int i = 0; i < count_sub_group.size(); ++i) {
			for (uint c : count_sub_group[i])
				re[i].push_back(p * ((double)c));
		}
		return re;
	}
	else {
		vector<vector<double>> re(1, vector<double>(no_users)); // since there is no_users constraints
		for (int i = 0; i < no_users; ++i) {
			re[0][i] = p * max_threshold[i];
		}
		return re;
	}
	
}


void DataSet::clear()
{
	neighbors.clear();
	mapNodeId.clear();
	groups.clear();
	sample_graphs.clear();
	count_sub_group.clear();

	similarity.clear();
	user_favorate_movies.clear();
	max_threshold.clear();
}

void DataSet::generate_sample(uint no_samples)
{
	sample_graphs = vector<Adj>(no_samples, Adj(n));
	#pragma omp parallel for
	for (int i = 0; i < no_samples; ++i) {
		Adj adj(n);
		// generate adj list first
		for (int u = 0; u < n; ++u) {
			if (Constants::IM_MODEL == INF_MODEL::IC) {
				for (uint v : neighbors[u]) {
					uint r = Common::getInstance()->randomInThread(omp_get_thread_num()) % neighbors[u].size();
					if (r == 0) {
						adj[v].push_back(u);
					}
				}
			}
			else {
				uint r = Common::getInstance()->randomInThread(omp_get_thread_num()) % neighbors[u].size(); // 1/2 being active
				if (r < neighbors[u].size()) adj[neighbors[u][r]].push_back(u);
			}
		}

		// generate connected set
		for (int u = 0; u < n; ++u) {
			queue<uint> q;
			vector<uint> checked(n, false);
			q.push(u);
			checked[u] = true;
			while (!q.empty()) {
				uint e = q.front();
				q.pop();
				for (uint v : adj[e]) {
					if (!checked[v]) {
						sample_graphs[i][u].push_back(v);
						checked[v] = false;
						q.push(v);
					}
				}
			}
		}
	}
}
