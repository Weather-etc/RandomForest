#pragma once
#include <vector>
#include <queue>
#include <map>
#include <set>
#include "randomtree.h"
using namespace std;

vector<int> AddVec(const vector<int>& a, const vector<int>& b);

/*
* Please Notice that this is NOT SUITABLE for multiclass problems, and
*  it will raise errors if it is used directly on multiclass problems.
*/

class RandomForest {
public:
	queue<RandomTree_RI> RiTrees;
	map<int, string> IntyDic;

	RandomForest(int size, int depth, int numfea);
	vector<int> _EncodeY(vector<string> y);
	void build(vector<vector<Field>> X, vector<string> y);
	vector<string> pred(vector<vector<Field>> X);
};