#pragma once
#include <vector>
#include <string>
#include <map>
#include <iostream>

using namespace std;

/*
* node of random decision tree
*/
struct Node {
	bool isLeaf;	// True if it is a leaf node, False if not
	string attr;	// for internal node: record attr; for leaf node: record result
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

	RandomTree_base(int maxdep, int numfea);
	virtual void build(vector<vector<string>> X, vector<string> y) = 0;
};

class RandomTree_RI : public RandomTree_base {

};