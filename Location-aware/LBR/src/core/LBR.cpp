/********************************************************
 * LBR.cpp
 * C++ implements on LBR [Lo et al, ICWS'2012]
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/10
********************************************************/

#include <iostream>
#include <cstring>
#include <cmath>
#include "LBR.h"
using namespace std;


void getLocSim_core(double *geoData, double *locSimData, int numLine, double theta)
{
	// --- transfer the 1D pointer to 2D array pointer
    double **geoAxis = vector2Matrix(geoData, numLine, 2);
    double **locSim = vector2Matrix(locSimData, numLine, numLine);

    // --- compute the location similarity
    int i, j;
    for (i = 0; i < numLine; i++) {
    	for (j = 0; j < numLine; j++) {
    		if (j > i) {

    			double axisX = geoAxis[i][0] - geoAxis[j][0];
				double axisY = geoAxis[i][1] - geoAxis[j][1];
				// the parameter 111261 is as defalt in the ref paper
				double dist = 111261 * sqrt(axisX * axisX + axisY * axisY);
				if (dist < theta) {
					double sim = 1 - 1.0 / (1 + exp(-dist));
					if (sim > 0) {
						locSim[i][j] = sim;
					}
				}
    		}
    		else {
    			locSim[i][j] = locSim[j][i];
    		}
    	}
    }

    delete ((char*) geoAxis);
    delete ((char*) locSim);
}


/********************************************************
* Udata and Sdata are the output values
********************************************************/
void LBR(double *removedData, double *locSimData, int numUser, int numService, int dim, 
	double lmda, double alpha, int maxIter, double etaInit, double *Udata, double *Sdata)
{	
	// --- transfer the 1D pointer to 2D array pointer
    double **removedMatrix = vector2Matrix(removedData, numUser, numService);
    double **locSim = vector2Matrix(locSimData, numUser, numUser);
    double **U = vector2Matrix(Udata, numUser, dim);
    double **S = vector2Matrix(Sdata, numService, dim);
    
    // --- create a set of temporal matries
    double **gradU = createMatrix(numUser, dim);
    double **gradS = createMatrix(numService, dim);

	// --- inital loss value 
	double lossValue = loss(U, S, removedMatrix, locSim, lmda, alpha, numUser, numService, dim);

    // --- iterate by standard gradient descent algorithm
    int iter, i, j, k; 
    for (iter = 0; iter < maxIter; iter++) {
    	// update gradients
    	gradLoss(U, S, removedMatrix, gradU, gradS, locSim, lmda, alpha, 
    		numUser, numService, dim);

    	// line search to find the best learning rate eta
		double eta = linesearch(U, S, removedMatrix, locSim, lossValue, gradU, gradS, 
			etaInit, lmda, alpha, numUser, numService, dim);

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
		lossValue = loss(U, S, removedMatrix, locSim, lmda, alpha, numUser, numService, dim);
		//cout << lossValue << endl;
    }

    delete2DMatrix(gradU);
    delete2DMatrix(gradS);
    delete ((char*) U);
    delete ((char*) S);
    delete ((char*) removedMatrix);
}


double loss(double **U, double **S, double **removedMatrix, double **locSim,
	double lmda, double alpha, int numUser, int numService, int dim)
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

	// Location regularization
	for (i = 0; i < numUser; i++) {
		for (g = 0; g < numUser; g++) {
			if (locSim[i][g] > 0) {
				for (k = 0; k < dim; k++) {
					double tmp = (U[i][k] - U[g][k]);
					loss += 0.5 * alpha * locSim[i][g] * tmp * tmp;
				}
			}
		}
	}

    delete2DMatrix(predMatrix);
	return loss;
}


void gradLoss(double **U, double **S, double **removedMatrix, double **gradU, 
	double **gradS, double **locSim, double lmda, double alpha, int numUser, 
	int numService, int dim)
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
			for (g = 0; g < numUser; g++) {
				if (locSim[i][g] > 0) {
					grad += alpha * locSim[i][g] * (U[i][k] - U[g][k]);
				}
			}
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
			gradS[j][k] = grad;			
		}
	}

}


double linesearch(double **U, double **S, double **removedMatrix, double **locSim, 
	double lastLossValue, double **gradU, double **gradS, double etaInit, 
	double lmda, double alpha, int numUser, int numService, int dim)
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

        lossValue = loss(U1, S1, removedMatrix, locSim, lmda, alpha, 
        	numUser, numService, dim);

        if (lossValue <= lastLossValue)
            break;
        eta = eta / 2;
    }

    delete2DMatrix(U1);
    delete2DMatrix(S1);
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


double dotProduct(double *vec1, double *vec2, int len)  
{
	double product = 0;
	int i;
	for (i = 0; i < len; i++) {
		product += vec1[i] * vec2[i];
	}
	return product;
}


double **createMatrix(int row, int col) 
{
    double **matrix = new double *[row];
    matrix[0] = new double[row * col];
    memset(matrix[0], 0, row * col * sizeof(double)); // Initialization
    int i;
    for (i = 1; i < row; i++) {
    	matrix[i] = matrix[i - 1] + col;
    }
    return matrix;
}


void delete2DMatrix(double **ptr) {
	delete ptr[0];
	delete ptr;
}




