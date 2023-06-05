#include "randomforest.h"
#include "dataloader.h"

using namespace std;

string train_data_path = "./data/train.csv";
string test_data_path = "./data/test.csv";
string res_path = "./res.csv";
int numtree = 3;
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
	
	// build random forest
	RandomForest rf(numtree, depth, numfea);
	cout << "\n-------------------- log --------------------\n";
	rf.build(TrainX, Trainy);
	cout << "RandomForest: Build completed\n";
	cout << "---------------------------------------------\n";

	// predict
	DataLoader TestDataloader(test_data_path, "csv");
	vector<vector<Field>> TestX = TestDataloader.file;
	// remove columns
	for (auto it : Columns)
		if (it != y_Column)
			TestX.erase(TestX.begin() + it);
	vector<string> PredRes = rf.pred(TestX);

	// write results to csv file
	cout << PredRes.size();
	return 0;
}