/********************************************************
 * NIMF.h: header file of NIMF.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/18
********************************************************/

#include "UIPCC.h"


/* Perform the core approach of NIMF */
void NIMF(double *removedData, int numUser, int numService, int dim, 
	double lmda, double alpha, int maxIter, double etaInit, int topK,
	double *Udata, double *Sdata, double *predData);

/* Compute the loss value */
double loss(double **U, double **S, double **removedMatrix, double lmda, 
	double alpha, int numUser, int numService, int dim, 
	vector<vector<pair<int, double> > > &topkSimUserDic);

/* Compute the gradients of the loss function */
void gradLoss(double **U, double **S, double **removedMatrix, double **gradU, 
	double **gradS, double lmda, double alpha, int numUser, int numService, int dim, 
	vector<vector<pair<int, double> > > &topkSimUserDic, 
	vector<vector<pair<int, double> > >	&topkSimUserDicRecord);

/* Perform line search to find the best learning rate */
double linesearch(double **U, double **S, double **removedMatrix, 
	double lastLossValue, double **gradU, double **gradS, double etaInit, 
	double lmda, double alpha, int numUser, int numService, int dim, 
	vector<vector<pair<int, double> > > &topkSimUserDic);

/* Compute the predicted values for missing entries */
void getPredMatrix(double **removedMatrix, double **U, double **S, int numUser, 
		int numService, int dim, double alpha, 
		vector<vector<pair<int, double> > > &topkSimUserDic, double **predMatrix);

/* Compute the dot product of two vectors */
double dotProduct(double *vec1, double *vec2, int len);

