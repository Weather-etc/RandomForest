#pragma once
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "dataloader.h"
using namespace std;

/*
* node of random decision tree
*/
struct Node {
	bool isLeaf;					// True if it is a leaf node, False if not
	string attr;					// for internal node: record attr; for leaf node: record result
	map<string, Node> branch_map;	// record branches
};

struct RootNode {
	Node* node;
};

class RandomTree_base {
public:
	RootNode root;
	int maxDep;
	int numFea;
};

class RandomTree_RI : public RandomTree_base {
public:
	RandomTree_RI(int maxdep, int numfea);
	void build(vector<vector<Field>> X, vector<int> y);
	void split(vector<vector<Field>> X, vector<int> y, Node currNode, int depth);

	string decideRes(vector<int> y);

};