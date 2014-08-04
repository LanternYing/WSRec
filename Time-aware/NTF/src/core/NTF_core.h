/********************************************************
 * NTF_core.h: header file of NTF_core.cpp
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
void NTF_core(double *removedData, double *predData, int numUser, int numService, 
	int numTimeSlice, int dim, double lmda, int maxIter, bool debugMode, 
	double *Udata, double *Sdata, double *Tdata);

/* Compute the loss value of NTF */
pdd lossFunction(bool ***I, double ***Y, double ***Y_hat, double **U, double **S, 
    double **T, int m, int n, int c, int d, double lmda);

/* Update the corresponding Y_hat */
double updateY_hat(double ***Y_hat, double **U, double **S, double **T, int m, int n, 
    int c, int d);

/* Transform a vector into a matrix */ 
double **vector2Matrix(double *vector, int row, int col);

/* Transform a vector into a 3D tensor */ 
double ***vector2Tensor(double *vector, int row, int col, int height);
bool ***vector2Tensor(bool *vector, int row, int col, int height);

/* Allocate memory for a 2D array */
double **createMatrix(int row, int col);

/* Free memory for a 2D array */ 
void delete2DMatrix(double **ptr); 

/* Get current date/time, format is YYYY-MM-DD hh:mm:ss */
const string currentDateTime();

