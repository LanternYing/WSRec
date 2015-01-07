/********************************************************
 * CLUS.h: header file of CLUS.cpp
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/7/15
********************************************************/

#include <algorithm>
#include <vector>
using namespace std;

const double eps = 1e-8;

/* Perform the core approach of CLUS */
void CLUS(double *removedData, double *predData, int numUser, int numService, 
	int numTimeSlice, vector<int> attrEv, vector<int> attrUs, vector<int> attrWs, 
    vector<vector<int> > clusterEv, vector<vector<int> > clusterUs, 
    vector<vector<int> > clusterWs, bool debugMode);

/* Transform a vector into a 3D tensor */ 
double ***vector2Tensor(double *vector, int row, int col, int height);

/* Get current date/time, format is YYYY-MM-DD hh:mm:ss */
const string currentDateTime();

