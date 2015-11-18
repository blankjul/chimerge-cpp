
#include <vector>
#include <string>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>


using namespace std;

struct Entry
{
	double x1;
	double x2;
	double x3;
	double x4;

	string target;
};


vector<Entry> readCsv();

vector<Entry> readCsv()
{

	vector<Entry> result;

	ifstream file ( "iris.csv" );
	string line;

	while(file.good())
	{
		Entry e;

		string tmp;
		getline(file, tmp, ',');
		e.x1 = atof(tmp.c_str());

		getline(file, tmp, ',') ;
		e.x2 = atof(tmp.c_str());

		getline(file, tmp, ',') ;
		e.x3 = atof(tmp.c_str());

		getline(file, tmp, ',') ;
		e.x4 = atof(tmp.c_str());

		getline(file, tmp) ;
		e.target = tmp;

		result.push_back(e);
	}
	return result;

}
