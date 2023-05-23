#include <iostream>
#include <istream>
#include <streambuf>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdlib.h>
using namespace std;

/*
* class used to load data from dataset files, a few methods
* doing preprocessing will also be included.
*/
class DataLoader {
public:
	vector<string> header;
	vector<vector<string>> file;
	DataLoader(string path, string type);
};

/*
* Load data into memory
* @params path: path of dataset file
* @params type: type of dataset file, only support csv file up to now
*/
DataLoader::DataLoader(const string path, const string type) {
	if (type != "csv") {
		cout << "ERROR: only csv file supported now\n";
		exit(1);
	}

	ifstream csv_data(path, ios::in);
	string line = "";

	if (!csv_data.is_open()) {
		cout << "ERROR: fail to open file " << path << "\n";
		exit(1);
	}

	istringstream sin;
	string word;

	// read data from dataset file
	int count = 0;
	while (getline(csv_data, line)) {
		vector<string> words;
		sin.clear();
		sin.str(line);
		while (getline(sin, word, ',')) {
			words.push_back(word);
		}
		if (count == 0)
			this->header = words;
		else
			this->file.push_back(words);
		count++;
	}
	csv_data.close();
	cout << "data load: successed\n";
	return;
}