#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <numeric>
#include <map>

#include "dataloader.h"

using namespace std;

struct Var_res {
	float IG;
	int index;
};

struct Group_res {
	vector<vector<vector<Field>>> X_res;
	vector<vector<int>> y_res;
	vector<Field> values;
};

/*====================================  RandomForest_base  ============================================*/

/*
* node of random decision tree
*/
struct Node {
	bool isLeaf;					// True if it is a leaf node, False if not
	string attr;					// for internal node: record attr
	string res;						//for leaf node: record result
	map<Field, Node> branch_map;	// record branches
};

class RandomTree_base {
public:
	Node root;
	int maxDep;
	int numFea;

	RandomTree_base(int maxdep, int numfea);
	vector<Var_res> Var_criterion(vector<vector<Field>> X, vector<int> y, vector<int> columns);
	Group_res GroupData(vector<vector<Field>> X, vector<int> y, int fea);
};

RandomTree_base::RandomTree_base(int maxdep, int numfea) {
	this->maxDep = maxdep;
	this->numFea = numfea;
}

vector<Var_res> RandomTree_base::Var_criterion
(vector<vector<Field>> X, vector<int> y, vector<int> columns) {

}

Group_res RandomTree_base::GroupData(vector<vector<Field>> X, vector<int> y, int fea) {
	Group_res res;
	map<string, vector<int>> res_map;
	map<string, int> count_map;
	for (int i = 0; i < X[fea].size(); i++) {
		string str = X[fea][i].str_value;
		if (count_map[str] == 0) {
			count_map[str]++;
			vector<int> initial = { i };
			res_map[str] = initial;
		}
		else {
			count_map[str]++;
			res_map[str].push_back(i);
		}
	}
	for (auto it : res_map) {
		vector<vector<Field>> X_state;
		vector<int> y_state;
		vector<int> index_state = it.second;
		for (auto it0 : index_state) {
			vector<Field> X_state_rec;
			for (int i = 0; i < X.size(); i++) 
				X_state_rec.push_back(X[i][it0]);
			X_state.push_back(X_state_rec);
			y_state.push_back(y[it0]);
		}
		res.X_res.push_back(X_state);
		res.y_res.push_back(y_state);
		res.values.push_back(X_state[fea][0]);
	}
	return res;
}

/*====================================  RandomForest_RI  ==============================================*/

// subclass: Used for randomforest-RI. See "Random Forest" by Leo Breiman, 2001
class RandomTree_RI : public RandomTree_base {
public:
	RandomTree_RI(int maxdep, int numfea) : RandomTree_base(maxdep, numfea) {};
	void build(vector<vector<Field>> X, vector<int> y);
	void split(vector<vector<Field>> X, vector<int> y, Node currNode, int depth);

	int decideRes(vector<int> y);
};

void RandomTree_RI::build(vector<vector<Field>> X, vector<int> y) {
	int columns = X[0].size();
	// do some input checks
	if (columns < this->numFea) {
		cout << "ERROR in tree building: " << "only " << columns << " variables are available but "
			<< this->numFea << " are requested\n";
		exit(1);
	}
	if (X.size() != y.size()) {
		cout << "ERROR in tree building: " << "X and y size donot match\n";
		exit(1);
	}
	split(X, y, this->root, 0);
	return;
}

int RandomTree_RI::decideRes(vector<int> y) {
	map<int, int> count_map;
	for (int i = 0; i < y.size(); i++) {
		count_map[y[i]]++;
	}
	int max_key = 0;
	int max_value = 0;
	for (auto it : count_map) {
		if (it.second > max_value)
			max_key = it.first;
	}
	return max_key;
}

/* in each node we choose feature form a randomly choosed feature group, using criteria in CART
* 
* REFERENCE: 1. "Classification and Regression Trees", by L. Breiman et.al
*			2. "Random Forest", by Leo Breiman
*/
void RandomTree_RI::split(vector<vector<Field>> X, vector<int> y, Node currNode, int depth) {
	// TODO: stop condition
	if (depth >= this->maxDep) {
		Node leafNode;
		leafNode.isLeaf = true;
		leafNode.res = decideRes(y);
		return;
	}

	// randomly select features: Knuth Durstenfeld Shuffle Alg.
	int columns = X[0].size();
	vector<int> indexes(columns);
	vector<int> selected(this->numFea);
	iota(indexes.begin(), indexes.end(), 1);
	for (int i = 0; i < this->numFea; i++) {
		int j = rand() % (columns - i);
		swap(indexes[j], indexes[columns - i - 1]);
		selected[i] = indexes[columns - i - 1];
	}

	// do split using variance criterion
	vector<Var_res> info_gain = Var_criterion(X, y, selected);
	// TODO: select best feature, save in (int)fea_sel
	int fea_sel;

	currNode.res = decideRes(y);
	currNode.attr = fea_sel;
	currNode.isLeaf = false;
	Group_res grouped = GroupData(X, y, fea_sel);
	vector<vector<vector<Field>>> X_grouped = grouped.X_res;
	vector<vector<int>> y_grouped = grouped.y_res;
	vector<Field> values = grouped.values;
	for (int i = 0; i < X_grouped.size(); i++) {
		Node newNode;
		currNode.branch_map.insert(pair<Field, Node>(values[i], newNode));
		split(X_grouped[i], y_grouped[i], newNode, depth + 1);
	}
	return;
}