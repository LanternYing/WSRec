/********************************************************
 * PMF.h: header file of PMF.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/12
********************************************************/


/* Perform the core approach of PMF */
void PMF(double *removedData, int numUser, int numService, int dim, 
	double lmda, int maxIter, double etaInit, double *Udata, double *Sdata);

/* Compute the loss value of PMF */
double loss(double **U, double **S, double **removedMatrix,	double lmda, 
	int numUser, int numService, int dim);

/* Compute the gradients of the loss function */
void gradLoss(double **U, double **S, double **removedMatrix, double **gradU, 
	double **gradS, double lmda, int numUser, int numService, int dim);

/* Perform line search to find the best learning rate */
double linesearch(double **U, double **S, double **removedMatrix,
	double lastLossValue, double **gradU, double **gradS, double etaInit, 
	double lmda, int numUser, int numService, int dim);

/* Compute predMatrix */
void U_dot_S(double **removedMatrix, double **U, double **S, int numUser, 
		int numService, int dim, double **predMatrix);

/* Transform a vector into a matrix */ 
double **vector2Matrix(double *vector, int row, int col);

/* Compute the dot product of two vectors */
double dotProduct(double *vec1, double *vec2, int len);

/* Allocate memory for a 2D array */
double **createMatrix(int row, int col);

/* Free memory for a 2D array */ 
void delete2DMatrix(double **ptr); 




