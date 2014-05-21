/********************************************************
 * HMF_core.h: header file of HMF_core.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/18
********************************************************/

#include <utility>
#include <vector>
#include <map>
using namespace std; 


/* Perform the core approach of HMF */
void HMF_core(double *removedData, double *userGroup1, double *wsGroup1,
	double *userGroup2, double *wsGroup2, int numUser, int numService, 
	int numUser1, int numService1, int numUser2, int numService2, int dim, 
	double lmda, double lmda1, double lmda2, double alpha, int maxIter, 
	double etaInit, double *predData);

/* Compute the global loss value */
double globalLoss(double **U, double **S, double **removedMatrix, double **localMatrix, 
	double lmda, double alpha, int numUser, int numService, int dim);

/* Compute the local loss value */
double localLoss(double **U, double **S, double **removedMatrix, double *userGroup, 
	double *wsGroup, double lmda, int numUser, int numService, int dim);

/* Compute the gradients of the global loss function */
void gradGlobalLoss(double **U, double **S, double **removedMatrix, double **localMatrix, 
	double **predMatrix, double **gradU, double **gradS, double lmda, double alpha, 
	int numUser, int numService, int dim);

/* Compute the gradients of the local loss function */
void gradLocalLoss(double **U, double **S, double **removedMatrix, double **localMatrix, 
	double **gradU, double **gradS, double *userGroup, double *wsGroup, double lmda, 
	int numUser, int numService, int dim);

/* Update the predicted values in the training set */
void getTrainMatrix(double **U1, double **S1, double **U2, double **S2, double **U, 
	double **S, double ** removedMatrix, double **localMatrix, double **predMatrix, 
	double *userGroup1, double *wsGroup1, double *userGroup2, double *wsGroup2, 
	double alpha, int numUser1, int numService1, int numUser2, int numService2, 
	int numUser, int numService, int dim);

/* Compute the predicted values for missing entries */
void getPredMatrix(double **U1, double **S1, double **U2, double **S2, double **U, 
	double **S, double ** removedMatrix, double **localMatrix, double **predMatrix, 
	double *userGroup1, double *wsGroup1, double *userGroup2, double *wsGroup2, 
	double alpha, int numUser1, int numService1, int numUser2, int numService2, 
	int numUser, int numService, int dim);

/* Perform line search to find the best learning rate */
double lineSearchForGlobal(double **U, double **S, double **removedMatrix, 
	double lastLossValue, double **gradU, double **gradS, double etaInit, 
	double **localMatrix, double lmda, double alpha, int numUser, int numService, 
	int dim);
double lineSearchForLocal(double **U, double **S, double **removedMatrix, 
	double lastLossValue, double **gradU, double **gradS, double etaInit, 
	double *userGroup, double *wsGroup, double lmda, int numUser, int numService, 
	int dim);

/* Compute the dot product of two vectors */
double dotProduct(double *vec1, double *vec2, int len);

/* Transform a vector into a matrix */ 
double **vector2Matrix(double *vector, int row, int col);

/* Create a 2D array with zeros */
double **createZeroMatrix(int row, int col);

/* Create a 2D array with random values */
double **createRandomMatrix(int row, int col);

/* Free memory for a 2D array */ 
void deleteMatrix(double **ptr); 


