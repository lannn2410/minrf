This folder includes:
+ Source code of our algorithms: Alg0 (RandGr, Greedy, ThresGr, Sep), Alg1 and AlgR + Disjoint.
+ Facebook dataset (in "data" folder) for testing the algorithms. Due to size limit, we cannot include Movie dataset. In order to run movie recommendation, please download dataset at https://grouplens.org/datasets/movielens/20m/; unzip the ml-20m.zip file and put movies.csv and ratings.csv to "data" folder.

To build our code, run:

```
	g++ -std=c++11 *.cpp -o fair -DIL_STD -fopenmp -g
```

After building, to run our code, run:
```
	./fair 	-t <data type, 0: social, 1: movie>
		-a <alpha value # default: 0.1>
		-T <T value # default: 0.1>
		-r <r value # default: 1>
		-a0 <algorithm for r=0 (run alone if r=0, in combination with algr if r > 0), 0: randgr (default value), 1: greedy, 2: thresholdgr, 3: sep>
		-a1 <algorithm for r=1 (only valid if r=1), 0: alg1 (default), 1: algr, 2: disjoint>
		-ar <algorithm for r>1, 0: algr (default), 1: disjoint>
		-p <number of threads to running algorithms, default: 4>
```
Or:
```
	./fair -h
```
to print help.

We conducted experiments on a Linux machine with 2.3Ghz Xeon 18 core processor and 256Gb of RAM.
