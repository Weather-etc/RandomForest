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
	vector<string> header;
	vector<vector<string>> file;
	DataLoader(string path, string type);
};
