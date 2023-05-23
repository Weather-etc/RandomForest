#pragma once
#include <iostream>
#include <istream>
#include <streambuf>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdlib.h>
using namespace std;

class DataLoader {
public:
	int count = 0;
	vector<string> header;
	vector<vector<string>> file;
	vector<int> count_null;

	DataLoader(string path, string type);
	vector<vector<string>> Bootstrap();
};
