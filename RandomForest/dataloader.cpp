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

struct Field {
	int type;						// -1: undefined || 0:int || 1:float || 2:string
	int int_value;
	float float_value;
	string str_value;
};

struct Boost_data {
	vector<vector<Field>> data;
	vector<int> indexes;
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
		vector<string> WordsRes;
		int index = 0;
		sin.clear();
		sin.str(line);
		while (getline(sin, word, ','))
			words.push_back(word);
		for (int i = 0; i < words.size(); i++) {
			if (words[i] == "") {
				WordsRes.push_back(words[i]);
				count_null[index]++;
				index++;
				continue;
			}
			if (words[i][0] == '"') {
				string str = words[i];
				i++;
				string state = words[i];
				while (state[state.length() - 1] != '"') {
					str = str + "," + state;
					i++;
					state = words[i];
				}
				WordsRes.push_back(str);
				index++;
				continue;
			}
			WordsRes.push_back(words[i]);
			index++;
		}
		if (count == 0) {
			header = WordsRes;
			count_null.resize(header.size());
			file.resize(header.size());
		}
		else {
			while (WordsRes.size() < header.size()) {
				WordsRes.push_back("");
				count_null[WordsRes.size() - 1]++;
			}
			for (int i = 0; i < WordsRes.size(); i++) {
				Field NewField;
				NewField.type = 2;
				NewField.str_value = WordsRes[i];
				file[i].push_back(NewField);
			}
		}
		count++;
	}
	csv_data.close();
	this->count = count - 1;

	cout << "\n-----------------dataset info-----------------\n";
	cout << "count of records: " << this->count << "\n";
	cout << "unknown values proportion:\n";
	for (int i = 0; i < this->header.size(); i++) {
		cout << this->header[i] << ": ";
		cout << float(this->count_null[i]) / float(this->count) << "\n";
	}
	cout << "---------------------------------------------\n";
	return;
}

bool DataLoader::removeColumn(int index) {
	if (!check_validity(index, 0, this->header.size() - 1))
		return false;
	this->file.erase(this->file.begin() + index);
	this->header.erase(this->header.begin() + index);
	this->count_null.erase(this->count_null.begin() + index);
	return true;
}

bool DataLoader::ColtoInt(int index) {
	if (check_validity(index, 0, this->header.size() - 1) == false) {
		cout << "Warning: invalid index in ColtoInt func\n";
		return false;
	}
	for (int i = 0; i < this->count; i++) {
		this->file[index][i].type = 0;
		this->file[index][i].int_value = atoi(this->file[index][i].str_value.c_str());
	}
	return true;
}

bool DataLoader::ColtoFloat(int index) {
	if (check_validity(index, 0, this->header.size() - 1) == false) {
		cout << "Warning: invalid index in ColtoFloat func\n";
		return false;
	}
	for (int i = 0; i < this->count; i++) {
		this->file[index][i].type = 1;
		this->file[index][i].float_value = atof(this->file[index][i].str_value.c_str());
	}
	return true;
}

bool DataLoader::SettoContinue(int index) {
	if (check_validity(index, 0, this->header.size() - 1) == false) {
		cout << "Warning: invalid index in SettoContinue func\n";
		return false;
	}
	this->isClass[index] = false;
	return true;
}

/*
* Classicial bootstrap method. Generalize data from dataset provided by random sampling
* with replacement. It will be used to complete bagging.
* 
* return: a Boost_data struct, including vector<vector<Field>> randomly sampled from 
*		resource data and vector<int> indicates origin indexes.
*
* REFERENCE: "Bagging Predictors" by Leo Breiman, 1996.
*/
Boost_data DataLoader::Bootstrap() {
	Boost_data boost_data;
	vector<vector<Field>> data(this->count);
	vector<int> indexes(this->count);
	for (int i = 0; i < this->count; i++) {
		int index = (rand() % (this->count));
		indexes[i] = index;
		data[i] = this->file[index];
	}
	boost_data.data = data;
	boost_data.indexes = indexes;
	return boost_data;
}
