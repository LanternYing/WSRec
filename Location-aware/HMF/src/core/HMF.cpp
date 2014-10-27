/********************************************************
 * HMF.cpp
 * C++ implements on HMF [He et al., ICWS'2014]
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/20
********************************************************/

#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <utility>
#include <algorithm> 
#include <cmath>
#include "HMF.h"
using namespace std;


/********************************************************
 * predData is the output variable
********************************************************/
void HMF(double *removedData, double *userGroup1, double *wsGroup1,
	double *userGroup2, double *wsGroup2, int numUser, int numService, 
	int numUser1, int numService1, int numUser2, int numService2, int dim, 
	double lmda, double lmda1, double lmda2, double alpha, int maxIter, 
	double etaInit, double *predData)
{
	// --- transfer the 1D pointer to 2D array pointer
    double **removedMatrix = vector2Matrix(removedData, numUser, numService);
    double **predMatrix = vector2Matrix(predData, numUser, numService);

    // --- create a set of temporal matries
    double **U = createRandomMatrix(numUser, dim);
    double **S = createRandomMatrix(numService, dim);
    double **U1 = createRandomMatrix(numUser1, dim);
    double **S1 = createRandomMatrix(numService1, dim);
    double **U2 = createRandomMatrix(numUser2, dim);
    double **S2 = createRandomMatrix(numService2, dim);
    double **gradU = createZeroMatrix(numUser, dim);
    double **gradS = createZeroMatrix(numService, dim);
    double **gradU1 = createZeroMatrix(numUser1, dim);
    double **gradS1 = createZeroMatrix(numService1, dim);
    double **gradU2 = createZeroMatrix(numUser2, dim);
    double **gradS2 = createZeroMatrix(numService2, dim);
    double **localMatrix = createZeroMatrix(numUser, numService);  

    // --- iterate by standard gradient descent algorithm
    int iter, i, j, k;
    for (iter = 0; iter < maxIter; iter++) {
    	// update predicted values
    	getTrainMatrix(U1, S1, U2, S2, U, S, removedMatrix, localMatrix, predMatrix, userGroup1, 
			wsGroup1, userGroup2, wsGroup2, alpha, numUser1, numService1, numUser2, numService2, 
			numUser, numService, dim);

    	// compute loss values
		double lossValue1 = localLoss(U1, S1, removedMatrix, userGroup1, wsGroup1, lmda1, 
			numUser1, numService1, dim);
		double lossValue2 = localLoss(U2, S2, removedMatrix, userGroup2, wsGroup2, lmda2, 
			numUser2, numService2, dim); 
		double lossValue = globalLoss(U, S, removedMatrix, localMatrix, lmda, alpha, 
			numUser, numService, dim);
		// cout << "Local1: " << lossValue1 << "\tLocal2: " << lossValue2 <<"\tGlobal: " 
		// 	<< lossValue << endl;

    	// update gradients
    	gradLocalLoss(U1, S1, removedMatrix, localMatrix, gradU1, gradS1, userGroup1, 
    		wsGroup1, lmda1, numUser1, numService1, dim);
    	gradLocalLoss(U2, S2, removedMatrix, localMatrix, gradU2, gradS2, userGroup2, 
    		wsGroup2, lmda2, numUser2, numService2, dim);
    	gradGlobalLoss(U, S, removedMatrix, localMatrix, predMatrix, gradU, gradS,
    		lmda, alpha, numUser, numService, dim);

    	// line search to find the best learning rate eta
		double eta1 = lineSearchForLocal(U1, S1, removedMatrix, lossValue1, gradU1, 
			gradS1, etaInit, userGroup1, wsGroup1, lmda1, numUser1, numService1, dim);
		double eta2 = lineSearchForLocal(U2, S2, removedMatrix, lossValue2, gradU2, 
			gradS2, etaInit, userGroup2, wsGroup2, lmda2, numUser2, numService2, dim);
		double eta = lineSearchForGlobal(U, S, removedMatrix, lossValue, gradU, 
			gradS, etaInit, localMatrix, lmda, alpha, numUser, numService, dim);

		// gradient descent updates
		for (k = 0; k < dim; k++) {
			// update local U1, S1
			for (i = 0; i < numUser1; i++) {
				U1[i][k] -= eta1 * gradU1[i][k];
			}
			for (j = 0; j < numService1; j++) {
				S1[j][k] -= eta1 * gradS1[j][k];
			}

			// update local U2, S2
			for (i = 0; i < numUser2; i++) {
				U2[i][k] -= eta2 * gradU2[i][k];
			}
			for (j = 0; j < numService2; j++) {
				S2[j][k] -= eta2 * gradS2[j][k];
			}

			// update global U, S
			for (i = 0; i < numUser; i++) {
				U[i][k] -= eta * gradU[i][k];
			}
			for (j = 0; j < numService; j++) {
				S[j][k] -= eta * gradS[j][k];
			}
		}	
    }

    // make predictions
	getPredMatrix(U1, S1, U2, S2, U, S, removedMatrix, localMatrix, predMatrix, userGroup1, 
		wsGroup1, userGroup2, wsGroup2, alpha, numUser1, numService1, numUser2, numService2, 
		numUser, numService, dim);

    deleteMatrix(gradU);
    deleteMatrix(gradS);
    deleteMatrix(gradU1);
    deleteMatrix(gradS1);
    deleteMatrix(gradU2);
    deleteMatrix(gradS2);
    deleteMatrix(U);
    deleteMatrix(S);
    deleteMatrix(U1);
    deleteMatrix(S1);
    deleteMatrix(U2);
    deleteMatrix(S2);
    deleteMatrix(localMatrix);
    delete ((char*) removedMatrix);
    delete ((char*) predMatrix);
}


double globalLoss(double **U, double **S, double **removedMatrix, double **localMatrix, 
	double lmda, double alpha, int numUser, int numService, int dim)
{
	int i, j, k;
	double loss = 0;

	// sum of squared error
	for (i = 0; i < numUser; i++) {
		for (j = 0; j < numService; j++) {
			if (removedMatrix[i][j] != 0) {
				double predValue = dotProduct(U[i], S[j], dim);
				if (localMatrix[i][j] > 0) {
					predValue = alpha * predValue + (1 - alpha) * localMatrix[i][j];
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


double localLoss(double **U, double **S, double **removedMatrix, double *userGroup, 
	double *wsGroup, double lmda, int numUser, int numService, int dim)
{
	int i, j, k;
	double loss = 0;

	// sum of squared error
	for (i = 0; i < numUser; i++) {
		int idx = userGroup[i];
		for (j = 0; j < numService; j++) {
			int idy = wsGroup[j];
			if (removedMatrix[idx][idy] != 0) {
				double predValue = dotProduct(U[i], S[j], dim);
				loss += 0.5 * (removedMatrix[idx][idy] - predValue)
							* (removedMatrix[idx][idy] - predValue);
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


void gradGlobalLoss(double **U, double **S, double **removedMatrix, double **localMatrix, 
	double **predMatrix, double **gradU, double **gradS, double lmda, double alpha, 
	int numUser, int numService, int dim)
{
	int i, j, k, g;
	double grad = 0;

	// gradU
	for (i = 0; i < numUser; i++) {
		for (k = 0; k < dim; k++) {
			grad = 0;
			for (j = 0; j < numService; j++) {
				if (removedMatrix[i][j] != 0) {
					if (localMatrix[i][j] > 0) {
						grad += (removedMatrix[i][j] - predMatrix[i][j]) 
							* alpha * (-S[j][k]);
					}
					else {
						grad += (removedMatrix[i][j] - predMatrix[i][j]) 
							* (-S[j][k]);
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
					if (localMatrix[i][j] > 0) {
						grad += (removedMatrix[i][j] - predMatrix[i][j]) 
							* alpha * (-U[i][k]);
					}
					else {
						grad += (removedMatrix[i][j] - predMatrix[i][j]) 
							* (-U[i][k]);
					}
				} 
			}
			grad += lmda * S[j][k];
			gradS[j][k] = grad;			
		}
	}
}


void gradLocalLoss(double **U, double **S, double **removedMatrix, double **localMatrix, 
	double **gradU, double **gradS, double *userGroup, double *wsGroup, double lmda, 
	int numUser, int numService, int dim)
{
	int i, j, k, g;
	double grad = 0;

	// gradU
	for (i = 0; i < numUser; i++) {
		int idx = userGroup[i];
		for (k = 0; k < dim; k++) {
			grad = 0;
			for (j = 0; j < numService; j++) {
				int idy = wsGroup[j];
				if (removedMatrix[idx][idy] != 0) {
					grad += (removedMatrix[idx][idy] - localMatrix[idx][idy]) 
						* (-S[j][k]);
				} 
			}
			grad += lmda * U[i][k];
			gradU[i][k] = grad;
		}
	}

	// gradS
	for (j = 0; j < numService; j++) {
		int idy = wsGroup[j];
		for (k = 0; k < dim; k++) {
			grad = 0;
			for (i = 0; i < numUser; i++) {
				int idx = userGroup[i];
				if (removedMatrix[idx][idy] != 0) {
					grad += (removedMatrix[idx][idy] - localMatrix[idx][idy]) 
						* (-U[i][k]);
				} 
			}
			grad += lmda * S[j][k];
			gradS[j][k] = grad;			
		}
	}
}


void getTrainMatrix(double **U1, double **S1, double **U2, double **S2, double **U, 
	double **S, double ** removedMatrix, double **localMatrix, double **predMatrix, 
	double *userGroup1, double *wsGroup1, double *userGroup2, double *wsGroup2, 
	double alpha, int numUser1, int numService1, int numUser2, int numService2, 
	int numUser, int numService, int dim)
{
	int i, j;

	// update cluster 1
	for (i = 0; i < numUser1; i++) {
		int idx = userGroup1[i];
		for (j = 0; j < numService1; j++) {
			int idy = wsGroup1[j];
			if (removedMatrix[idx][idy] != 0) {
				localMatrix[idx][idy] = dotProduct(U1[i], S1[j], dim);
			}
		}
	}

	// update cluster 2
	for (i = 0; i < numUser2; i++) {
		int idx = userGroup2[i];
		for (j = 0; j < numService2; j++) {
			int idy = wsGroup2[j];
			if (removedMatrix[idx][idy] != 0) {
				localMatrix[idx][idy] = dotProduct(U2[i], S2[j], dim);
			}
		}
	}

	// update global matrix
	for (i = 0; i < numUser; i++) {
		for (j = 0; j < numService; j++) {
			if (removedMatrix[i][j] != 0) {
				double predValue = dotProduct(U[i], S[j], dim);
				if (localMatrix[i][j] > 0) {
					predValue = alpha * predValue + (1 - alpha) * localMatrix[i][j];
				}
				predMatrix[i][j] = predValue;
			}
		}
	}
}


double lineSearchForLocal(double **U, double **S, double **removedMatrix, 
	double lastLossValue, double **gradU, double **gradS, double etaInit, 
	double *userGroup, double *wsGroup, double lmda, int numUser, int numService, 
	int dim)
{
	double eta = etaInit;
	double lossValue;
	double **U1 = createZeroMatrix(numUser, dim);
	double **S1 = createZeroMatrix(numService, dim);

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

        lossValue = localLoss(U1, S1, removedMatrix, userGroup, wsGroup, lmda, 
        	numUser, numService, dim);

        if (lossValue <= lastLossValue)
            break;
        eta = eta / 2;
    }

    deleteMatrix(U1);
    deleteMatrix(S1);
    return eta;
}


double lineSearchForGlobal(double **U, double **S, double **removedMatrix, 
	double lastLossValue, double **gradU, double **gradS, double etaInit, 
	double **localMatrix, double lmda, double alpha, int numUser, int numService, 
	int dim)
{
	double eta = etaInit;
	double lossValue;
	double **U1 = createZeroMatrix(numUser, dim);
	double **S1 = createZeroMatrix(numService, dim);

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

        lossValue = globalLoss(U1, S1, removedMatrix, localMatrix, lmda, alpha, 
        	numUser, numService, dim);

        if (lossValue <= lastLossValue)
            break;
        eta = eta / 2;
    }

    deleteMatrix(U1);
    deleteMatrix(S1);
    return eta;
}


void getPredMatrix(double **U1, double **S1, double **U2, double **S2, double **U, 
	double **S, double ** removedMatrix, double **localMatrix, double **predMatrix, 
	double *userGroup1, double *wsGroup1, double *userGroup2, double *wsGroup2, 
	double alpha, int numUser1, int numService1, int numUser2, int numService2, 
	int numUser, int numService, int dim)
{
	int i, j;

	// update cluster 1
	for (i = 0; i < numUser1; i++) {
		int idx = userGroup1[i];
		for (j = 0; j < numService1; j++) {
			int idy = wsGroup1[j];
			if (removedMatrix[idx][idy] == 0) {
				localMatrix[idx][idy] = dotProduct(U1[i], S1[j], dim);
			}
		}
	}

	// update cluster 2
	for (i = 0; i < numUser2; i++) {
		int idx = userGroup2[i];
		for (j = 0; j < numService2; j++) {
			int idy = wsGroup2[j];
			if (removedMatrix[idx][idy] == 0) {
				localMatrix[idx][idy] = dotProduct(U2[i], S2[j], dim);
			}
		}
	}

	// update global matrix
	for (i = 0; i < numUser; i++) {
		for (j = 0; j < numService; j++) {
			if (removedMatrix[i][j] == 0) {
				double predValue = dotProduct(U[i], S[j], dim);
				if (localMatrix[i][j] > 0) {
					predValue = alpha * predValue + (1 - alpha) * localMatrix[i][j];
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


double **vector2Matrix(double *vector, int row, int col)  
{
	double **matrix = new double *[row];
	if (!matrix) {
		cout << "Memory allocation failed in vector2Matrix." << endl;
		return NULL;
	}

	int i;
	for (i = 0; i < row; i++) {
		matrix[i] = vector + i * col;  
	}
	return matrix;
}


double **createZeroMatrix(int row, int col) 
{
    double **matrix = new double *[row];
    matrix[0] = new double[row * col];
    // Initialization
    memset(matrix[0], 0, row * col * sizeof(double)); 
    int i;
    for (i = 1; i < row; i++) {
    	matrix[i] = matrix[i - 1] + col;
    }
    return matrix;
}


double **createRandomMatrix(int row, int col) 
{
    double **matrix = new double *[row];
    matrix[0] = new double[row * col];
    int i, j;
    for (i = 0; i < row; i++) {
    	if (i > 0) {
    		matrix[i] = matrix[i - 1] + col;
		}
    	// Initialization
    	for (j = 0; j < col; j++) {
    		matrix[i][j] = (double) rand()/RAND_MAX;
    	}
    }
    return matrix;
}


void deleteMatrix(double **ptr) 
{
	delete ptr[0];
	delete ptr;
}





