/********************************************************
 * ADF.h: header file of ADF.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/4/29
 * Last updated: 2014/7/16
********************************************************/

#include <utility>
#include <vector>
#include <map>
using namespace std; 

/* Compute similarity matrix */
void SIM(double *removedData, double *uMean, int numUser, 
	int numService, int topK, double *simData, int type);

/* User-based ADF */
void PRED(double *removedData, double *removedRData, double *uMean, int numUser, 
	int numService, int topK, double *predData, double *simData);

/* Compute similarity value between two vectors */
double getSIMU(double *uA, double *uB, double meanA, double meanB, int numService);
double getSIMI(double *uA, double *uB, double *uMean, int numService);

/* Transform a vector into a matrix */ 
double **vector2Matrix(double *vector, int row, int col);

/* Allocate memory for a 2D array */
double **createMatrix(int row, int col);

/* Free memory for a 2D array */ 
void delete2DMatrix(double **ptr);

/* Sort a map by value, but return a vector */
bool cmpPairbyValue(const pair<int, double>& lhs, const pair<int, double>& rhs);
vector<pair<int, double> > sortMapByValue(const map<int, double>& pccMap);
