/********************************************************
 * EMF.h: header file of EMF.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/14
********************************************************/

#include "UIPCC.h"


/* Perform the core approach of PMF */
void EMF(double *removedData, int numUser, int numService, int dim, 
	double lmda, double alpha, int maxIter, double etaInit, int topK_U, int topK_S, 
	double *Udata, double *Sdata);

/* Compute the loss value of PMF */
double loss(double **U, double **S, double **removedMatrix, double lmda, 
	double alpha, int numUser, int numService, int dim, 
	vector<vector<pair<int, double> > > &pccUserMatrix, 
	vector<vector<pair<int, double> > > &pccServiceMatrix);

/* Compute the gradients of the loss function */
void gradLoss(double **U, double **S, double **removedMatrix, double **gradU, 
	double **gradS, double lmda, double alpha, int numUser, int numService, int dim, 
	vector<vector<pair<int, double> > > &pccUserMatrix, 
	vector<vector<pair<int, double> > > &pccServiceMatrix);

/* Perform line search to find the best learning rate */
double linesearch(double **U, double **S, double **removedMatrix, 
	double lastLossValue, double **gradU, double **gradS, double etaInit, 
	double lmda, double alpha, int numUser, int numService, int dim, 
	vector<vector<pair<int, double> > > &pccUserMatrix, 
	vector<vector<pair<int, double> > > &pccServiceMatrix);

/* Compute predMatrix */
void U_dot_S(double **removedMatrix, double **U, double **S, int numUser, 
		int numService, int dim, double **predMatrix);

/* Compute the dot product of two vectors */
double dotProduct(double *vec1, double *vec2, int len);

/* Transpose a matrix */
void transpose(double **matrix, double **matrix_T, int row, int col);

