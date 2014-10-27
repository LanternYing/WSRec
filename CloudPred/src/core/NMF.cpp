/********************************************************
 * NMF.cpp
 * C++ implements on NMF
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/7
********************************************************/

#include <iostream>
#include <cstring>
#include <cmath>
#include <limits> 
#include "NMF.h"
using namespace std;

#define EPS numeric_limits<double>::epsilon()


void NMF_core(double *removedData, int numUser, int numService, int dim, 
	double lmda, int maxIter, double *Udata, double *Sdata)
{	
	// --- transfer the 1D pointer to 2D array pointer
    double **removedMatrix = vector2Matrix(removedData, numUser, numService);
    double **U = vector2Matrix(Udata, numUser, dim);
    double **S = vector2Matrix(Sdata, numService, dim);
    
    // --- create a set of temporal matries
    double **predMatrix = createMatrix(numUser, numService);

	// --- update predMatrix
	U_dot_S(removedMatrix, U, S, numUser, numService, dim, predMatrix);

    // --- iterate by standard NMF algorithm
    int iter, i, j, k; 
    for (iter = 0; iter < maxIter; iter++) {

    	// update U
    	for (i = 0; i < numUser; i++) {
    		for (k = 0; k < dim; k++) {
    			double up = 0;
    			double down = 0;
    			for (j = 0; j < numService; j++) {
    				if (removedMatrix != 0) {
	    				up += removedMatrix[i][j] * S[j][k];
	    				down += predMatrix[i][j] * S[j][k];
    				}
    			}
    			down += U[i][k] * lmda;
    			if (down == 0) down += EPS;		
	    		U[i][k] = U[i][k] * up / down;
	    	}
    	}
		
		// update predMatrix
		U_dot_S(removedMatrix, U, S, numUser, numService, dim, predMatrix);

    	// update S
    	for (j = 0; j < numService; j++) {
    		for (k = 0; k < dim; k++) {
    			double up = 0;
    			double down = 0;
    			for (i = 0; i < numUser; i++) {
    				if (removedMatrix[i][j] != 0) {
	    				up += removedMatrix[i][j] * U[i][k];
	    				down += predMatrix[i][j] * U[i][k];
    				}
				}
				down += S[j][k] * lmda;
				if (down == 0) down += EPS;
				S[j][k] = S[j][k] * up / down;
	    	}
    	}

    	// update predMatrix
    	U_dot_S(removedMatrix, U, S, numUser, numService, dim, predMatrix);

    	// Uncomment this line for testing
    	// cout << "Loss: " << loss(U, S, removedMatrix, predMatrix, lmda, numUser, numService, dim) << endl;
    }

    delete2DMatrix(predMatrix);
    delete ((char*) U);
    delete ((char*) S);
    delete ((char*) removedMatrix);
}


double loss(double **U, double **S, double **removedMatrix, double **predMatrix, 
	double lmda, int numUser, int numService, int dim)
{
	int i, j, k;
	double loss = 0;

	// cost
	for (i = 0; i < numUser; i++) {
		for (j = 0; j < numService; j++) {
			if (removedMatrix[i][j] != 0) {
				loss += 0.5 * (removedMatrix[i][j] - predMatrix[i][j])
							* (removedMatrix[i][j] - predMatrix[i][j]);	
			}
		}
	}

	// regularization
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


void transpose(double **matrix, double **matrix_T, int row, int col) 
{
	int i, j;
	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			matrix_T[j][i] = matrix[i][j];
		}
	}
}



