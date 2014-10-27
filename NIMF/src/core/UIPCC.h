/********************************************************
 * UIPCC.h
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/4/29
 * Last updated: 2014/5/14
********************************************************/

#include <utility>
#include <vector>
#include <map>
using namespace std; 

/* Compute similairty matrix */
vector<vector<pair<int, double> > > getPCCMatrix(double **removedMatrix, 
	int numUser, int numService, int topK, vector<vector<pair<int, double> > > 
	&topkSimUserDicRecord);

/* Transform a vector into a matrix */ 
double **vector2Matrix(double *vector, int row, int col);

/* Compute pcc value between two vectors */
double getPCC(double *uA, double *uB, double meanA, double meanB, int numUser);

/* Sort a map by value, but return a vector */
bool cmpPairbyValue(const pair<int, double>& lhs, const pair<int, double>& rhs);
vector<pair<int, double> > sortMapByValue(const map<int, double>& pccMap);

/* Allocate memory for a 1D array */
double *createVector(int size);

/* Free memory for a 1D array */ 
void deleteVector(double *ptr); 

/* Allocate memory for a 2D array */
double **createMatrix(int row, int col);

/* Free memory for a 2D array */ 
void deleteMatrix(double **ptr); 



