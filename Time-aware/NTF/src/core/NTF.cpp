/********************************************************
 * NTF.cpp
 * C++ implements on NTF
 * Author: Yuwen Xiong, Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/7/15
********************************************************/

#include <iostream>
#include <cstring>
#include <cmath>
#include <iomanip>
#include "NTF.h"
#include <vector>
#include <ctime>
#include <algorithm>

using namespace std;


/// note that predData is the output of this function
void NTF(double *removedData, double *predData, int numUser, int numService, 
	int numTimeSlice, int dim, double lmda, int maxIter, bool debugMode, 
	double *Udata, double *Sdata, double *Tdata)
{	
	// --- transfer the 1D pointer to 2D/3D array pointer
    double ***Y = vector2Tensor(removedData, numUser, numService, numTimeSlice);
    double ***Y_hat = vector2Tensor(predData, numUser, numService, numTimeSlice);
    double **U = vector2Matrix(Udata, numUser, dim);
    double **S = vector2Matrix(Sdata, numService, dim);
    double **T = vector2Matrix(Tdata, numTimeSlice, dim);
     
    // --- iterate by muplication rules
    double t1, t2;
    for (int iter = 1; iter <= maxIter; iter++) {
        // update Y_hat
        updateY_hat(false, Y, Y_hat, U, S, T, numUser, numService, numTimeSlice, dim);

        // log the debug info
        cout.setf(ios::fixed);
        if (debugMode) {
            pdd loss = lossFunction(Y, Y_hat, U, S, T, numUser, numService, 
                numTimeSlice, dim, lmda);
            cout << currentDateTime() << ": ";
            cout << "iter = " << iter << ", lossValue = " << loss.first + loss.second 
                << ", cost = " << loss.first << ", reg = " << loss.second << endl;
        }  

        // update U
        int i, j, k, l;
        for (i = 0; i < numUser; i++) {          
            for (l = 0; l < dim; l++) {
                t1 = 0, t2 = 0;
                for (j = 0; j < numService; j++) {
                    for (k = 0; k < numTimeSlice; k++) {
                        t1 += Y[i][j][k] * S[j][l] * T[k][l];
                        t2 += Y_hat[i][j][k] * S[j][l] * T[k][l];
                    }
                }
                U[i][l] *= t1 / (t2 + lmda * U[i][l] + eps);
            } 
        }   

        // update Y_hat
        updateY_hat(false, Y, Y_hat, U, S, T, numUser, numService, numTimeSlice, dim);

        // update S
        for (j = 0; j < numService; j++) {
            for (l = 0; l < dim; l++) {
                t1 = 0, t2 = 0;
                for (i = 0; i < numUser; i++) {
                    for (k = 0; k < numTimeSlice; k++) {
                        t1 += Y[i][j][k] * U[i][l] * T[k][l];
                        t2 += Y_hat[i][j][k] * U[i][l] * T[k][l];
                    }
                }
                S[j][l] *= t1 / (t2 + lmda * S[j][l] + eps);
            }
        }

        // update Y_hat
        updateY_hat(false, Y, Y_hat, U, S, T, numUser, numService, numTimeSlice, dim);
            
        // update T
        for (k = 0; k < numTimeSlice; k++) {
            for (l = 0; l < dim; l++) {
                t1 = 0, t2 = 0;
                for (i = 0; i < numUser; i++) {
                    for (j = 0; j < numService; j++) {
                        t1 += Y[i][j][k] * U[i][l] * S[j][l];
                        t2 += Y_hat[i][j][k] * U[i][l] * S[j][l];
                    }
                }
                T[k][l] *= t1 / (t2 + lmda * T[k][l] + eps);
            }
        }  
    }

    // update Y_hat
    updateY_hat(true, Y, Y_hat, U, S, T, numUser, numService, numTimeSlice, dim);
  
    delete ((char*) U);
    delete ((char*) S);
    delete ((char*) T);
    delete ((char*) Y);
    delete ((char*) Y_hat);
}


inline double sqr(double x) {return x * x;}

pdd lossFunction(double ***Y, double ***Y_hat, double **U, double **S, double **T, 
    int numUser, int numService, int numTimeSlice, int dim, double lmda)
{
    double reg = 0, cost = 0;
    
    for (int l = 0; l < dim; l++) {
        for (int i = 0; i < numUser; i++) {
            reg += sqr(U[i][l]);
        }
        for (int j = 0; j < numService; j++) {
            reg += sqr(S[j][l]);
        }
        for (int k = 0; k < numTimeSlice; k++) {
            reg += sqr(T[k][l]);
        }
    }
    reg *= lmda;

    for (int i = 0; i < numUser; i++) {
        for (int j = 0; j < numService; j++) {
            for (int k = 0 ; k < numTimeSlice; k++) {
                if (Y[i][j][k] > 0) {
                    cost += sqr(Y[i][j][k] - Y_hat[i][j][k]);
                }
            }
        }
    }
    
    return pdd(cost / 2, reg / 2);
}


void updateY_hat(bool flag, double ***Y, double ***Y_hat, double **U, double **S, 
    double **T, int numUser, int numService, int numTimeSlice, int dim)
{
    for (int i = 0; i < numUser; i++) {
        for (int j = 0; j < numService; j++) {
            for (int k = 0; k < numTimeSlice; k++) {
                if (flag == true || Y[i][j][k] > 0) {
                    Y_hat[i][j][k] = 0;
                    for (int l = 0; l < dim; l++) {
                        Y_hat[i][j][k] += U[i][l] * S[j][l] * T[k][l];
                    }
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

