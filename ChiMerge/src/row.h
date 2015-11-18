
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>


using namespace std;

#ifndef ROW_H_
#define ROW_H_

template<class N> class Row {

public:

	Row(N, int*, int);
	virtual ~Row();
	bool Row<N>::operator==(const Row<N>&);

	N value;

	Row<N>* next;
	Row<N>* last;

	int *lFrequency;
	int iClassFrequency;


	double chi();
	void print();

private:

	double dChi;

};

template<class N>
bool Row<N>::operator==(const Row<N>& other) {
  return  value == other->value;
}



template<class N>
Row<N>::Row(N value, int * lFrequency, int iClassFrequency)
{
	this->value = value;
	this->lFrequency = lFrequency;
	this->iClassFrequency = iClassFrequency;
	
	next = nullptr;
	last = nullptr;

	dChi = -1;

}

template<class N>
Row<N>::~Row(){}


template<class N>
void Row<N>::print()
{
	//cout << "-----------------------" << endl;
	cout << setw(3) << value <<  "   ";
	cout << "l" << last << "   ";
	cout << "s" <<  this << "   ";
	cout << "n"<<  next << "   ";
	cout << this->chi() <<endl;
}


template<class N>
double Row<N>::chi()
{
	// if we had calculated this value before just return it
	//if (dChi != -1) return dChi;

	// else start to calculate the value
	dChi = 0;

	// if we have a next pointer -> this is need to calc the chi square value
	if (next != nullptr)
	{
		const int i_max = 2;
		const int j_max = iClassFrequency;
		
		int* C = new int[j_max];
		for (int j=0; j<j_max; j++) 
		{
			C[j] = 0; 
		
		}

		int* R = new int[i_max];
		for (int i=0; i<i_max; i++) 
		{
			R[i] = 0; 
		
		}


		int N = 0;

		for (int i = 0; i < i_max; i++)
		{
			for( int j = 0; j < j_max; j++ )
			{
				int tmp_value= (i == 0) ? lFrequency[j] : next->lFrequency[j];
				R[i] += tmp_value;
				C[j] += tmp_value;
				N += tmp_value;
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
				int tmp_value= (i == 0) ? lFrequency[j] : next->lFrequency[j];
				dChi += ((tmp_value - expected) * (tmp_value - expected) / denominator);
			}
		 }
	}
	
	// if there isn't a next value the chi square value is very high.
	// normally this is only for the last element that could not be a merge object
	else
	{
		dChi =  std::numeric_limits<N>::max();
	}

	return dChi;
}


#endif /* ROW_H_ */