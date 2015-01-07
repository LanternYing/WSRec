/********************************************************
 * CLUS.cpp
 * C++ implements on CLUS
 * Author: Yuwen Xiong <Orpine@GitHub>
 * Created: 2014/7/27
 * Last updated: 2014/7/27
********************************************************/

#include <iostream>
#include <cstring>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <vector>
#include <ctime>
#include <algorithm>
#include "CLUS.h"
using namespace std;


/// note that predData is the output of this function
void CLUS(double *removedData, double *predData, int numUser, int numService, 
	int numTimeSlice, vector<int> attrEv, vector<int> attrUs, vector<int> attrWs, 
    vector<vector<int> > clusterEv, vector<vector<int> > clusterUs, 
    vector<vector<int> > clusterWs, bool debugMode)
{	
	// --- transfer the 1D pointer to 2D/3D array pointer
    double ***Y = vector2Tensor(removedData, numUser, numService, numTimeSlice);
    double ***Y_hat = vector2Tensor(predData, numUser, numService, numTimeSlice);
    
    // --- temporal variables
    double *tf = new double[clusterUs.size() * clusterWs.size() * clusterEv.size()];
    memset(tf, 0, clusterUs.size() * clusterWs.size() * clusterEv.size() * sizeof(double));
    double ***f = vector2Tensor(tf, clusterUs.size(), clusterWs.size(), clusterEv.size());
    
    for (int i = 0; i < numUser; ++i) {
    	for (int j = 0; j < numService; ++j) {
    		for (int k = 0; k < numTimeSlice; ++k) {
                if (fabs(Y[i][j][k]) > eps) {
                    Y_hat[i][j][k] = Y[i][j][k];
                    continue;
                }

                // use the mean value in the same context cluster for prediction
    			int cnt = 0;
    			double tot = 0;
                for (int l = 0; l < clusterEv[attrEv[k]].size(); l++) {
    				tot += Y[i][j][clusterEv[attrEv[k]][l]];
    				cnt += (fabs(Y[i][j][clusterEv[attrEv[k]][l]]) > eps);
    			}
    			if (cnt != 0) {
    				Y_hat[i][j][k] = tot / cnt;
    				continue;
    			}
                
                // query the hash space f for prediction
                if (fabs(f[attrUs[i]][attrWs[j]][attrEv[k]]) > eps) {
                    Y_hat[i][j][k] = f[attrUs[i]][attrWs[j]][attrEv[k]];
                    continue;
                }

                // compute the hash space f
    			cnt = 0;
                tot = 0;
                for (int x = 0; x < clusterUs[attrUs[i]].size(); x++)
    			{
                    for (int y = 0; y < clusterWs[attrWs[j]].size(); y++)
                    {
                        for (int z = 0; z < clusterEv[attrEv[k]].size(); z++)
    					{
    						double Y_entry = Y[clusterUs[attrUs[i]][x]][clusterWs[attrWs[j]][y]]
    							[clusterEv[attrEv[k]][z]];
    						tot += Y_entry;
    						cnt += (fabs(Y_entry) > eps);
    					}
    				}
    			}
                if (cnt != 0) {
                    Y_hat[i][j][k] = tot / cnt;
                    f[attrUs[i]][attrWs[j]][attrEv[k]] = Y_hat[i][j][k];
                    continue;
                }

                // compute iMean as the prediction value
                cnt = 0;
                tot = 0;
                for (int x = 0; x < numUser; x++) {                   
                    tot += Y[x][j][k];
                    cnt += (fabs(Y[x][j][k]) > eps);
                }
                if (cnt != 0) {
                    Y_hat[i][j][k] = tot / cnt;
                    continue;
                }

                // compute uMean as the prediction value
                cnt = 0;
                tot = 0;
                for (int y = 0; y < numService; y++) {
                    tot += Y[i][y][k];
                    cnt += (fabs(Y[i][y][k]) > eps);
                }
                if (cnt != 0) {
                    Y_hat[i][j][k] = tot / cnt;
                    continue;
                }

                // compute overall mean as the prediction value
                cnt = 0;
                tot = 0;
                for (int x = 0; x < numUser; x++) {
                    for (int y = 0; y < numService; y++) {              
                        tot += Y[x][y][k];
                        cnt += (fabs(Y[x][y][k]) > eps);
                    }
                }
                Y_hat[i][j][k] = tot / (cnt + eps);
    		}
    	}
    }  
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

