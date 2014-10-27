/********************************************************
 * LACF.h: header file of LACF.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/16
********************************************************/

#include <utility>
#include <vector>
#include <map>
using namespace std; 


/* Perform the core approach of LACF */
void LACF(double *removedData, double *userGroupByASData, double *userGroupByCountryData,
	double *wsGroupByASData, double *wsGroupByCountryData, int numUser, int numService, 
	int topK, double lmda, double *predData);

/* Compute missing values by UPCC */
double **UPCC(double **removedMatrix, vector<vector<int> > &userGroupDicByAS, 
	vector<vector<int> > &userGroupDicByCountry, int numUser, int numService, int topK);

/* Compute pcc value between two vectors */
double getPCC(double *uA, double *uB, double meanA, double meanB, int numUser);

/* Cold-start problem handling */
double coldstartFill(double **removedMatrix, vector<vector<int> > &userGroupDicByAS, 
	vector<vector<int> > &userGroupDicByCountry, vector<vector<int> > &wsGroupDicByAS, 
	vector<vector<int> > &wsGroupDicByCountry, int userId, int wsId, int numUser, 
	int numService);

/* Transform a vector into a matrix */ 
double **vector2Matrix(double *vector, int row, int col);

/* Compute the dot product of two vectors */
double dotProduct(double *vec1, double *vec2, int len);

/* Allocate memory for a 2D array */
double **createMatrix(int row, int col);

/* Free memory for a 2D array */ 
void deleteMatrix(double **ptr); 

/* Allocate memory for a 1D array */
double *createVector(int size);

/* Free memory for a 1D array */ 
void deleteVector(double *ptr); 

/* Transpose a matrix */ 
void transpose(double **matrix, double **matrix_T, int row, int col);

/* Transform a matrix into a dictionary (data structure), the row and col are equal */
vector<vector<int> > matrix2Dic(double **matrix, int size);

/* Sort a map by value, but return a vector */
bool cmpPairbyValue(const pair<int, double>& lhs, const pair<int, double>& rhs);
vector<pair<int, double> > sortMapByValue(const map<int, double>& pccMap);
