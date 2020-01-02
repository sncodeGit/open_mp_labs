#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "omp.h"

using namespace std;

size_t N = 0, M = 0;
// Boundaries of values of matrix elements
int leftBoard = -10, rightBoard = 10;
// For time execution
clock_t startTime = 0, endTime = 0;

void rowsMultiply(const vector< vector<int> >&, const vector<int>&, \
                     vector<int>&, bool);
void columnsMultiply(const vector< vector<int> >&, const vector<int>&, \
                     vector<int>&, bool);
void blocksMultiply(const vector< vector<int> >&, const vector<int>&, \
                     vector<int>&, bool);
void dispResult(const vector<int>&);

int main()
{	
	bool isParallel = false;
	
	cout << "Enter the number of rows in the matrix:" << endl;
	cin >> N;
	cout << "Enter enter the number of columns in the matrix";
	cout << "and the dimension of the vector:" << endl;
	cin >> M;
	cout << "Enter parallel or not (0 - false, other - true):" << endl;
	cin >> isParallel;

	vector< vector<int> > matrix;
	vector<int> vec(M), resVec(N);
	
	srand(time(NULL));
	for (size_t i = 0; i < N; i++)
	{
		vector<int> row(M);
		for (size_t j = 0; j < M; j++)
			row[j] = leftBoard + rand() % ((rightBoard + 1) - leftBoard);
		matrix.push_back(row);
	}
	for (size_t i = 0; i < M; i++)
		vec[i] = leftBoard + rand() % ((rightBoard + 1) - leftBoard);

	// To check the correctness of the program on small data
	if ((N < 6) and (M < 6))
	{
		cout << endl << "Matrix:" << endl;
		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < M; j++)
				cout << matrix[i][j] << ' ';
			cout << endl;
		}
		cout << endl << "vector:" << endl;
		for (size_t i = 0; i < M; i++)
			cout << vec[i] << ' ';
		cout << endl;
	}

	// Main part and output
	cout << endl << "----------" << endl << "Multiply by rows: " << endl;
	rowsMultiply(matrix, vec, resVec, isParallel);
	dispResult(resVec);
	
	cout << endl << "----------" << endl << "Multiply by columns: " << endl;
	columnsMultiply(matrix, vec, resVec, isParallel);
	dispResult(resVec);
	
	cout << endl << "----------" << endl << "Multiply by blocks: " << endl;
	blocksMultiply(matrix, vec, resVec, isParallel);
	dispResult(resVec);
	
	return 0;
}

void rowsMultiply(const vector< vector<int> >& matrix, const vector<int>& vec, \
                     vector<int>& resVec, bool isParallel)
{
	size_t elemVal = 0;

	startTime = clock();
	
	#pragma omp parallel for default(shared) firstprivate(elemVal) \
	schedule(static) if(isParallel)
	for (size_t i = 0; i < N; i++)
	{
		elemVal = 0;
		for (size_t j = 0; j < M; j++)
			elemVal += vec[j] * matrix[i][j];
		resVec[i] = elemVal;
	}

	endTime = clock();
}

void columnsMultiply(const vector< vector<int> >& matrix, const vector<int>& vec, \
                     vector<int>& resVec, bool isParallel){
	/* This should work in newer versions of g++ compiler (reduction with array)
	#pragma omp parallel default(shared) if(isParallel)
	{
		#pragma omp for schedule(static) reduction(+: resVec[:N])
		for (size_t i = 0; i < M; i++)
			for (size_t j = 0; j < N; j++)
				resVec[j] += matrix[j][i] * vec[i];
	}
	*/

	// Second variant

	size_t elemVal = 0;

	startTime = clock();
	
	for (size_t i = 0; i < N; i++)
	{
		elemVal = 0;
		#pragma omp parallel for schedule(static) reduction(+: elemVal) if(isParallel)
		for (size_t j = 0; j < M; j++)
			elemVal += matrix[i][j] * vec[j];
		resVec[i] = elemVal;
	}

	endTime = clock();
}

void blocksMultiply(const vector< vector<int> >& matrix, const vector<int>& vec, \
                     vector<int>& resVec, bool isParallel){
	startTime = clock();
	
	#pragma omp parallel shared(matrix, vec, resVec) if(isParallel)
	{
		size_t threadsNum = omp_get_num_threads();
		size_t height = N / threadsNum;
		size_t width = M / threadsNum;

		#pragma omp for
		for (size_t blockNum = 0; blockNum < threadsNum * threadsNum; blockNum++)
		{
			size_t a = blockNum / threadsNum;
			size_t b = blockNum % threadsNum;
			for (size_t i = a * height; i < (a + 1) * height; i++)
				for (size_t j = b * width; j < (b + 1) * width; j++)
					resVec[i] += matrix[i][j] * vec[j];
		}
	}

	endTime = clock();
}

void dispResult(const vector<int>& resVec){
	// To check the correctness of the program on small data
	if ((N < 6) and (M < 6))
	{
		cout << endl << "Result vector:" << endl;
		for (size_t i = 0; i < N; i++)
			cout << resVec[i] << ' ';
		cout << endl;
	}
	
	cout << endl << "Execution time: " << (endTime*1.0 - startTime) / CLOCKS_PER_SEC;
	cout << " sec." << endl;
}
