#include <iostream>

#include "dataloader.h"

using namespace std;

string train_data_path = "./data/train.csv";
string test_data_path = "./data/test.csv";

int main() {
	DataLoader train_dataloader(train_data_path, "csv");
	return 0;
}