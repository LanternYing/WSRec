/********************************************************
 * NTF_core.cpp
 * C++ implements on NTF
 * Author: Yuwen Xiong, Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/7/15
********************************************************/

#include <iostream>
#include <cstring>
#include <cmath>
#include <iomanip>
#include "NTF_core.h"
#include <vector>
#include <ctime>
#include <algorithm>

using namespace std;


/// note that predData is the output of this function
void NTF_core(double *removedData, double *predData, int numUser, int numService, 
	int numTimeSlice, int dim, double lmda, int maxIter, bool debugMode, 
	double *Udata, double *Sdata, double *Tdata)
{	
	// --- transfer the 1D pointer to 2D/3D array pointer
    double ***Y = vector2Tensor(removedData, numUser, numService, numTimeSlice);
    double ***Y_hat = vector2Tensor(predData, numUser, numService, numTimeSlice);
    double **U = vector2Matrix(Udata, numUser, dim);
    double **S = vector2Matrix(Sdata, numService, dim);
    double **T = vector2Matrix(Tdata, numTimeSlice, dim);
 
	// --- create matricies and tensor
    bool ***I = vector2Tensor(new bool[numUser * numService * numTimeSlice], numUser, 
        numService, numTimeSlice);

    // --- compute indicator matrix I
    int i, j, k, l;
    for (i = 0; i < numUser; i++) {
        for (j = 0; j < numService; j++) {
            for (k = 0; k < numTimeSlice; k++) {
                I[i][j][k] = fabs(Y[i][j][k]) > eps;
            }
        }
    }
    
    updateY_hat(Y_hat, U, S, T, numUser, numService, numTimeSlice, dim);
  
    // --- iterate by muplication rules
    double t1, t2;
    for (int iter = 1; iter <= maxIter; iter++) {
        for (l = 0; l < dim; l++) {
            // update U
            for (i = 0; i < numUser; i++) {
                t1 = 0, t2 = 0;
                for (j = 0; j < numService; j++) {
                    for (k = 0; k < numTimeSlice; k++) {
                        t1 += I[i][j][k] * Y[i][j][k] * S[j][l] * T[k][l];
                        t2 += I[i][j][k] * Y_hat[i][j][k] * S[j][l] * T[k][l];
                    }
                }
                U[i][l] *= t1 / (t2 + lmda * U[i][l] + eps);
            }           
            updateY_hat(Y_hat, U, S, T, numUser, numService, numTimeSlice, dim);

            // update S
            for (j = 0; j < numService; j++) {
                t1 = 0, t2 = 0;
                for (i = 0; i < numUser; i++) {
                    for (k = 0; k < numTimeSlice; k++) {
                        t1 += I[i][j][k] * Y[i][j][k] * U[i][l] * T[k][l];
                        t2 += I[i][j][k] * Y_hat[i][j][k] * U[i][l] * T[k][l];
                    }
                }
                S[j][l] *= t1 / (t2 + lmda * S[j][l] + eps);
            }
            updateY_hat(Y_hat, U, S, T, numUser, numService, numTimeSlice, dim);
            
            // update T
            for (k = 0; k < numTimeSlice; k++) {
                t1 = 0, t2 = 0;
                for (i = 0; i < numUser; i++) {
                    for (j = 0; j < numService; j++) {
                        t1 += I[i][j][k] * Y[i][j][k] * U[i][l] * S[j][l];
                        t2 += I[i][j][k] * Y_hat[i][j][k] * U[i][l] * S[j][l];
                    }
                }
                T[k][l] *= t1 / (t2 + lmda * T[k][l] + eps);
            }
            updateY_hat(Y_hat, U, S, T, numUser, numService, numTimeSlice, dim);
        }

        pdd loss = lossFunction(I, Y, Y_hat, U, S, T, numUser, numService, 
            numTimeSlice, dim, lmda);
        
        // log the debug info
        cout.setf(ios::fixed);
        if (debugMode) {
         	cout << currentDateTime() << ": ";
            cout << "iter = " << iter << ", lossValue = " << loss.first + loss.second 
            	<< ", cost = " << loss.first << ", reg = " << loss.second << endl;
        }      
    }
      
    delete ((char*) U);
    delete ((char*) S);
    delete ((char*) T);
    delete ((char*) Y);
    delete ((char*) Y_hat);
}


inline double sqr(double x) {return x * x;}

pdd lossFunction(bool ***I, double ***Y, double ***Y_hat, double **U, double **S, 
    double **T, int m, int n, int c, int d, double lmda)
{
    double reg = 0, cost = 0;
    
    for (int l = 0; l < d; l++) {
        for (int i = 0; i < m; i++) {
            reg += sqr(U[i][l]);
        }
        for (int j = 0; j < n; j++) {
            reg += sqr(S[j][l]);
        }
        for (int k = 0; k < c; k++) {
            reg += sqr(T[k][l]);
        }
    }
    reg *= lmda;

    int i, j, k;
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            for (k = 0 ; k < c; k++) {
                cost += sqr(I[i][j][k] * (Y[i][j][k] - Y_hat[i][j][k]));
            }
        }
    }
    
    return pdd(cost / 2, reg / 2);
}


double updateY_hat(double ***Y_hat, double **U, double **S, double **T, int m, int n, 
    int c, int d)
{
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < c; k++) {
                Y_hat[i][j][k] = 0;
                for (int l = 0; l < d; l++) {
                    Y_hat[i][j][k] += U[i][l] * S[j][l] * T[k][l];
                }
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


double ***vector2Tensor(double *vector, int row, int col, int height)
{
	double ***tensor = new double **[row];
	if (!tensor) {
		cout << "Memory allocation failed in vector2Tensor." << endl;
		return NULL;
	}

	int i, j;
	for (i = 0; i < row; i++) {
		tensor[i] = new double *[col];
		if (!tensor[i]) {
			cout << "Memory allocation failed in vector2Tensor." << endl;
			return NULL;
		}

		for (j = 0; j < col; j++) {
			tensor[i][j] = vector + i * col * height + j * height;
		}
	}

	return tensor;
}


bool ***vector2Tensor(bool *vector, int row, int col, int height)
{
    bool ***tensor = new bool **[row];
    if (!tensor) {
        cout << "Memory allocation failed in vector2Tensor." << endl;
        return NULL;
    }
    
    int i, j;
    for (i = 0; i < row; i++) {
        tensor[i] = new bool *[col];
        if (!tensor[i]) {
            cout << "Memory allocation failed in vector2Tensor." << endl;
            return NULL;
        }
        
        for (j = 0; j < col; j++) {
            tensor[i][j] = vector + i * col * height + j * height;
        }
    }
    
    return tensor;
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

