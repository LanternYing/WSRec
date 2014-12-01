/********************************************************
 * NTF.h: header file of NTF.cpp
 * Author: Yuwen Xiong, Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/7/15
********************************************************/

#include <algorithm>
#include <iostream>
using namespace std;

typedef pair<double, double> pdd;
const double eps = 1e-10;

/* Perform the core approach of NTF */
void NTF(double *removedData, double *predData, int numUser, int numService, 
	int numTimeSlice, int dim, double lmda, int maxIter, bool debugMode, 
	double *Udata, double *Sdata, double *Tdata);

/* Compute the loss value of NTF */
pdd lossFunction(double ***Y, double ***Y_hat, double **U, double **S, double **T, 
    int numUser, int numService, int numTimeSlice, int dim, double lmda);

/* Update the corresponding Y_hat */
void updateY_hat(bool flag, double ***Y, double ***Y_hat, double **U, double **S, 
    double **T, int numUser, int numService, int numTimeSlice, int dim);

/* Transform a vector into a matrix */ 
double **vector2Matrix(double *vector, int row, int col);

/* Transform a vector into a 3D tensor */ 
double ***vector2Tensor(double *vector, int row, int col, int height);

/* Get current date/time, format is YYYY-MM-DD hh:mm:ss */
const string currentDateTime();

