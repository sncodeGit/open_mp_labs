#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "omp.h"

using namespace std;

size_t N = 0, M = 0;
// Boundaries of values of matrix elements
int leftBoard = -100, rightBoard = 100;
// For time execution
clock_t startTime = 0, endTime = 0;

int getMaxOfMin(const vector< vector<int> >&, bool);

int main()
{	
	bool isParallel = false;
	
	cout << "Enter the number of rows:" << endl;
	cin >> N;
	cout << "Enter the number of columns:" << endl;
	cin >> M;
	cout << "Enter parallel or not (0 - false, other - true):" << endl;
	cin >> isParallel;

	vector< vector<int> > matrix;
	srand(time(NULL));
	for (size_t i = 0; i < N; i++)
	{
		vector<int> row(M);
		for (size_t j = 0; j < M; j++)
			row[j] = leftBoard + rand() % ((rightBoard + 1) - leftBoard);
		matrix.push_back(row);
	}

	// To check the correctness of the program on small matrices
	if ((N < 10) and (M < 10))
	{
		cout << endl << "Matrix:" << endl;
		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < M; j++)
				cout << matrix[i][j] << ' ';
			cout << endl;
		}
	}

	// Main part
	int maxElem = getMaxOfMin(matrix, isParallel);
	cout << endl << "Answer: " << maxElem;

	// Output
	cout << endl << "Execution time: " << (endTime*1.0 - startTime) / CLOCKS_PER_SEC;
	cout << " sec." << endl;
	
	return 0;
}

int getMaxOfMin(const vector< vector<int> >& matrix, bool isParallel)
{
	int maxElem = leftBoard;
	int minElem = rightBoard;

	startTime = clock();
	
	#pragma omp parallel for shared(matrix, N, M) firstprivate(minElem) \
	schedule(static) reduction(max: maxElem) if(isParallel) 
	for (size_t i = 0; i < N; i++)
	{
		minElem = rightBoard;
		for (size_t j = 0; j < M; j++)
			minElem = min(minElem, matrix[i][j]);
		maxElem = max(maxElem, minElem);
	}
	
	
	endTime = clock();

	return maxElem;
}
