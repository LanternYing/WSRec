/********************************************************
 * Biased_MF.h: header file of Biased_MF.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/12/10
********************************************************/

#include <algorithm>
#include <iostream>
using namespace std;


/* Perform the core approach of Biased_MF */
void Biased_MF(double *removedData, double *predData, int numUser, 
	int numService, int dim, double lmda, int maxIter, double etaInit, 
    double *bu, double *bs, double *Udata, double *Sdata, bool debugMode);

/* Compute the loss value */
double loss(double **U, double **S, double *bu, double *bs, double miu, double **removedMatrix, 
	double **predMatrix, double lmda, int numUser, int numService, int dim);

/* Compute the gradients of the loss function */
void gradLoss(double **U, double **S, double *bu, double *bs, double miu, double **removedMatrix, 
	double **predMatrix, double **gradU, double **gradS, double *gradbu, double *gradbs, 
	double lmda, int numUser, int numService, int dim);

/* Perform line search to find the best learning rate */
double linesearch(double **U, double **S, double *bu, double *bs, double miu, 
	double **removedMatrix,	double lastLossValue, double **gradU, double **gradS, 
	double *gradbu, double *gradbs, double etaInit,	double lmda, int numUser, 
	int numService, int dim);

/* Update predMatrix */
void updatePredMatrix(bool flag, double **removedMatrix, double **predMatrix, double **U, 
	double **S, double *bu, double *bs, double miu, int numUser, int numService, int dim);

/* Transform a vector into a matrix */ 
double **vector2Matrix(double *vector, int row, int col);

/* Compute the dot product of two vectors */
double dotProduct(double *vec1, double *vec2, int len);

/* Allocate memory for a 2D array */
double **createMatrix(int row, int col);

/* Free memory for a 2D array */ 
void delete2DMatrix(double **ptr); 

/* Get system time info */
const string currentDateTime();


