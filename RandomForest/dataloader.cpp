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
	int count = 0;
	vector<string> header;
	vector<vector<string>> file;
	vector<int> count_null;

	DataLoader(string path, string type);
	vector<vector<string>> Bootstrap();
};

/*
* Load data into memory.
* NOTICE: This is ONLY suitable for Titanic dataset. DO NOT use it on other datasets directly.
* 
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
		int index = 0, pos = 0;
		while (getline(sin, word, ',')) {
			if (pos == 4 && count != 0) {
				pos++;
				continue;
			}
			words.push_back(word);
			if (count == 0)
				this->count_null.push_back(0);
			else
				if (word == "")
					this->count_null[index] += 1;
			index++;
			pos++;
		}

		if (count == 0)
			this->header = words;
		else
			this->file.push_back(words);
		count++;
	}
	csv_data.close();
	this->count = count - 1;

	cout << "-----------------dataset info-----------------\n";
	cout << "count of records: " << this->count << "\n";
	cout << "unknown values proportion:\n";
	for (int i = 0; i < this->header.size(); i++) {
		cout << this->header[i] << ": ";
		cout << float(this->count_null[i]) / float(this->count) << "\n";
	}
	cout << "------------------info end-------------------\n";
	return;
}

/*
* Classicial bootstrap method. Generalize data from dataset provided by random sampling 
* with replacement. It will be used to complete bagging.
* 
* REFERENCE: "Bagging Predictors" by Leo Breiman, 1996.
*/
vector<vector<string>> DataLoader::Bootstrap() {

}