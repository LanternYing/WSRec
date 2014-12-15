/********************************************************
 * LN_LFM.cpp
 * C++ implements on LN_LFM
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/12/12
********************************************************/

#include <iostream>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <vector>
#include "LN_LFM.h"
using namespace std;

#define eps 1e-10


/********************************************************
 * predData are the output values
 *******************************************************/
void LN_LFM(double *removedData, double *predData, double *wsInfoData, int numUser, 
	int numService, int numProvider, int numCountry, int dim, double lambda, int maxIter, 
    double eta, double alpha, double *Udata, double *Sdata, double *Pdata, double *Cdata, 
    double *bu, double *bs, double *bp, double *bc, bool debugMode)
{	
	// --- transfer the 1D pointer to 2D array pointer
    double **removedMatrix = vector2Matrix(removedData, numUser, numService);
    double **predMatrix = vector2Matrix(predData, numUser, numService);
    double **wsInfoList = vector2Matrix(wsInfoData, numService, 2);
    double **U = vector2Matrix(Udata, numUser, dim);
    double **S = vector2Matrix(Sdata, numService, dim);
    double **P = vector2Matrix(Pdata, numProvider, dim);
    double **C = vector2Matrix(Cdata, numCountry, dim);

    // --- gloabl average
	int cnt = 0;
	double miu = 0;
    vector<int> userIdx, wsIdx, dataSequence; 
    for (int i = 0; i < numUser; i++) {
        for (int j = 0; j < numService; j++) {
            if (fabs(removedMatrix[i][j]) > eps) {
                userIdx.push_back(i);
                wsIdx.push_back(j);
                miu += removedMatrix[i][j];
                dataSequence.push_back(cnt);
                cnt++; 
            }
        }
    }
    miu /= cnt;

    // --- iterate by standard gradient descent algorithm
    int iter, i, j, k, l, g, idx;
    double lossValue;
    for (iter = 1; iter <= maxIter; iter++) {
        random_shuffle(dataSequence.begin(), dataSequence.end());
        for (idx = 0; idx < cnt; idx++) {
            double pred, error;
            int id; 
            id = dataSequence[idx];
            i = userIdx[id];
            j = wsIdx[id];
            k = wsInfoList[j][0];
            l = wsInfoList[j][1];
            pred = miu + bu[i] + bs[j] + bp[k] + bc[l] 
                + alpha * dotProduct(U[i], S[j], dim) 
                + (1 - alpha) * dotProduct(U[i], P[k], dim)
                + (1 - alpha) * dotProduct(U[i], C[l], dim);
            error = pred - removedMatrix[i][j];

            // update
            bc[l] -= eta * (error + lambda * bc[l]);
            bp[k] -= eta * (error + lambda * bp[k]);
            bs[j] -= eta * (error + lambda * bs[j]);
            bu[i] -= eta * (error + lambda * bu[i]);
            for (g = 0; g < dim; g++) {
                C[l][g] -= eta * (error * (1 - alpha) * U[i][g] + lambda * C[l][g]);
                P[k][g] -= eta * (error * (1 - alpha) * U[i][g] + lambda * P[k][g]);
                S[j][g] -= eta * (error * alpha * U[i][g] + lambda * S[j][g]);
                U[i][g] -= eta * (error * (alpha * S[j][g] + (1 - alpha) * (P[k][g] + C[l][g])) 
                    + lambda * U[i][g]);
            }

            // log the debug info
            cout.setf(ios::fixed);
            if (debugMode && idx % 10000 == 0) {
                lossValue = loss(U, S, P, C, bu, bs, bp, bc, miu, removedMatrix, wsInfoList, 
                    lambda, alpha, numUser, numService, numProvider, numCountry, dim);
                cout << currentDateTime() << ": ";
                cout << "iter = " << iter << ", samples = " << idx 
                    << ", lossValue = " << lossValue << endl;
            } 
        }
    }  

    // --- make predictions
    updatePredMatrix(predMatrix, U, S, P, C, bu, bs, bp, bc, miu, wsInfoList, 
        alpha, numUser, numService, dim);

    delete ((char*) U);
    delete ((char*) S);
    delete ((char*) P);
    delete ((char*) C);
    delete ((char*) wsInfoList);
    delete ((char*) removedMatrix);
    delete ((char*) predMatrix);
}


inline double sqr(double x) {return x * x;}

double loss(double **U, double **S, double **P, double **C, double *bu, double *bs, 
    double *bp, double *bc, double miu, double **removedMatrix, double **wsInfoList,
    double lambda, double alpha, int numUser, int numService, int numProvider, 
    int numCountry, int dim)
{
	int i, j, k, l, g;
	double loss = 0;

	// cost
	for (i = 0; i < numUser; i++) {
		for (j = 0; j < numService; j++) {
			if (removedMatrix[i][j] > 0) {
                double pred;
                k = wsInfoList[j][0];
                l = wsInfoList[j][1];
                pred = miu + bu[i] + bs[j] + bp[k] + bc[l] 
                    + alpha * dotProduct(U[i], S[j], dim) 
                    + (1 - alpha) * dotProduct(U[i], P[k], dim)
                    + (1 - alpha) * dotProduct(U[i], C[l], dim);
                loss += 0.5 * sqr(removedMatrix[i][j] - pred);
			}
		}
	}

	// L2 regularization
	for (g = 0; g < dim; g++) {
		for (i = 0; i < numUser; i++) {
			loss += 0.5 * lambda * sqr(U[i][g]);
		}
		for (j = 0; j < numService; j++) {
			loss += 0.5 * lambda * sqr(S[j][g]);
		}
        for (k = 0; k < numProvider; k++) {
            loss += 0.5 * lambda * sqr(P[k][g]);
        }
        for (l = 0; l < numCountry; l++) {
            loss += 0.5 * lambda * sqr(C[l][g]);
        }
	}    
    for (i = 0; i < numUser; i++) {
        loss += 0.5 * lambda * sqr(bu[i]);
    }
    for (j = 0; j < numService; j++) {
        loss += 0.5 * lambda * sqr(bs[j]);
    }
    for (k = 0; k < numProvider; k++) {
        loss += 0.5 * lambda * sqr(bp[k]);
    }
    for (l = 0; l < numCountry; l++) {
        loss += 0.5 * lambda * sqr(bc[l]);
    }
    
	return loss;
}


void updatePredMatrix(double **predMatrix, double **U, double **S, double **P, double **C, 
    double *bu, double *bs, double *bp, double *bc, double miu, double **wsInfoList, 
    double alpha, int numUser, int numService, int dim) 
{
	int i, j, k, l;
    for (i = 0; i < numUser; i++) {
        for (j = 0; j < numService; j++) {
            k = wsInfoList[j][0];
            l = wsInfoList[j][1];
            predMatrix[i][j] = miu + bu[i] + bs[j] + bp[k] + bc[l] 
                + alpha * dotProduct(U[i], S[j], dim) 
                + (1 - alpha) * dotProduct(U[i], P[k], dim)
                + (1 - alpha) * dotProduct(U[i], C[l], dim);
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


const string currentDateTime() 
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}

