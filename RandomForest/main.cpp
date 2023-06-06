#include <fstream>
#include "randomforest.h"
#include "dataloader.h"

using namespace std;

/*
* Hyper-parameters. Adjust as you need.
*/
string train_data_path = "./data/train.csv";
string test_data_path = "./data/test.csv";
string res_path = "./res.csv";
int numtree = 100;
int depth = 10;
int numfea = 4;

int main() {
	// train random forest
	// read training set into memory
	DataLoader train_dataloader(train_data_path, "csv");
	vector<vector<Field>> TrainX = train_dataloader.file;

	// remove some columns
	cout << "Please input index of column you want to remove.\n";
	cout << "Notice that index should be count from 0\n";
	cout << "If you have input all you want, enter 'exit' to exit\n";
	vector<int> Columns;
	string state;
	cin >> state;
	while (state != "exit") {
		int index = atoi(state.c_str());
		if (index < 0 || index > TrainX.size() - 1)
			cout << "ERROR: index out of range\n";
		else
			Columns.push_back(index);
		cin >> state;
	}

	// select y label
	int y_Column = -1;
	cout << "Please select one row as y\n";
	cin >> y_Column;
	vector<string> Trainy(TrainX[0].size());
	for (int i = 0; i < TrainX[0].size(); i++)
		Trainy[i] = TrainX[y_Column][i].str_value;
	// erase needless columns
	Columns.push_back(y_Column);
	sort(Columns.begin(), Columns.end(), greater<int>());
	for (auto it : Columns)
		TrainX.erase(TrainX.begin() + it);

	// set hyper parameters
	string flag;
	cout << "Please enter hyper parameters, they are 'numtree', 'depth' and 'numfea'\n";
	cout << "Default numbers are 100, 10, 4. \n";
	cout << "If you want to change, enter 'y', otherwise enter 'n'";
	cin >> flag;
	if (flag == "y") {
		cout << "Use space bar or enter to separate numbers.";
		cin >> numtree >> depth >> numfea;
	}

	// build random forest
	RandomForest rf(numtree, depth, numfea);
	rf.build(TrainX, Trainy);
	cout << "\n/*** RandomForest: Build completed ***/\n";

	// predict
	DataLoader TestDataloader(test_data_path, "csv");
	vector<vector<Field>> TestX = TestDataloader.file;
	vector<string> header = TestDataloader.header;
	string ylabel = train_dataloader.header[y_Column];
	// remove columns
	for (auto it : Columns) {
		if (it > y_Column)
			TestX.erase(TestX.begin() + it - 1);
		else if (it < y_Column)
			TestX.erase(TestX.begin() + it);
	}
	vector<string> PredRes = rf.pred(TestX);

	// write results to csv file
	cout << "Please give columns you want to output\n";
	cout << "Notice that index should be count from 0\n";
	cout << "If you have input all you want, enter 'exit' to exit\n";
	vector<int> OutputColumns;
	cin >> state;
	while (state != "exit") {
		int index = atoi(state.c_str());
		if (index < 0 || index > TestX.size() - 1)
			cout << "ERROR: index out of range\n";
		else
			OutputColumns.push_back(index);
		cin >> state;
	}
	// write header to csv
	ofstream OutputFile(res_path, ios::out);
	for (int i = 0; i < OutputColumns.size(); i++) {
		if (i != 0)
			OutputFile << ",";
		OutputFile << header[OutputColumns[i]];
	}
	OutputFile << "," << ylabel << "\n";
	// write results and needed columns to csv
	for (int i = 0; i < PredRes.size(); i++) {
		for (int j = 0; j < OutputColumns.size(); j++) {
			if (j != 0)
				OutputFile << ",";
			OutputFile << TestX[OutputColumns[j]][i].str_value;
		}
		OutputFile << "," << PredRes[i] << "\n";
	}
	return 0;
}