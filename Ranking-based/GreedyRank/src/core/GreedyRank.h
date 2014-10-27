/********************************************************
 * GreedyRank.h: header file of GreedyRank.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/6/16
********************************************************/

#include <vector>
#include <map>
using namespace std;


/* Perform the core approach of GreedyRank */
void GreedyRank(double *removedData, int numUser, int numService, int topK, int *predData);

/* Compute the KRCC similarity */
double getKRCC(double *uA, double *uB, int numService);

/* Compute the preference matrix between pairs of services */
void getPreferenceMatrix(double **removedMatrix, double *imean, int uid, 
	vector<pair<int, double> > &sortedkrccMap, int topK, double **preferenceMatrix, 
	int numUser, int numService);

/* Make the greedy ranking */
void greedyRank(double *removedQoSVector, double **preferenceMatrix, int *rankVector, 
	int numService, bool cloudrank);

/* Transform a vector into a matrix */ 
double **vector2Matrix(double *vector, int row, int col);
int **vector2Matrix(int *vector, int row, int col);

/* Allocate memory for a 2D array */
double **createZeroMatrix(int row, int col);

/* Free memory for a 2D array */ 
void deleteMatrix(double **ptr);

/* Allocate memory for a 1D array */
double *createVector(int size);

/* Free memory for a 1D array */ 
void deleteVector(double *ptr); 

/* Sort a map by value, but return a vector */
bool cmpPairbyValue(const pair<int, double>& lhs, const pair<int, double>& rhs);
vector<pair<int, double> > sortMapByValue(const map<int, double>& pccMap);
