/********************************************************
 * RegionKNN.h: header file of RegionKNN.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/26
********************************************************/

#include <utility>
#include <vector>
#include <map>
using namespace std; 


/* Perform the core approach of RegionKNN */
void RegionKNN(double *removedData, double *initUserRegionData, int numUser, int numService, 
	int numInitUserRegion, int topK, double lmda, double mu_u, double *predData);

/* Create user regions */
vector<int> createUserRegion(vector<vector<int> > &userRegionDic, double **removedMatrix, 
	double **userRegionCenter, double **sensitiveServiceFlag, int numUser, int numService, 
	int numInitUserRegion, double lmda, double mu_u);

/* Compute missing values by UPCC */
double **UPCC(double **removedMatrix, map<int, int> &userRegion, vector<int> &regionIdx, 
	double **userRegionCenter, double **sensitiveServiceFlag, int numUser, int numService, 
	int numInitUserRegion, int numUserRegion, int topK);

/* Compute pcc value between two vectors */
double getPCC(double *uA, double *uB, double uA_mean, double uB_mean, int size);

/* Compute the average of a vector */
double getAverage(double *vec, int size);

/* Identify whether a service is sensitive to a region */
bool isSensitiveService(vector<double> &region, double mu);

/* Compute the median value of a vector */
double median(vector<double> &vec);

/* Transform a vector into a matrix */ 
double **vector2Matrix(double *vector, int row, int col);

/* Allocate memory for a 2D array */
double **createZeroMatrix(int row, int col);

/* Free memory for a 2D array */ 
void deleteMatrix(double **ptr); 

/* Allocate memory for a 1D array */
double *createVector(int size);

/* Free memory for a 1D array */ 
void deleteVector(double *ptr); 

/* Transpose a matrix */ 
void transpose(double **matrix, double **matrix_T, int row, int col);

/* Transform a matrix into a dictionary (data structure) */
vector<vector<int> > matrix2Dic(double **matrix, int row, int col);

/* Sort a map by value, but return a vector */
bool cmpPairbyValue(const pair<int, double>& lhs, const pair<int, double>& rhs);
vector<pair<int, double> > sortMapByValue(const map<int, double>& pccMap);
