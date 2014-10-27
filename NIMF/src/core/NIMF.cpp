/********************************************************
 * NIMF.cpp
 * C++ implements on NIMF [Zheng et al., TSC'13]
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/18
********************************************************/

#include <iostream>
#include <cstring>
#include <cmath>
#include "NIMF.h"
using namespace std;


/********************************************************
 * Udata and Sdata are the output values
********************************************************/
void NIMF(double *removedData, int numUser, int numService, int dim, 
	double lmda, double alpha, int maxIter, double etaInit, int topK,
	double *Udata, double *Sdata, double *predData)
{	
	// --- transfer the 1D pointer to 2D array pointer
    double **removedMatrix = vector2Matrix(removedData, numUser, numService);
    double **predMatrix = vector2Matrix(predData, numUser, numService);
    double **U = vector2Matrix(Udata, numUser, dim);
    double **S = vector2Matrix(Sdata, numService, dim);
    
    // --- create a set of temporal matries
    double **gradU = createMatrix(numUser, dim);
    double **gradS = createMatrix(numService, dim);

    // --- compute the NORMALIZED similarity matrix
    vector<vector<pair<int, double> > > topkSimUserDicRecord;
    topkSimUserDicRecord.resize(numUser);
    vector<vector<pair<int, double> > > topkSimUserDic = getPCCMatrix(
    	removedMatrix, numUser, numService, topK, topkSimUserDicRecord);

	// --- inital loss value 
	double lossValue = loss(U, S, removedMatrix, lmda, alpha, numUser, numService,
		dim, topkSimUserDic);

    // --- iterate by standard gradient descent algorithm
    int iter, i, j, k;
    for (iter = 0; iter < maxIter; iter++) {
    	// update gradients
    	gradLoss(U, S, removedMatrix, gradU, gradS, lmda, alpha, numUser, numService, 
    		dim, topkSimUserDic, topkSimUserDicRecord);

    	// line search to find the best learning rate eta
		double eta = linesearch(U, S, removedMatrix, lossValue, gradU, gradS, 
			etaInit, lmda, alpha, numUser, numService, dim, topkSimUserDic);

		// gradient descent updates
		for (k = 0; k < dim; k++) {
			// update U
			for (i = 0; i < numUser; i++) {
				U[i][k] -= eta * gradU[i][k];
			}
			// update S
			for (j = 0; j < numService; j++) {
				S[j][k] -= eta * gradS[j][k];
			}
		}

		// update loss value
		lossValue = loss(U, S, removedMatrix, lmda, alpha, numUser, numService, 
			dim, topkSimUserDic);
		// cout << lossValue << endl;
    }

    // make predictions
	getPredMatrix(removedMatrix, U, S, numUser, numService, dim, alpha, 
		topkSimUserDic, predMatrix);

    deleteMatrix(gradU);
    deleteMatrix(gradS);
    delete ((char*) U);
    delete ((char*) S);
    delete ((char*) removedMatrix);
    delete ((char*) predMatrix);
}


double loss(double **U, double **S, double **removedMatrix, double lmda, 
	double alpha, int numUser, int numService, int dim, 
	vector<vector<pair<int, double> > > &topkSimUserDic)
{
	int i, j, k, g;
	double loss = 0;

	// sum of squared error
	for (i = 0; i < numUser; i++) {
		for (j = 0; j < numService; j++) {
			if (removedMatrix[i][j] != 0) {
				double predValue = alpha * dotProduct(U[i], S[j], dim);
				for (g = 0; g < topkSimUserDic[i].size(); g++) {
					int uid = topkSimUserDic[i][g].first;
					double sim = topkSimUserDic[i][g].second;
					predValue += (1 - alpha) * sim 
						* dotProduct(U[uid], S[j], dim);
				}
				loss += 0.5 * (removedMatrix[i][j] - predValue)
							* (removedMatrix[i][j] - predValue);	
			}
		}
	}

	// L2 regularization
	for (k = 0; k < dim; k++) {
		for (i = 0; i < numUser; i++) {
			loss += 0.5 * lmda * U[i][k] * U[i][k];
		}
		for (j = 0; j < numService; j++) {
			loss += 0.5 * lmda * S[j][k] * S[j][k];
		}
	}

	return loss;
}


void gradLoss(double **U, double **S, double **removedMatrix, double **gradU, 
	double **gradS, double lmda, double alpha, int numUser, int numService, int dim, 
	vector<vector<pair<int, double> > > &topkSimUserDic, 
	vector<vector<pair<int, double> > >	&topkSimUserDicRecord)
{
	int i, j, k, g, t;
	double grad = 0;

	// gradU
	for (i = 0; i < numUser; i++) {
		for (k = 0; k < dim; k++) {
			grad = 0;
			for (j = 0; j < numService; j++) {
				if (removedMatrix[i][j] != 0) {
					double predValue = alpha * dotProduct(U[i], S[j], dim);
					for (g = 0; g < topkSimUserDic[i].size(); g++) {
						int uid = topkSimUserDic[i][g].first;
						double sim = topkSimUserDic[i][g].second;
						predValue += (1 - alpha) * sim 
							* dotProduct(U[uid], S[j], dim);
					}
					grad += (removedMatrix[i][j] - predValue) * (-S[j][k]);
				} 
			}
			grad *= alpha;

			for (t = 0; t < topkSimUserDicRecord[i].size(); t++) {
				int p = topkSimUserDicRecord[i][t].first;
				for (j = 0; j < numService; j++) {
					if (removedMatrix[p][j] != 0) {
						double sum = alpha * dotProduct(U[p], S[j], dim);
						for (g = 0; g < topkSimUserDic[p].size(); g++) {
							int uid = topkSimUserDic[p][g].first;
							double sim = topkSimUserDic[p][g].second;
							sum += (1 - alpha) * sim 
								* dotProduct(U[uid], S[j], dim);
						}
						grad += (1 - alpha) * topkSimUserDicRecord[i][t].second 
							* S[j][k] * (sum - removedMatrix[p][j]); 
					}
				}
			}

			grad += lmda * U[i][k];
			gradU[i][k] = grad;
		}
	}

	// gradS
	for (j = 0; j < numService; j++) {
		for (k = 0; k < dim; k++) {
			grad = 0;
			for (i = 0; i < numUser; i++) {
				if (removedMatrix[i][j] != 0) {
					double predValue = alpha * dotProduct(U[i], S[j], dim);
					double right = alpha * U[i][k];
					for (g = 0; g < topkSimUserDic[i].size(); g++) {
						int uid = topkSimUserDic[i][g].first;
						double sim = topkSimUserDic[i][g].second;
						predValue += (1 - alpha) * sim 
							* dotProduct(U[uid], S[j], dim);
						right += (1 - alpha) * sim * U[uid][k];
					}
					grad += (removedMatrix[i][j] - predValue) * (-right);						
				}
			}
			grad += lmda * S[j][k];
			gradS[j][k] = grad;			
		}
	}
}


double linesearch(double **U, double **S, double **removedMatrix, 
	double lastLossValue, double **gradU, double **gradS, double etaInit, 
	double lmda, double alpha, int numUser, int numService, int dim, 
	vector<vector<pair<int, double> > > &topkSimUserDic)
{
	double eta = etaInit;
	double lossValue;
	double **U1 = createMatrix(numUser, dim);
	double **S1 = createMatrix(numService, dim);

	int iter, i, j, k;
	for (iter = 0; iter < 20; iter++) {
    	// update U1, S1
		for (k = 0; k < dim; k++) {
			// update U
			for (i = 0; i < numUser; i++) {
				U1[i][k] = U[i][k] - eta * gradU[i][k];
			}
			// update S
			for (j = 0; j < numService; j++) {
				S1[j][k] = S[j][k] - eta * gradS[j][k];
			}
		}    

        lossValue = loss(U1, S1, removedMatrix, lmda, alpha,
        	numUser, numService, dim, topkSimUserDic);

        if (lossValue <= lastLossValue)
            break;
        eta = eta / 2;
    }

    deleteMatrix(U1);
    deleteMatrix(S1);
    return eta;
}


void getPredMatrix(double **removedMatrix, double **U, double **S, int numUser, 
		int numService, int dim, double alpha, 
		vector<vector<pair<int, double> > > &topkSimUserDic, double **predMatrix)
{
	int i, j, g;
	for (i = 0; i < numUser; i++) {
		for (j = 0; j < numService; j++) {
			if (removedMatrix[i][j] == 0) {
				double predValue = 0;
				predValue += alpha * dotProduct(U[i], S[j], dim);
				for (g = 0; g < topkSimUserDic[i].size(); g++) {
					int uid = topkSimUserDic[i][g].first;
					double sim = topkSimUserDic[i][g].second;
					predValue += (1 - alpha) * sim * dotProduct(U[uid], S[j], dim);
				}
				predMatrix[i][j] = predValue;  
			}
		}
	}
}


double dotProduct(double *vec1, double *vec2, int len)  
{
	double product = 0;
	int i;
	for (i = 0; i < len; i++) {
		product += vec1[i] * vec2[i];
	}
	return product;
}

