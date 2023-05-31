#pragma once
#include <vector>
#include <queue>
#include <map>
#include "randomtree.h"
using namespace std;


class RandomForest {
public:
	queue<RandomTree_RI> ri_trees;
	map<int, string> IntyDic;

	RandomForest(int size, int depth, int numfea);
	void build(vector<vector<Field>> X, vector<string> y);
	vector<string> pred(vector<vector<Field>> X);
};