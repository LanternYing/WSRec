/********************************************************
 * UIPCC.h: header file of UIPCC.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/4/29
 * Last updated: 2014/4/29
********************************************************/

#include <utility>
#include <vector>
#include <map>
using namespace std; 

void UPCC_core(double *removedData, double *uMean, int numUser, int numService, 
	int topK, double *predData);

/* Transform a vector into a matrix */ 
double **vector2Matrix(double *vector, int row, int col);

/* Allocate memory for a 2D array */
double **createMatrix(int row, int col);

/* Free memory for a 2D array */ 
void delete2DMatrix(double **ptr);

/* Compute pcc value between two vectors */
double getPCC(double *uA, double *uB, double meanA, double meanB, int numUser);

/* Sort a map by value, but return a vector */
bool cmpPairbyValue(const pair<int, double>& lhs, const pair<int, double>& rhs);
vector<pair<int, double> > sortMapByValue(const map<int, double>& pccMap);





