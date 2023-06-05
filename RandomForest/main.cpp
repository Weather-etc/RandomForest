#include <iostream>

#include "randomforest.h"
#include "dataloader.h"

using namespace std;

string train_data_path = "./data/train.csv";
string test_data_path = "./data/test.csv";
int numtree = 1;
int depth = 3;
int numfea = 3;

int main() {
	// train random forest
	// read training set into memory
	DataLoader train_dataloader(train_data_path, "csv");
	vector<vector<Field>> TrainX = train_dataloader.file;
	// remove some columns
	cout << "Please input index of column you want to remove.\n";
	cout << "Notice that index should be count from 0\n";
	cout << "If you have input all you want, enter 'exit' to exit\n";
	string state;
	cin >> state;
	while (state != "exit") {
		int index = atoi(state.c_str());
		if (index < 0 || index > TrainX.size() - 1) {
			cout << "ERROR: index out of range";
			continue;
		}
		TrainX.erase(TrainX.begin() + index);
		cin >> state;
	}
	// select y label
	int y_Column = -1;
	cout << "Please select one row as y\n";
	cin >> y_Column;
	vector<string> Trainy(TrainX[0].size());
	for (int i = 0; i < TrainX[0].size(); i++)
		Trainy[i] = TrainX[y_Column][i].str_value;
	TrainX.erase(TrainX.begin() + y_Column);
	
	// build random forest
	RandomForest rf(numtree, depth, numfea);
	rf.build(TrainX, Trainy);

	return 0;
}