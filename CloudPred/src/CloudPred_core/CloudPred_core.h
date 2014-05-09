/********************************************************
 * CloudPred_core.h: header file of CloudPred_core.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/8
 * Last updated: 2014/5/8
********************************************************/

#include <utility>
#include <vector>
#include <map>
#include "NMF_core.h"
using namespace std; 

void UPCC_core(double *removedData, double *uMean, int numUser, int numService, 
	int topK, double *predData);

/* Compute pcc value between two vectors */
double getPCC(double *uA, double *uB, double meanA, double meanB, int numUser);

/* Sort a map by value, but return a vector */
bool cmpPairbyValue(const pair<int, double>& lhs, const pair<int, double>& rhs);
vector<pair<int, double> > sortMapByValue(const map<int, double>& pccMap);





