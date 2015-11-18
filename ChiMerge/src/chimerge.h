
#include <string>
#include <cstdlib>
#include <map>
#include <set>
#include <algorithm>
#include <unordered_set>
#include <boost/foreach.hpp>
#include <boost/range/combine.hpp>
#include "row.h"

using namespace std;

#ifndef CHIMERGE_H_
#define CHIMERGE_H_

template<class N, class C>
class ChiMerge {

public:
	
	static const bool verbose = false;

	// construtor - this two vectors are always need to initialize a chimerge class
	ChiMerge(vector<N>*, vector<C>*, int);

	//destructor
	virtual ~ChiMerge();

	// the values that should be discretized
	vector<N> *values;
	
	// this is the target that contain the class values
	vector<C> *target;

	// this is the size of the value and target array - both need to have the same size!
	int iSize;
	
	// this is a map that contains for each class the frequency
	map<C, int> mClassIndex;

	// this is a map that contains for each class the frequency
	map<C, int> mClassFrequency;

	// this is a map that contains for each unique value the class distribution
	map<N, int* > mValueFrequency;

	vector<N> ranges;
	
	//this is the core method that looks for the best splitting points of the values
	void fit();

	double dMin;


private:

	// a list with the entries - this is for calculating the chi square values
	struct cmp
	{
		bool operator()(Row<N>* a, Row<N>* b) 
		{
			double a_chi = a->chi();
			double b_chi = b->chi();
			if (a_chi == b_chi) {
				return a->value < b->value;
			} else {
				return a_chi < b_chi;
			}
		}
	};
	set<Row<N>*, cmp> sRows;



	// counts the class distribution overall and the frequency of each unique entry
	void count_classes();

	// counts the class distribution overall and the frequency of each unique entry
	void count_values();

	// create the tableau for the chi merge objects
	void tableau();
	
	// start merging of the ranges
	void merge();

	// prints the current rows
	void print();

};




template<class N, class C>
ChiMerge<N, C>::ChiMerge(vector<N>* par_values, vector<C>* par_target, int par_iSize)
{
	values = par_values;
	target = par_target;
	iSize = par_iSize;
	dMin = 4.6;
}


template<class N, class C>
ChiMerge<N, C>::~ChiMerge(){}


template<class N, class C>
void ChiMerge<N, C>::count_classes()
{
	C target_value;
	BOOST_FOREACH(target_value, *target) {
		mClassIndex[target_value] = 1;
	}
	int i = 0;
	for ( const auto &class_value : mClassIndex ) {
        mClassIndex[class_value.first] = i++;
    }
}


template<class N, class C>
void ChiMerge<N, C>::fit()
{

	count_classes();
	count_values();
	tableau();
	merge();


	if (verbose) print();
	
	if (sRows.size() > 1){
		for (Row<N> *r: sRows) {
			ranges.push_back(r->value);
		}
		std::sort(ranges.begin(), ranges.end());
		//ranges.erase(ranges.begin());
	}


}


template<class N, class C>
void ChiMerge<N, C>::count_values()
{
	C class_value;
	N value;

	int class_size = mClassIndex.size();

	BOOST_FOREACH(boost::tie(value, class_value), boost::combine(*values, *target)) {

		int *ptr;
		map<N, int* >::iterator it_values = mValueFrequency.find(value);
		if (it_values != mValueFrequency.end()){
			ptr = it_values->second;
		} else {
			int * array = new int[class_size];
			fill_n(array, class_size, 0);
			mValueFrequency[value] = array;
			ptr = array;
		}

		int index = mClassIndex[class_value];
		ptr[index] += 1;

	}
}



template<class N, class C>
void ChiMerge<N, C>::tableau()
{
	int class_size = mClassIndex.size();
	Row<N> *ptrLast = nullptr;
	for(map<N, int* >::iterator iterator_values = mValueFrequency.begin(); iterator_values != mValueFrequency.end(); iterator_values++)
	{
		N dValue = iterator_values->first;
		int *m = iterator_values->second;

		// create the row object that contains the frequencys, values
		// and offers methods for calculating the chi square value
		Row<N>* r = new Row<N>(dValue, m, class_size);
		
		// set the pointer
		r->last = ptrLast;
		if (ptrLast != nullptr)
		{
			ptrLast->next = r;
		}
		ptrLast = r;
		//insert the pointer of the row to the sorted set -> the problem is
		// that the next value have to be set. so we could only add the last last row.
		if (ptrLast != nullptr && ptrLast->last != nullptr){
			sRows.insert(ptrLast->last);
		}
	}
	sRows.insert(ptrLast);
}



template<class N, class C>
void ChiMerge<N, C>::merge(){

	Row<N> *r = (*sRows.begin());

	// while we have not reached the last range -> this has the max double chi square value
	while (r->chi() != std::numeric_limits<double>::max() && r->chi() < dMin)
	{
		
		if (verbose)cout << "-------------------------------------------" << endl;
		if (verbose)cout << "Merge: " << r->value << " with " << r->next->value << endl;
		
		//delete all the entries
		sRows.erase(r);
		if (r->next != nullptr) {sRows.erase(r->next);}
		if (r->last != nullptr) {sRows.erase(r->last);}

		for (int i = 0; i < r->iClassFrequency; i++)
		{
			r->lFrequency[i] +=  r->next->lFrequency[i];
		}
		
		// normal merge
		if (r->next->next != nullptr)
		{
			r->next = r->next->next;
			r->next->last = r;
		// if we have to merge with the last object
		} else
		{
			r->next = nullptr;
		}
		sRows.insert(r);
		if (r->last != nullptr) {sRows.insert(r->last);}

		if (verbose) print();
		if (verbose)cout << "-------------------------------------------" << endl;
		//cout << "Finished Merging: " << r->value << " with " <<  ptr->value << endl;
		if (verbose)cout << "-------------------------------------------" << endl;

		r = (*sRows.begin());
	}

}


template<class N, class C>
void ChiMerge<N, C>::print(){
	
	struct cmp{
		bool operator()(Row<N>* a, Row<N>* b) 
		{
			return a->value < b->value;
		}
	};
	set<Row<N>*, cmp> sPrint;

	for (Row<N> *r: sRows) {sPrint.insert(r);}
	for (Row<N> *r: sPrint) {r->print();}

}



#endif /* CHIMERGE_H_ */