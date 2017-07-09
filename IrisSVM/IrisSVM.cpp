#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <algorithm>
#include <iterator>
#include <ctime>
using namespace std;

int main()
{
	srand(time(0));
	
	//Read in Iris Dataset
	const int dataRows = 150;
	const int dataCols = 5;

	float irisData[dataRows][dataCols];

	struct c_unique {
		int current;
		c_unique() { current = 0; }
		int operator()() { return current++; }
	} UniqueNumber;

	try
	{

		ifstream file("iris.csv");

		for (int row = 0; row < dataRows; row++)
		{
			string line;
			getline(file, line);

			if (!file.good())
				break;

			stringstream iss(line);

			for (int col = 0; col < dataCols; col++)
			{
				string val;
				getline(iss, val, ',');
				if (!iss.good())
					break;

				stringstream convertor(val);
				convertor >> irisData[row][col];
			}
		}
	}
	catch (const std::exception& e)
	{
		cerr << e.what();
	}


	//Output File Data
	/*
	for (int a = 0; a < dataCols; a++)
		{
			for (int i = 0; i < dataRows; i++)
			{
				cout << irisData[a][i] << "|";
			}
			cout << "\n";
		}

		cin.ignore();
	*/

	int randomisedIndices[150];
	int randomisedTrainingIndices[135];
	int randomisedTestingIndices[15];

	generate(begin(randomisedIndices), end(randomisedIndices), UniqueNumber);

	random_shuffle(begin(randomisedIndices), end(randomisedIndices));

	copy(randomisedIndices, randomisedIndices+135, randomisedTrainingIndices);
	copy(randomisedIndices+135, randomisedIndices + 150, randomisedTestingIndices);
	
	cin.ignore();

	return 0;
}