#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "omp.h"

using namespace std;

size_t N1 = 0, M1 = 0, N2 = 0, M2 = 0;
// Boundaries of values of matrix elements
int leftBoard = -10, rightBoard = 10;
// For time execution
clock_t startTime = 0, endTime = 0;

void multiplyMatrices(const vector< vector<int> >&, const vector< vector<int> >&, \
                     vector< vector<int> >&, bool);
void printMatrix(const vector< vector<int> >&, const size_t N, const size_t M);
void fillMatrix(vector< vector<int> >&, const size_t N, const size_t M);

int main()
{	
	bool isParallel = false;
	
	cout << "Enter the number of rows in first matrix:" << endl;
	cin >> N1;
	cout << "Enter the number of rows and the number of columns";
	cout << " in the first and second matrix respectively:" << endl;
	cin >> M1;
	N2 = M1;
	cout << "Enter the number of columns in second matrix:" << endl;
	cin >> M2;
	cout << "Enter parallel or not (0 - false, other - true):" << endl;
	cin >> isParallel;

	vector< vector<int> > matrix1, matrix2, resultMatrix;
	srand(time(NULL));
	// Fill the primordial matrices
	fillMatrix(matrix1, N1, M1);
	fillMatrix(matrix2, N2, M2);
	// Fill the result matrix
	for (size_t i = 0; i < N1; i++)
	{
		vector<int> row(M2);
		resultMatrix.push_back(row);
	}

	// To check the correctness of the program on small matrices
	if (((N1 < 6) and (M1 < 6)) and ((N2 < 6) and (M2 < 6)))
	{
		cout << endl << "First matrix:" << endl;
		printMatrix(matrix1, N1, M1);
		cout << endl << "Second matrix:" << endl;
		printMatrix(matrix2, N2, M2);
	}

	// Main part
	multiplyMatrices(matrix1, matrix2, resultMatrix, isParallel);

	// To check the correctness of the program on small matrices
	if (((N1 < 6) and (M1 < 6)) and ((N2 < 6) and (M2 < 6)))
	{
		cout << endl << "Result matrix:" << endl;
		printMatrix(resultMatrix, N1, M2);
	}

	// Output
	cout << endl << "Execution time: " << (endTime*1.0 - startTime) / CLOCKS_PER_SEC;
	cout << " sec." << endl;
	
	return 0;
}

void printMatrix(const vector< vector<int> >& matrix, const size_t N, const size_t M)
{
		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < M; j++)
				cout << matrix[i][j] << ' ';
			cout << endl;
		}
}

void fillMatrix(vector< vector<int> >& matrix, const size_t N, const size_t M)
{
	for (size_t i = 0; i < N; i++)
	{
		vector<int> row(M);
		for (size_t j = 0; j < M; j++)
			row[j] = leftBoard + rand() % ((rightBoard + 1) - leftBoard);
		matrix.push_back(row);
	}
}

void multiplyMatrices(const vector< vector<int> >& matrix1, const vector< vector<int> >& matrix2, \
                     vector< vector<int> >& resultMatrix, bool isParallel)
{
	startTime = clock();

	int elemVal = 0;
	
	#pragma omp parallel default(shared) firstprivate(elemVal) if(isParallel)
	{
		#pragma omp for schedule(static)
		for (size_t i = 0; i < N1; i++)
			for (size_t j = 0; j < M2; j++)
			{
				elemVal = 0;
				for (size_t k = 0; k < N2; k++)
					elemVal += matrix1[i][k] * matrix2[k][j];
				resultMatrix[i][j] = elemVal;
			}
	}
	
	endTime = clock();

	return;
}
