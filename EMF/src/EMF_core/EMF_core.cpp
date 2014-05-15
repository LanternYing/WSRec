/********************************************************
 * EMF_core.cpp
 * C++ implements on EMF [Lo et al., SCC'12]
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/14
********************************************************/

#include <iostream>
#include <cstring>
#include <cmath>
#include "EMF_core.h"
using namespace std;


/********************************************************
 * Udata and Sdata are the output values
********************************************************/
void EMF(double *removedData, int numUser, int numService, int dim, 
	double lmda, double alpha, int maxIter, double etaInit, int topK_U, int topK_S, 
	double *Udata, double *Sdata)
{	
	// --- transfer the 1D pointer to 2D array pointer
    double **removedMatrix = vector2Matrix(removedData, numUser, numService);
    double **U = vector2Matrix(Udata, numUser, dim);
    double **S = vector2Matrix(Sdata, numService, dim);
    
    // --- create a set of temporal matries
    double **gradU = createMatrix(numUser, dim);
    double **gradS = createMatrix(numService, dim);
    double **removedMatrix_T = createMatrix(numService, numUser);

    // --- compute the similarity matrix
    vector<vector<pair<int, double> > > pccUserMatrix = getPCCMatrix(
    	removedMatrix, numUser, numService, topK_U);
    transpose(removedMatrix, removedMatrix_T, numUser, numService);
    vector<vector<pair<int, double> > > pccServiceMatrix = getPCCMatrix(
    	removedMatrix_T, numService, numUser, topK_S);

	// --- inital loss value 
	double lossValue = loss(U, S, removedMatrix, lmda, alpha, numUser, numService,
		dim, pccUserMatrix, pccServiceMatrix);

    // --- iterate by standard gradient descent algorithm
    int iter, i, j, k;
    for (iter = 0; iter < maxIter; iter++) {
    	// update gradients
    	gradLoss(U, S, removedMatrix, gradU, gradS, lmda, alpha, numUser, numService, 
    		dim, pccUserMatrix, pccServiceMatrix);

    	// line search to find the best learning rate eta
		double eta = linesearch(U, S, removedMatrix, lossValue, gradU, gradS, 
			etaInit, lmda, alpha, numUser, numService, dim, pccUserMatrix, 
			pccServiceMatrix);

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
			dim, pccUserMatrix, pccServiceMatrix);
		// cout << lossValue << endl;
    }

    deleteMatrix(gradU);
    deleteMatrix(gradS);
    deleteMatrix(removedMatrix_T);
    delete ((char*) U);
    delete ((char*) S);
    delete ((char*) removedMatrix);
}


double loss(double **U, double **S, double **removedMatrix, double lmda, 
	double alpha, int numUser, int numService, int dim, 
	vector<vector<pair<int, double> > > &pccUserMatrix, 
	vector<vector<pair<int, double> > > &pccServiceMatrix)
{
	int i, j, k, g;
	double loss = 0;
	double **predMatrix = createMatrix(numUser, numService);

	// update predMatrix
	U_dot_S(removedMatrix, U, S, numUser, numService, dim, predMatrix);

	// cost
	for (i = 0; i < numUser; i++) {
		for (j = 0; j < numService; j++) {
			if (removedMatrix[i][j] != 0) {
				loss += 0.5 * (removedMatrix[i][j] - predMatrix[i][j])
							* (removedMatrix[i][j] - predMatrix[i][j]);	
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

	// User regularization
	for (i = 0; i < numUser; i++) {
		for (k = 0; k < dim; k++) {
			double sum = 0;
			for (g = 0; g < pccUserMatrix[i].size(); g++) {
				int id = pccUserMatrix[i][g].first;
				sum += pccUserMatrix[i][g].second * U[id][k];					
			}
			double tmp = U[i][k] - sum;
			loss += 0.5 * alpha * tmp * tmp;
		}
	}

	// Service regularization
	for (j = 0; j < numService; j++) {
		for (k = 0; k < dim; k++) {
			double sum = 0;
			for (g = 0; g < pccServiceMatrix[j].size(); g++) {
				int id = pccServiceMatrix[j][g].first;
				sum += pccServiceMatrix[j][g].second * S[id][k];					
			}
			double tmp = S[j][k] - sum;
			loss += 0.5 * alpha * tmp * tmp;
		}
	}

    deleteMatrix(predMatrix);
	return loss;
}


void gradLoss(double **U, double **S, double **removedMatrix, double **gradU, 
	double **gradS, double lmda, double alpha, int numUser, int numService, int dim, 
	vector<vector<pair<int, double> > > &pccUserMatrix, 
	vector<vector<pair<int, double> > > &pccServiceMatrix)
{
	int i, j, k, g;
	double grad = 0;

	// gradU
	for (i = 0; i < numUser; i++) {
		for (k = 0; k < dim; k++) {
			grad = 0;
			for (j = 0; j < numService; j++) {
				if (removedMatrix[i][j] != 0) {
					grad += (removedMatrix[i][j] - dotProduct(U[i], S[j], dim)) 
						* (-S[j][k]);
				} 
			}
			grad += lmda * U[i][k];

			double sum = 0;
			for (g = 0; g < pccUserMatrix[i].size(); g++) {
				int id = pccUserMatrix[i][g].first;
				sum += pccUserMatrix[i][g].second * U[id][k];					
			}
			grad += alpha * (U[i][k] - sum);

			gradU[i][k] = grad;
		}
	}

	// gradS
	for (j = 0; j < numService; j++) {
		for (k = 0; k < dim; k++) {
			grad = 0;
			for (i = 0; i < numUser; i++) {
				if (removedMatrix[i][j] != 0) {
					grad += (removedMatrix[i][j] - dotProduct(U[i], S[j], dim)) 
						* (-U[i][k]);
				} 
			}
			grad += lmda * S[j][k];

			double sum = 0;
			for (g = 0; g < pccServiceMatrix[j].size(); g++) {
				int id = pccServiceMatrix[j][g].first;
				sum += pccServiceMatrix[j][g].second * S[id][k];					
			}
			grad += alpha * (S[j][k] - sum);

			gradS[j][k] = grad;			
		}
	}

}


double linesearch(double **U, double **S, double **removedMatrix, 
	double lastLossValue, double **gradU, double **gradS, double etaInit, 
	double lmda, double alpha, int numUser, int numService, int dim, 
	vector<vector<pair<int, double> > > &pccUserMatrix, 
	vector<vector<pair<int, double> > > &pccServiceMatrix)
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
        	numUser, numService, dim, pccUserMatrix, pccServiceMatrix);

        if (lossValue <= lastLossValue)
            break;
        eta = eta / 2;
    }

    deleteMatrix(U1);
    deleteMatrix(S1);
    return eta;
}


void U_dot_S(double **removedMatrix, double **U, double **S, int numUser, 
		int numService, int dim, double **predMatrix)
{
	int i, j;
	for (i = 0; i < numUser; i++) {
		for (j = 0; j < numService; j++) {
			if (removedMatrix[i][j] != 0) {
				predMatrix[i][j] = dotProduct(U[i], S[j], dim);  
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


void transpose(double **matrix, double **matrix_T, int row, int col) 
{
	int i, j;
	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			matrix_T[j][i] = matrix[i][j];
		}
	}
}




