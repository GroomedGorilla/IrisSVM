#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <algorithm>
#include <iterator>
#include <ctime>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "svm.h"
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

using namespace std;

const int dataRows = 150;
const int dataCols = 5;
const int problemSize = 135;
const int predictionSize = 15;

struct c_unique {
		int current;
		c_unique() { current = 0; }
		int operator()() { return current++; }
	} UniqueNumber;

struct svm_parameter param;
struct svm_problem prob;
struct svm_problem probInc;
struct svm_model *model;
struct svm_model *modelInc;
struct svm_node *x_space;
struct svm_node *x_spaceInc;

int main()
{
	//**Set the following parameters at the start of each run**
	int taskNum = 3;
/*	1 = SVM
	2 = One-vs-One Multiclass
	3 = Incremental Multiclass
*/
	int incrementalTrainingSteps = 50; //**Set at start when running incremental training**

	bool incrementalTraining = false;
	
	if (taskNum == 3)
		incrementalTraining = true;
	
	
	srand(time(0));

	param.svm_type = C_SVC;
	param.kernel_type = RBF;
	param.degree = 3;
	param.gamma = 0.5;
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = 1;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;


	//Read in Iris Dataset from CSV
	float irisData[dataRows][dataCols];

	////Load Data Set
	//###############################################################
	
	// Task 1 - I.Setosa = 0; Not I.Setosa = 1;
	//ifstream file("irisBinaryClass.csv");

	//Task 2/3 - Multi-Class - One-vs-One Classification
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

	// Problem Definition - LibSVM
	prob.l = problemSize; //For task 1 & 2
	probInc.l = problemSize; //Task 3 (incremental)

	// Randomise Indices to choose 90% (135 lines) of data for Training, 10% (15) for Testing
	int randomisedIndices[dataRows];
	int randomisedTrainingIndices[problemSize];
	int randomisedTestingIndices[predictionSize];

	generate(begin(randomisedIndices), end(randomisedIndices), UniqueNumber); //generate array with values 0 to 150

	random_shuffle(begin(randomisedIndices), end(randomisedIndices)); //Shuffle array values

	copy(randomisedIndices, randomisedIndices + problemSize, randomisedTrainingIndices); 
	copy(randomisedIndices + problemSize, randomisedIndices + dataRows, randomisedTestingIndices);

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

	cout << "File Loaded || Training/Testing Data Set";

	//Train Model
	//###############################################################
	
	if (incrementalTraining)
	{
		//Task 3 - Incremental Training

		svm_node** xinc = Malloc(svm_node*, 135);
		probInc.y = Malloc(double, 135);

		for (int row = 0; row < incrementalTrainingSteps; row++)
		{
			svm_node* x_spaceInc = Malloc(svm_node, 4);
			for (int col = 0; col < 4; col++)
			{
				x_spaceInc[col].index = col;
				x_spaceInc[col].value = irisData[randomisedTrainingIndices[row]][col];
			}
			x_spaceInc[4].index = -1;
			xinc[row] = x_spaceInc;

			probInc.x = xinc;

			//Setting Y values (targets) per iteration
			probInc.y[row] = irisData[randomisedTrainingIndices[row]][4];

			//Training
			svm_model *modelInc = svm_train(&probInc, &param);
		}
	}
	else
	{
		//Tasks 1 & 2
		
		//Assign Values to Matrix

		svm_node** x = Malloc(svm_node*, prob.l);

		for (int row = 0; row < prob.l; row++)
		{
			svm_node* x_space = Malloc(svm_node, 4);
			for (int col = 0; col < 4; col++)
			{
				x_space[col].index = col;
				x_space[col].value = irisData[randomisedTrainingIndices[row]][col];
			}
			x_space[4].index = -1;
			x[row] = x_space;
		}

		prob.x = x;

		//Setting Y values (targets)
		prob.y = Malloc(double, prob.l);

		//Load target values
		for (int row = 0; row < prob.l; row++)
		{
			prob.y[row] = irisData[randomisedTrainingIndices[row]][4];
		}

		//Training
		svm_model *model = svm_train(&prob, &param);		
	}

	//Test Model
	//Train Model
	//###############################################################

	//- Read test data + parse

	svm_node** testNode = Malloc(svm_node*, predictionSize);

	for (int row = 0; row < predictionSize; row++)
	{
		svm_node* test_space = Malloc(svm_node, 4);
		for (int col = 0; col < 4; col++)
		{
			test_space[col].index = col;
			test_space[col].value = irisData[randomisedTestingIndices[row]][col];
		}
		test_space[4].index = -1;
		testNode[row] = test_space;

		double retVal;

		if (incrementalTraining)
		{
			retVal = svm_predict(modelInc, test_space);
		}
		else
		{
			retVal = svm_predict(model, test_space);
		}
		cout << "Return Value: " << retVal << "\n";
		printf("Expected: %f\n\n", irisData[randomisedTestingIndices[row]][4]);
	}


	cin.ignore();

	svm_destroy_param(&param);
	free(prob.y);
	free(prob.x);
	free(x_space);
	free(probInc.y);
	free(probInc.x);
	free(x_spaceInc);

	return 0;

}