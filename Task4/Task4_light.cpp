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

void blocksMultiply(const vector< vector<int> >&, const vector< vector<int> >&, \
                     vector< vector<int> >&, bool);
void tapesMultiply(const vector< vector<int> >& matrix1, const vector< vector<int> >& matrix2, \
                     vector< vector<int> >& resultMatrix, bool isParallel);
void printMatrix(const vector< vector<int> >&, const size_t N, const size_t M);
void fillMatrix(vector< vector<int> >&, const size_t N, const size_t M);
void printResult(const vector< vector<int> >&);

int main()
{	
	bool isParallel = false;
	size_t threadsNum = 1;
	
//	cout << "Enter the number of rows in first matrix:" << endl;
	cin >> N1;
//	cout << "Enter the number of rows and the number of columns";
//	cout << " in the first and second matrix respectively:" << endl;
	cin >> M1;
	N2 = M1;
//	cout << "Enter the number of columns in second matrix:" << endl;
	cin >> M2;
//	cout << "Enter parallel or not (0 - false, other - true):" << endl;
	cin >> isParallel;
	cin >> threadsNum;
	omp_set_num_threads(threadsNum);

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

	// Main part
//	cout << endl << "By blocks: " << endl;
	blocksMultiply(matrix1, matrix2, resultMatrix, isParallel);
	printResult(resultMatrix);
//	cout << endl << "By tapes: " << endl;
	tapesMultiply(matrix1, matrix2, resultMatrix, isParallel);
	printResult(resultMatrix);
	
	return 0;
}

void printResult(const vector< vector<int> >& resultMatrix)
{
	cout << (endTime*1.0 - startTime) / CLOCKS_PER_SEC << endl;
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

void blocksMultiply(const vector< vector<int> >& matrix1, const vector< vector<int> >& matrix2, \
                     vector< vector<int> >& resultMatrix, bool isParallel)
{
	startTime = clock();
	
	#pragma omp parallel default(shared) if(isParallel)
	{
		size_t threadsNum = omp_get_num_threads();
		size_t m1BlockHeight = N1 / threadsNum;
		size_t m1BlockWidth = N2 / threadsNum;
		size_t m2BlockWidth = M2 / threadsNum;
	#pragma omp for schedule(static)
		for(size_t m1BlockCount = 0; m1BlockCount < threadsNum * threadsNum; m1BlockCount++ )
		{
			size_t iBlockIndex = m1BlockCount / threadsNum;
			size_t jBlockIndex = m1BlockCount % threadsNum;
			for(size_t tBlockIndex = 0; tBlockIndex < threadsNum; tBlockIndex++)
			{
				for(size_t i = iBlockIndex * m1BlockHeight; i < (iBlockIndex + 1) * m1BlockHeight; i++)
					for(size_t j = jBlockIndex * m1BlockWidth; j < (jBlockIndex + 1) * m1BlockWidth; j++)
						for(size_t t = tBlockIndex * m2BlockWidth; t < (tBlockIndex + 1) * m2BlockWidth; t++)
							resultMatrix[i][t] += matrix1[i][j] * matrix2[j][t];				
			}
		}
	}

	endTime = clock();
	
	return;
}

void tapesMultiply(const vector< vector<int> >& matrix1, const vector< vector<int> >& matrix2, \
                     vector< vector<int> >& resultMatrix, bool isParallel)
{
	int elemVal = 0;
	
	startTime = clock();
	
	#pragma omp parallel for schedule(static) \
	default(shared) firstprivate(elemVal) if(isParallel)
	for (size_t i = 0; i < N1; i++)
		for (size_t j = 0; j < M2; j++)
		{
			elemVal = 0;
			for (size_t k = 0; k < N2; k++)
				elemVal += matrix1[i][k] * matrix2[k][j];
			resultMatrix[i][j] = elemVal;
		}
	
	endTime = clock();
}
