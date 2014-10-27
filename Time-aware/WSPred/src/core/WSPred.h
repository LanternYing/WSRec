/********************************************************
 * WSPred.h: header file of WSPred.cpp
 * Author: Yuwen Xiong, Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/7/19
********************************************************/

#include <algorithm>
#include <iostream>
using namespace std;

typedef pair<pair<double, double>, double> tuple;
const double eps = 1e-10;

/* Perform the core approach of WSPred */
void WSPred(double *removedData, double *predData, int numUser, int numService, 
	int numTimeSlice, int dim, double etaInit, double lmda, double alpha, int maxIter, 
	bool debugMode,	double *Udata, double *Sdata, double *Tdata);

/* Compute the loss value of WSPred */
tuple lossFunction(bool ***I, double ***Y, double ***Y_hat, double **Y_bar, double **U, 
	double **S, double **T, int m, int n, int c, int d, double lmda, double alpha);

/* Compute the gradients of the loss function */
void calcGrad(double **gradU, double **gradS, double **gradT, bool ***I, double ***Y, 
	double ***Y_hat, double **Y_bar, double **U, double **S, double **T, double lmda, 
	double alpha, int numUser, int numService, int numTimeSlice, int dim);

/* Update the corresponding latent vectors U, S, T */
void updateUST(double **aim, double **ori, double eta, double **grad, int m, int n);

/* Update the corresponding Y_hat */
void updateY_hat(double ***Y_hat, double **U, double **S, double **T, int m, int n, 
	int c, int d);

/* Perform line search to find the best learning rate */
pair<double, tuple> linesearch(bool ***I, double ***Y, double ***Y_hat, double **Y_bar, 
	double **U, double **S, double **T, double **gradU, double **gradS, double **gradT, 
    double **U_t, double **S_t, double ** T_t, double eta, double lmda, double alpha, 
    double lastLossValue, int numUser, int numService, int numTimeSlice, int dim);

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