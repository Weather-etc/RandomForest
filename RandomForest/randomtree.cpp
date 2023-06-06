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
	Node* root;
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
	for (auto it : columns) {
		map<string, pair<int, int>> CountTimes;
		vector<pair<string, pair<int, int>>> times_vec;
		for (int i = 0; i < y.size(); i++) {
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
		float total = X[0].size();
		float total_state = 0.0;
		float total_L_class0 = 0.0;
		float total_L_class1 = 0.0;
		float total_R_class0 = 0.0;
		float total_R_class1 = 0.0;
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
		Node state = *root;
		while (state.isLeaf != true && state.R_Vec.size() != 0) {
			int attr = state.attr;
			bool flag = false;
			for (int j = 0; j < state.R_Vec.size(); j++)
				if (X[attr][i].str_value == state.R_Vec[j]) 
					flag = true;
			if (flag == false) {
				if (state.L_Child != nullptr)
					state = *state.L_Child;
				else
					break;
			}
			else {
				if (state.R_Child != nullptr)
					state = *state.R_Child;
				else
					break;
			}
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
	Node* split(vector<vector<Field>> X, vector<int> y, int depth);

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
Node* RandomTree_RI::split(vector<vector<Field>> X, vector<int> y, int depth) {
	// TODO: stop condition
	if (depth >= this->maxDep) {
		Node* leafNode = (Node*)malloc(sizeof(Node));
		new (&leafNode->R_Vec) vector<string>();
		leafNode->isLeaf = true;
		leafNode->res = decideRes(y);
		return leafNode;
	}
	if (y.size() == 0)
		return nullptr;

	// randomly select features: Knuth Durstenfeld Shuffle Alg.
	int columns = X.size();
	vector<int> indexes(columns);
	vector<int> selected(this->numFea);
	iota(indexes.begin(), indexes.end(), 0);
	for (int i = 0; i < this->numFea; i++) {
		int j = rand() % (columns - i);
		swap(indexes[j], indexes[columns - i - 1]);
		selected[i] = indexes[columns - i - 1];
	}

	// do split using variance criterion
	vector<VarRes> CriRes = Var_Criterion(X, y, selected);
	// select best feature, save in (int)fea_sel. It should has minimal variance
	float min = CriRes[0].Var;
	int index = 0;
	vector<string> BranchR;
	int FeaSel = -1;
	for (int i = 0; i < CriRes.size(); i++) 
		if (CriRes[i].Var < min)
			index = i;
	BranchR = CriRes[index].BranchR;
	FeaSel = selected[index];

	Node* currNode = (Node*)malloc(sizeof(Node));
	new (&currNode->R_Vec) vector<string>();
	currNode->res = decideRes(y);
	currNode->attr = FeaSel;
	currNode->isLeaf = false;
	currNode->R_Vec = BranchR;
	GroupRes grouped = GroupData(X, y, BranchR, FeaSel);

	currNode->L_Child = split(grouped.L_Res, grouped.L_Y, depth + 1);
	currNode->R_Child = split(grouped.R_Res, grouped.R_Y, depth + 1);
	return currNode;
}