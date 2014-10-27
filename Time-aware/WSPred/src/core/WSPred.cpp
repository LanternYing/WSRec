/********************************************************
 * WSPred.cpp
 * C++ implements on WSPred
 * Author: Yuwen Xiong, Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/7/19
********************************************************/

#include <iostream>
#include <cmath>
#include <cstring>
#include <iomanip>
#include "WSPred.h"
#include <vector>
#include <ctime>
#include <algorithm>
using namespace std; 


/// note that predData is the output of this function
void WSPred(double *removedData, double *predData, int numUser, int numService, 
	int numTimeSlice, int dim, double etaInit, double lmda, double alpha, int maxIter, 
	bool debugMode,	double *Udata, double *Sdata, double *Tdata)
{	
	// --- transfer the 1D pointer to 2D/3D array pointer
    double ***Y = vector2Tensor(removedData, numUser, numService, numTimeSlice);
    double ***Y_hat = vector2Tensor(predData, numUser, numService, numTimeSlice);
    double **U = vector2Matrix(Udata, dim, numUser);
    double **S = vector2Matrix(Sdata, dim, numService);
    double **T = vector2Matrix(Tdata, dim, numTimeSlice);
    
    // --- create matricies and tensor
    bool ***I = vector2Tensor(new bool[numUser * numService * numTimeSlice], numUser, 
    	numService, numTimeSlice);
    double **U_t = createMatrix(dim, numUser);
    double **S_t = createMatrix(dim, numService);
    double **T_t = createMatrix(dim, numTimeSlice);
    double **gradU = createMatrix(dim, numUser);
    double **gradS = createMatrix(dim, numService);
    double **gradT = createMatrix(dim, numTimeSlice);
    double **Y_bar = createMatrix(numUser, numService);

    // --- compute indicator matrix I
    for (int i = 0; i < numUser; i++) {
        for (int j = 0; j < numService; j++) {
            for (int k = 0; k < numTimeSlice; k++) {
                I[i][j][k] = fabs(Y[i][j][k]) > eps;
            }
        }
    }

    // --- compute average matrix Y_bar
    for (int i = 0; i < numUser; i++) {
        for (int j = 0; j < numService; j++) {
            int cnt = 0;
            for (int k = 0; k < numTimeSlice; k++) {
                Y_bar[i][j] += Y[i][j][k];
                if (I[i][j][k]) {
                    cnt++;
                }
            }
            Y_bar[i][j] /= max(cnt, 1);
        }
    }

    updateY_hat(Y_hat, U, S, T, numUser, numService, numTimeSlice, dim);   
    tuple loss_tuple = lossFunction(I, Y, Y_hat, Y_bar, U, S, T, numUser, numService, 
    	numTimeSlice, dim, lmda, alpha);
    double lossValue = loss_tuple.first.first + loss_tuple.first.second + loss_tuple.second;
    
    // --- iterate by gradient descent
    for (int iter = 1; iter <= maxIter; iter++) {
    	// compute gradients
        calcGrad(gradU, gradS, gradT, I, Y, Y_hat, Y_bar, U, S, T, lmda, alpha, numUser, 
        	numService, numTimeSlice, dim);
        // perform line search      
        pair<double, tuple> out = linesearch(I, Y, Y_hat, Y_bar, U, S, T, gradU, gradS, gradT, 
        	U_t, S_t, T_t, etaInit, lmda, alpha, lossValue, numUser, 
        	numService, numTimeSlice, dim);

        double eta = out.first;
        loss_tuple = out.second;
        lossValue = loss_tuple.first.first + loss_tuple.first.second + loss_tuple.second;       

        // update U, S, T
        updateUST(U, U, eta, gradU, dim, numUser);
        updateUST(S, S, eta, gradS, dim, numService);
        updateUST(T, T, eta, gradT, dim, numTimeSlice);                

        // log the debug info
        cout.setf(ios::fixed);
        if (debugMode) {
         	cout << currentDateTime() << ": ";
            cout << "iter = " << iter << ", lossValue = " << lossValue << ", cost1 = " 
            	<< loss_tuple.first.first << ", cost2 = " << loss_tuple.first.second 
                << ", reg = " << loss_tuple.second << ", eta = " << eta << endl;
        }       
    }
        
    delete ((char*) U);
    delete ((char*) S);
    delete ((char*) T);
    delete ((char*) Y);
    delete ((char*) Y_hat);
    delete2DMatrix(U_t);
    delete2DMatrix(S_t);
    delete2DMatrix(T_t);
    delete2DMatrix(gradU);
    delete2DMatrix(gradS);
    delete2DMatrix(gradT);
    delete2DMatrix(Y_bar);
}


inline double sqr(double x) {return x * x;}

tuple lossFunction(bool ***I, double ***Y, double ***Y_hat, double **Y_bar, double **U, 
	double **S, double **T, int m, int n, int c, int d, double lmda, double alpha)
{
    double reg = 0, cost1 = 0, cost2 = 0;

    for (int l = 0; l < d; l++) {
        for (int i = 0; i < m; i++) {
            reg += sqr(U[l][i]);
        }
        for (int j = 0; j < n; j++) {
            reg += sqr(S[l][j]);
        }
        for (int k = 0; k < c; k++) {
            reg += sqr(T[l][k]);
        }
    }
    reg *= lmda;

    int i, j, k;
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            for (k = 0 ; k < c; k++) {
                cost1 += sqr(I[i][j][k] * (Y[i][j][k] - Y_hat[i][j][k]));
                cost2 += alpha * sqr(I[i][j][k] * (Y_hat[i][j][k] - Y_bar[i][j]));
            }
        }
    }

    return make_pair(make_pair(cost1 / 2, cost2 / 2), reg / 2);
}


void calcGrad(double **gradU, double **gradS, double **gradT, bool ***I, double ***Y, 
	double ***Y_hat, double **Y_bar, double **U, double **S, double **T, double lmda, 
	double alpha, int numUser, int numService, int numTimeSlice, int dim)
{
    for (int l = 0; l < dim; l++) {
        for (int i = 0; i < numUser; i++) {
            gradU[l][i] = lmda * U[l][i];
        }
        for (int j = 0; j < numService; j++) {
            gradS[l][j] = lmda * S[l][j];
        }
        for (int k = 0; k < numTimeSlice; k++) {
            gradT[l][k] = lmda * T[l][k];
        }

        for (int i = 0; i < numUser; i++) {
            for (int j = 0; j < numService; j++) {
                for (int k = 0; k < numTimeSlice; k++) {
                    gradU[l][i] += I[i][j][k] * ((Y_hat[i][j][k] - Y[i][j][k]) 
                    	+ alpha * (Y_hat[i][j][k] - Y_bar[i][j])) * (S[l][j] * T[l][k]);
                    gradS[l][j] += I[i][j][k] * ((Y_hat[i][j][k] - Y[i][j][k]) 
                    	+ alpha * (Y_hat[i][j][k] - Y_bar[i][j])) * (U[l][i] * T[l][k]);
                    gradT[l][k] += I[i][j][k] * ((Y_hat[i][j][k] - Y[i][j][k]) 
                    	+ alpha * (Y_hat[i][j][k] - Y_bar[i][j])) * (U[l][i] * S[l][j]);
                }
            }
        }
    }
}


void updateUST(double **aim, double **ori, double eta, double **grad, int m, int n)
{
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            aim[i][j] = ori[i][j] - eta * grad[i][j];
        }
    }
}


void updateY_hat(double ***Y_hat, double **U, double **S, double **T, int m, int n, 
	int c, int d)
{
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < c; k++) {
                Y_hat[i][j][k] = 0;
                for (int l = 0; l < d; l++) {
                    Y_hat[i][j][k] += U[l][i] * S[l][j] * T[l][k];
                }
            }
        }
    }
}


pair<double, tuple> linesearch(bool ***I, double ***Y, double ***Y_hat, double **Y_bar, 
	double **U, double **S, double **T, double **gradU, double **gradS, double **gradT, 
    double **U_t, double **S_t, double ** T_t, double eta, double lmda, double alpha, 
    double lastLossValue, int numUser, int numService, int numTimeSlice, int dim)
{  
    tuple loss_tuple;
    double lossValue;

    for (int iter = 0; iter < 20; iter++) {
        updateUST(U_t, U, eta, gradU, dim, numUser);
        updateUST(S_t, S, eta, gradS, dim, numService);
        updateUST(T_t, T, eta, gradT, dim, numTimeSlice);
        
        updateY_hat(Y_hat, U_t, S_t, T_t, numUser, numService, numTimeSlice, dim);        
        loss_tuple = lossFunction(I, Y, Y_hat, Y_bar, U_t, S_t, T_t, numUser, numService, 
        	numTimeSlice, dim, lmda, alpha);
        lossValue = loss_tuple.first.first + loss_tuple.first.second + loss_tuple.second;

        if (lossValue < lastLossValue) {
            break;
        }
        eta /= 2;
    }

    return make_pair(eta, loss_tuple);
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


