#include <vector>
#include <queue>
#include <map>
#include <set>
#include "randomtree.h"
using namespace std;


class RandomForest {
public:
	queue<RandomTree_RI> ri_trees;
	map<int, string> IntyDic;

	RandomForest(int size, int depth, int numfea);
	vector<int> EncodeY(vector<string> y);
	void build(vector<vector<Field>> X, vector<string> y);
	vector<int> pred(vector<vector<Field>> X);
};

RandomForest::RandomForest(int size, int depth, int numfea) {
	for (int i = 0; i < size; i++) {
		ri_trees.push(RandomTree_RI(depth, numfea));
	}
}

vector<int> RandomForest::EncodeY(vector<string> y) {
	set<string> yvalues;
	map<string, int> StryDic;
	vector<int> EncodeRes(y.size());
	int encode = 0, index = 0;
	for (auto it : y)
		yvalues.insert(it);
	for (auto it : yvalues) {
		StryDic[it] = encode;
		encode++;
	}
	for (auto it : y) {
		EncodeRes[index] = StryDic[it];
		index++;
	}
	return EncodeRes;
}

void RandomForest::build(vector<vector<Field>> X, vector<string> y) {
	RandomTree_RI flag = ri_trees.front();
	RandomTree_RI state = ri_trees.front();
	vector<int> y_encoded = EncodeY(y);
	state.build(X, y_encoded);
	ri_trees.pop();
	ri_trees.push(state);
	while (&flag != &ri_trees.front()) {
		state = ri_trees.front();
		state.build(X, y);
		ri_trees.pop();
		ri_trees.push(state);
	}
}

vector<string> RandomForest::pred(vector<vector<Field>> X) {
	vector<string> res(X[0].size());
	vector<string> votes(ri_trees.size());
	for (int i = 0; i < res.size(); i++) {
		
	}
}
