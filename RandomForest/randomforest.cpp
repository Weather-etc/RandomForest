#include <vector>
#include <queue>
#include <map>
#include <set>
#include "randomtree.h"
using namespace std;

vector<int> AddVec(const vector<int>& a, const vector<int>& b) {
	if (a.size() != b.size()) {
		cout << "ERROR: sizes do not match in AddVec function\n";
		exit(1);
	}
	vector<int> res(a.size());
	for (int i = 0; i < a.size(); i++) 
		res[i] = a[i] + b[i];
	return res;
}

/*
* Please Notice that this is NOT SUITABLE for multiclass problems, and
*  it will raise errors if it is used directly on multiclass problems. 
*/

class RandomForest {
public:
	queue<RandomTree_RI> RiTrees;
	map<int, string> IntyDic;
	int Size = 0, Depth = 0, NumFea = 0;

	RandomForest(int size, int depth, int numfea);
	vector<int> _EncodeY(vector<string> y);
	void build(vector<vector<Field>> X, vector<string> y);
	vector<string> pred(vector<vector<Field>> X);
};

RandomForest::RandomForest(int size, int depth, int numfea) {
	Size = size;
	Depth = depth;
	NumFea = numfea;
	for (int i = 0; i < size; i++) {
		RiTrees.push(RandomTree_RI(depth, numfea, i));
	}
}

vector<int> RandomForest::_EncodeY(vector<string> y) {
	set<string> yvalues;
	map<string, int> StryDic;
	vector<int> EncodeRes(y.size());
	int encode = 0, index = 0;
	for (auto it : y)
		yvalues.insert(it);
	for (auto it : yvalues) {
		StryDic[it] = encode;
		IntyDic[encode] = it;
		encode++;
	}
	for (auto it : y) {
		EncodeRes[index] = StryDic[it];
		index++;
	}
	return EncodeRes;
}

void RandomForest::build(vector<vector<Field>> X, vector<string> y) {
	if (X.size() < NumFea) {
		cout << "ERROR: too much features in RandomForest.build()";
		exit(1);
	}
	RandomTree_RI state = RiTrees.front();
	int flag = state.ID;
	vector<int> y_encoded = _EncodeY(y);
	state.build(X, y_encoded);
	RiTrees.pop();
	RiTrees.push(state);
	while (flag != RiTrees.front().ID) {
		state = RiTrees.front();
		state.build(X, y_encoded);
		RiTrees.pop();
		RiTrees.push(state);
	}
	return;
}

// Do predict by voting within random trees
vector<string> RandomForest::pred(vector<vector<Field>> X) {
	cout << "\n*************************\n";
	cout << "IntyDic: \n";
	cout << "0: " << IntyDic[0] << "	" << "1: " << IntyDic[1] << "\n";
	cout << "*************************\n";
	vector<string> res(X[0].size());
	vector<int> ResInt(X[0].size());
	fill(ResInt.begin(), ResInt.end(), 0);
	int NumTrees = Size;

	RandomTree_RI state = RiTrees.front();
	RiTrees.pop();
	RiTrees.push(state);
	int flag = state.ID;
	ResInt = AddVec(ResInt, state.pred(X));

	while (flag != RiTrees.front().ID) {
		state = RiTrees.front();
		RiTrees.pop();
		RiTrees.push(state);
		ResInt = AddVec(ResInt, state.pred(X));
	}
	for (int i = 0; i < ResInt.size(); i++) {
		if (ResInt[i] != 0)
			res[i] = IntyDic[0];
		else
			res[i] = IntyDic[1];
	}
	return res;
}