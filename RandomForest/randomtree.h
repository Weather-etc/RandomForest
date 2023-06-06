#pragma once
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
	Node* L_Child, * R_Child;
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