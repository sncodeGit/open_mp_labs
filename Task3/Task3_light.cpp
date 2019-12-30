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

	cin >> N;
	cin >> M;
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

	// Main part and output
	rowsMultiply(matrix, vec, resVec, isParallel);
	dispResult(resVec);

	columnsMultiply(matrix, vec, resVec, isParallel);
	dispResult(resVec);

	blocksMultiply(matrix, vec, resVec, isParallel);
	dispResult(resVec);

	return 0;
}

void rowsMultiply(const vector< vector<int> >& matrix, const vector<int>& vec, \
	vector<int>& resVec, bool isParallel)
{
	int elemVal = 0;

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
	vector<int>& resVec, bool isParallel) {
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

	int elemVal = 0;

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
	vector<int>& resVec, bool isParallel) {
	startTime = clock();
 
#pragma omp parallel shared(matrix, vec, resVec) if(isParallel)
	{
		int threadsNum = omp_get_num_threads();
		int height = N / threadsNum;
		int width = M / threadsNum;

#pragma omp for 
		for (int blockNum = 0; blockNum < threadsNum * threadsNum; blockNum++)
		{
			int a = blockNum / threadsNum;
			int b = blockNum % threadsNum;
			for (int i = a * height; i < (a + 1) * height; i++)
				for (int j = b * width; j < (b + 1) * width; j++)
					resVec[i] += matrix[i][j] * vec[j];
		}
	}

	endTime = clock();
}

void dispResult(const vector<int>& resVec) {
	cout << (endTime * 1.0 - startTime) / CLOCKS_PER_SEC << endl;
}
