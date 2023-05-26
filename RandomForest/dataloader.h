#pragma once
#include <iostream>
#include <istream>
#include <streambuf>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <map>

using namespace std;

bool check_validity(int value, int i, int j) {
	if (value < i || value > j)
		return false;
	else
		return true;
}

struct Boost_data {
	vector<vector<string>> data;
	vector<int> indexes;
};

struct Field {
	int type;	// -1: undefined || 0:int || 1:float || 2:string
	int int_value;
	float float_value;
	string str_value;
};

/*
* class used to load data from dataset files, a few methods
* doing preprocessing will also be included.
*/
class DataLoader {
public:
	int count = 0;					// quantity of records
	vector<string> header;			// header of table
	vector<vector<Field>> file;		// data file of size (Columns, records)
	vector<int> count_null;			// amount of null value in each columns
	vector<bool> isClass;			// indicate whether the feature is a class, default true( is class

	DataLoader(string path, string type);
	bool removeColumn(int index);
	bool ColtoInt(int index);
	bool ColtoFloat(int index);
	bool SettoContinue(int index);

	Boost_data Bootstrap();
};
