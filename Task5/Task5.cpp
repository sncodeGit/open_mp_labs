#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include "omp.h"

using namespace std;

// For time execution
clock_t startTime = 0, endTime = 0;
// Name of file
char filename[] = "Task5_input.txt";

vector<size_t> searchSubstr(const string&, const string&, bool);

int main()
{	
	bool isParallel = false;
	string substr;

	cout << "Enter the search substring:" << endl;
	getline(cin, substr);
	cout << "Enter parallel or not (0 - false, other - true):" << endl;
	cin >> isParallel;

	string str, line;
	ifstream fin(filename);
	while(getline(fin, line))
	    str += line;
	fin.close();
	
	// Main part
	vector<size_t> pos = searchSubstr(str, substr, isParallel);

	// Output
	cout << endl << "Positions of occurrence of a substring in a string:" << endl;
	for (size_t i = 0; i < pos.size(); i++)
		cout << pos[i] << ' ';
	cout << endl;
	cout << endl << "Execution time: " << (endTime*1.0 - startTime) / CLOCKS_PER_SEC;
	cout << " sec." << endl;
	
	return 0;
}

vector<size_t> searchSubstr(const string& str, const string& substr, bool isParallel)
{
	bool isEqual = true;
	vector<size_t> pos;
	size_t strLen = str.length();
	size_t substrLen = substr.length();

	startTime = clock();

	#pragma omp parallel for default(shared) private(isEqual) schedule(dynamic) \
	if(isParallel)
	for (size_t i = 0; i < (strLen - substrLen + 1); i++)
	{
		isEqual = true;
		for (size_t j = 0; j < substrLen; j++)
			if (str[i + j] != substr[j])
			{
				isEqual = false;
				break;
			}
		
		if (!isEqual) continue;
		
		#pragma omp critical (push_back)
		{
			pos.push_back(i + 1);
		}
	}

	endTime = clock();
	
	return pos;
}
