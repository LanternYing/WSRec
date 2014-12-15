/********************************************************
 * LN_LFM.h: header file of LN_LFM.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/12/12
********************************************************/

#include <algorithm>
#include <iostream>
using namespace std;


/* Perform the core approach of LN_LFM */
void LN_LFM(double *removedData, double *predData, double *wsInfoData, int numUser, 
	int numService, int numProvider, int numCountry, int dim, double lmda, int maxIter, 
    double eta, double alpha, double *Udata, double *Sdata, double *Pdata, double *Cdata, 
    double *bu, double *bs, double *bp, double *bc, bool debugMode);

/* Compute the loss value */
double loss(double **U, double **S, double **P, double **C, double *bu, double *bs, 
    double *bp, double *bc, double miu, double **removedMatrix, double **wsInfoList,
    double lambda, double alpha, int numUser, int numService, int numProvider, 
    int numCountry, int dim);

/* Update predMatrix */
void updatePredMatrix(double **predMatrix, double **U, double **S, double **P, double **C, 
    double *bu, double *bs, double *bp, double *bc, double miu, double **wsInfoList, 
    double alpha, int numUser, int numService, int dim);

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


