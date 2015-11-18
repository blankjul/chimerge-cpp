/*
 * c++ dictino.cpp
 *
 *  Created on: 04.03.2014
 *      Author: jblank
 */

#include "row.h"
#include <sstream>
#include <iomanip>
#include <limits>



Row::Row(double value, vector<int> *lFrequency, int iClassFrequency)
{
	this->value = value;
	this->lFrequency = lFrequency;
	this->iClassFrequency = iClassFrequency;
	
	next = nullptr;
	last = nullptr;

	dChi = -1;

}

template<class N>
Row::~Row(){}

template<class N>
void Row::print()
{
	//cout << "-----------------------" << endl;
	cout << setw(3) << value <<  "   ";
	cout << "l" << last << "   ";
	cout << "s" <<  this << "   ";
	cout << "n"<<  next << "   ";
	for (int i = 0; i < iClassFrequency; i++)
	{
		cout << setw(3) << (*lFrequency)[i] << "   ";
	}
	cout << this->chi() <<endl;
}


template<class N>
double Row::chi()
{
	// if we had calculated this value before just return it
	//if (dChi != -1) return dChi;

	// else start to calculate the value
	dChi = 0;

	// if we have a next pointer -> this is need to calc the chi square value
	if (next != nullptr)
	{
		const int i_max = 2;
		const int j_max = 3;

		int freq[i_max][j_max];

		int R[2] = {0,0};
		int C[3] = {0,0,0};
		int N = 0;

		for (int i = 0; i < i_max; i++)
		{
			for( int j = 0; j < j_max; j++ )
			{
				freq[i][j] = (i == 0) ? lFrequency->at(j) : next->lFrequency->at(j);
				R[i] += freq[i][j];
				C[j] += freq[i][j];
				N += freq[i][j];
			}
	    }

		for (int i = 0; i < i_max; i++)
		{
			for( int j = 0; j < j_max; j++ )
			{
				double expected = (R[i] * C[j]) / double(N);
				double denominator = expected;
				if (denominator < 0.5){
					denominator = 0.5;
				}
				dChi += ((freq[i][j] - expected) * (freq[i][j] - expected) / denominator);
			}
		 }
	}
	
	// if there isn't a next value the chi square value is very high.
	// normally this is only for the last element that could not be a merge object
	else
	{
		dChi =  std::numeric_limits<double>::max();
	}

	return dChi;
}

template<class N>
Row* Row::merge(Row* a)
{
	
	// add the frequency of both
	int iFreq = iClassFrequency;
	double dValue = value;
	vector<int>* lFreq = new vector<int>(iFreq);
	for (int i = 0; i < iClassFrequency; i++)
	{
		(*lFreq)[i] = (*lFrequency)[i]  + (*a->lFrequency)[i];
	}

	// define the pointer to the last and next value
	Row* tmp =  new Row(dValue, lFreq, iFreq);
	(*tmp).last = last;
	(*tmp).next = a->next;

	return tmp;

}
