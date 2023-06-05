#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <numeric>
#include <map>
#include <utility>
#include <algorithm>

#include "dataloader.h"

using namespace std;

struct VarRes {
	float Var;
	vector<string> BranchR;
};

struct GroupRes {
	vector<vector<Field>> L_Res;
	vector<int> L_Y;
	vector<vector<Field>> R_Res;
	vector<int> R_Y;
};

/*====================================  RandomForest_base  ============================================*/

/*
* node of random decision tree
*/
struct Node {
	bool isLeaf;					// True if it is a leaf node, False if not
	int attr;						// for internal node: record attr
	int res;						//for leaf node: record result
	vector<string> R_Vec;
	Node* L_Child, *R_Child;
};

class RandomTree_base {
public:
	Node root;
	int maxDep;
	int numFea;
	int ID;

	RandomTree_base(int maxdep, int numfea, int index);
	vector<VarRes> Var_Criterion
	(vector<vector<Field>> X, vector<int> y, vector<int> columns);
	GroupRes GroupData
	(vector<vector<Field>> X, vector<int> y, vector<string> BranchR, int fea);
	vector<int> pred(vector<vector<Field>> X);
};

RandomTree_base::RandomTree_base(int maxdep, int numfea, int index) {
	this->maxDep = maxdep;
	this->numFea = numfea;
	this->ID = index;
}

vector<VarRes> RandomTree_base::Var_Criterion
(vector<vector<Field>> X, vector<int> y, vector<int> columns) {
	vector<VarRes> res;
	//cout << "X[0].size: " << X[0].size() << "\n";
	//cout << "X.size: " << X.size() << "    \n";
	for (auto it : columns) {
		cout << it << "\n";
		map<string, pair<int, int>> CountTimes;
		vector<pair<string, pair<int, int>>> times_vec;
		for (int i = 0; i < X[0].size(); i++) {
			if (y[i] == 1) 
				CountTimes[X[it][i].str_value].second++;
			else
				CountTimes[X[it][i].str_value].first++;
		}
		for (auto it0 : CountTimes) 
			times_vec.push_back(pair<string, pair<int, int>>(it0.first, it0.second));
		sort(times_vec.begin(), times_vec.end(),
			[](pair<string, pair<int, int>> a, pair<string, pair<int, int>> b) 
			{return a.second.second < b.second.second; });
		
		int index = -1;
		float min = INFINITY;
		int total = X[0].size();
		int total_state = 0;
		int total_L_class0 = 0;
		int total_L_class1 = 0;
		int total_R_class0 = 0;
		int total_R_class1 = 0;
		for (int i = 0; i < times_vec.size(); i++) {
			total_R_class0 += times_vec[i].second.first;
			total_R_class1 += times_vec[i].second.second;
		}
		for (int i = 0; i < times_vec.size() - 1; i++) {
			total_state += times_vec[i].second.first + times_vec[i].second.second;
			total_L_class0 += times_vec[i].second.first;
			total_L_class1 += times_vec[i].second.second;
			total_R_class0 -= times_vec[i].second.first;
			total_R_class1 -= times_vec[i].second.second;
			float state = total_L_class0 * total_L_class1 / (total * total_state) +
				total_R_class0 * total_R_class1 / (total * (total - total_state));
			if (state < min) {
				min = state;
				index = i;
			}
		}
		VarRes res_state;
		res_state.Var = min;
		for (int i = index + 1; i < times_vec.size(); i++)
			res_state.BranchR.push_back(times_vec[i].first);
		res.push_back(res_state);
	}
	return res;
}

GroupRes RandomTree_base::GroupData
(vector<vector<Field>> X, vector<int> y, vector<string> BranchR, int fea) {
	GroupRes res;
	res.L_Res.resize(X.size());
	res.R_Res.resize(X.size());
	for (int i = 0; i < X[0].size(); i++) {
		bool flag = false;
		string state = X[fea][i].str_value;
		for (auto it : BranchR)
			if (it == state)
				flag = true;
		if (flag == true) {
			for (int j = 0; j < X.size(); j++)
				res.R_Res[j].push_back(X[j][i]);
			res.R_Y.push_back(y[i]);
		}
		else {
			for (int j = 0; j < X.size(); j++)
				res.L_Res[j].push_back(X[j][i]);
			res.L_Y.push_back(y[i]);
		}
	}
	return res;
}

vector<int> RandomTree_base::pred(vector<vector<Field>> X) {
	vector<int> res(X[0].size());
	for (int i = 0; i < X[0].size(); i++) {
		Node state = root;
		while (state.isLeaf != true && state.R_Vec.size() != 0) {
			int attr = state.attr;
			bool flag = false;
			for (int j = 0; j < X.size(); j++) {
				if (X[attr][i].str_value == state.R_Vec[j]) 
					flag = true;
			}
			if (flag == false)
				state = *state.L_Child;
			else
				state = *state.R_Child;
		}
		res[i] = state.res;
	}
	return res;
}

/*====================================  RandomForest_RI  ==============================================*/

// subclass: Used for randomforest-RI. See "Random Forest" by Leo Breiman, 2001
class RandomTree_RI : public RandomTree_base {
public:
	RandomTree_RI(int maxdep, int numfea, int index) :
		RandomTree_base(maxdep, numfea, index) {};
	void build(vector<vector<Field>> X, vector<int> y);
	Node split(vector<vector<Field>> X, vector<int> y, int depth);

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
	if (X[0].size() != y.size()) {
		cout << "ERROR in tree building: " << "X and y size donot match\n";
		exit(1);
	}
	cout << "ysize: " << y.size() << "\n";
	root = split(X, y, 0);
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
Node RandomTree_RI::split(vector<vector<Field>> X, vector<int> y, int depth) {
	// TODO: stop condition
	if (depth >= this->maxDep) {
		Node leafNode;
		leafNode.isLeaf = true;
		leafNode.res = decideRes(y);
		return leafNode;
	}

	// randomly select features: Knuth Durstenfeld Shuffle Alg.
	int columns = X.size();
	vector<int> indexes(columns);
	vector<int> selected(this->numFea);
	iota(indexes.begin(), indexes.end(), 1);
	for (int i = 0; i < this->numFea; i++) {
		int j = rand() % (columns - i);
		swap(indexes[j], indexes[columns - i - 1]);
		selected[i] = indexes[columns - i - 1];
	}

	cout << "select completed\n";
	// do split using variance criterion
	vector<VarRes> VarRes = Var_Criterion(X, y, selected);
	// select best feature, save in (int)fea_sel. It should has minimal variance
	float min = VarRes[0].Var;
	vector<string> BranchR;
	int FeaSel = 0;
	cout << "criteria completed\n";
	for (int i = 0; i < VarRes.size(); i++) {
		if (VarRes[i].Var < min) {
			min = VarRes[i].Var;
			BranchR = VarRes[i].BranchR;
			FeaSel = i;
		}
	}

	cout << "split completed\n";
	Node currNode;
	currNode.res = decideRes(y);

	cout << "decideRes completed\n";

	currNode.attr = FeaSel;
	currNode.isLeaf = false;
	currNode.R_Vec = BranchR;
	GroupRes grouped = GroupData(X, y, BranchR, FeaSel);

	cout << grouped.L_Res.size() << "\n";
	cout << grouped.R_Res.size() << "\n";

	cout << "recursiving\n";
	Node LNode = split(grouped.L_Res, grouped.L_Y, depth + 1);
	currNode.L_Child = &LNode;
	Node RNode = split(grouped.R_Res, grouped.R_Y, depth + 1);
	currNode.R_Child = &RNode;
	return currNode;
}