/********************************************************
 * NRCF_core.h: header file of NRCF_core.cpp
 * Author: Yifei Lu, Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/4/29
 * Last updated: 2014/7/16
********************************************************/

#include <utility>
#include <vector>
#include <map>
using namespace std; 

/* Compute User-based NRCF */
void U_NRCF_core(double *removedData, double *uMax, double *uMean, int numUser, 
	int numService, int topK, double *predData);

/* Compute similarity value between two vectors */
double getNR(double *uA, double *uB, double maxA, 
	double maxB, int numService);

/* Transform a vector into a matrix */ 
double **vector2Matrix(double *vector, int row, int col);

/* Allocate memory for a 2D array */
double **createMatrix(int row, int col);

/* Free memory for a 2D array */ 
void delete2DMatrix(double **ptr);

/* Sort a map by value, but return a vector */
bool cmpPairbyValue(const pair<int, double>& lhs, const pair<int, double>& rhs);
vector<pair<int, double> > sortMapByValue(const map<int, double>& pccMap);





