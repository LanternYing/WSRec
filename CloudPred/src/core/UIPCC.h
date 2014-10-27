/********************************************************
 * UIPCC.h: header file of UIPCC.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/4/29
 * Last updated: 2014/4/29
********************************************************/

#include <utility>
#include <vector>
#include <map>
#include "NMF.h"
using namespace std; 


void UPCC_core(double *removedData, double *UData, double *uMean, int numUser, int numService, 
	int dim, int topK, double *predData);

/* Compute pcc value between two vectors */
double getPCC(double *uA, double *uB, int dim);

/* Compute the average of a vector */
double getAverage(double *vec, int dim);

/* Sort a map by value, but return a vector */
bool cmpPairbyValue(const pair<int, double>& lhs, const pair<int, double>& rhs);
vector<pair<int, double> > sortMapByValue(const map<int, double>& pccMap);





