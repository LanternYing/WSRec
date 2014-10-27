/********************************************************
 * LBR.h: header file of LBR.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/10
********************************************************/

/* Compute the location similarity */
void getLocSim_core(double *geoData, double *locSimData, 
	int numLine, double theta);

/* Perform the core approach of LBR */
void LBR(double *removedData, double *locSimData, int numUser, int numService, int dim, 
	double lmda, double alpha, int maxIter, double etaInit, double *Udata, double *Sdata);

/* Compute the loss value of LBR */
double loss(double **U, double **S, double **removedMatrix, double **locSim,
	double lmda, double alpha, int numUser, int numService, int dim);

/* Compute the gradients of the loss function */
void gradLoss(double **U, double **S, double **removedMatrix, double **gradU, 
	double **gradS, double **locSim, double lmda, double alpha, int numUser, 
	int numService, int dim);

/* Perform line search to find the best learning rate */
double linesearch(double **U, double **S, double **removedMatrix, double **locSim, 
	double lastLossValue, double **gradU, double **gradS, double etaInit, 
	double lmda, double alpha, int numUser, int numService, int dim);

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




