/********************************************************
 * NRCF.cpp
 * C++ implements on NRCF
 * Author: Yifei Lu, Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/4/29
 * Last updated: 2014/4/29
********************************************************/

#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <utility>
#include <algorithm> 
#include <cmath>
#include "NRCF.h"
using namespace std;

const double eps = 1e-16;


void U_NRCF_core(double *removedData, double *uMax, double *uMean, int numUser, 
	int numService, int topK, double *predData)
{	
    double **predMatrix = vector2Matrix(predData, numUser, numService);
    double **removedMatrix = vector2Matrix(removedData, numUser, numService);
    
    int i, j; 
    double **NRMatrix = createMatrix(numUser, numUser);
	
  	for (i = 0; i < numUser; i++) {
  		map<int, double> NRMap;
		
		double umax=uMax[i];
  		for (j = 0; j < numUser; j++) {
  			if(uMax[i] == 0 || uMax[j] == 0) continue; 
			double vmax=uMax[j];
  			double NRValue = 0;
  			if (j > i) {			
				NRValue = getNR(removedMatrix[i], removedMatrix[j], umax, vmax, numService);
				NRMatrix[i][j] = NRValue;
				NRMatrix[j][i] = NRValue;
  			}
  			else if (j < i) {
  				NRValue = NRMatrix[i][j];
  			}
			// find similar users
			if (NRValue > 0) NRMap[j] = NRValue;
  		}

  		vector<pair<int, double> > sortedNRMap = sortMapByValue(NRMap);

  		// predict the value for each entry 
		for (j = 0; j < numService; j++) {
			if(removedMatrix[i][j] != 0) continue; // Skip the remained entries
			
			int k = 0;
			double NRSum = 0; 
			double predValue = 0;
			vector<pair<int, double> >::iterator it = sortedNRMap.begin();
			while (k < topK && it != sortedNRMap.end()) {
				int userID = it->first;
				double userNRValue = it->second;
				it++;
				// if the similar user does not use this item previously, can not be used. 
				if (removedMatrix[userID][j] == 0) continue;				
				NRSum += userNRValue;
				k++;
				double nr = removedMatrix[userID][j] / uMax[userID];
				predValue += (userNRValue) * nr;
			}

			// no similar users, use umean. 
			if (NRSum == 0) {
				predValue = uMean[i];
			} 
			else { 
				predValue = umax * predValue / NRSum ;
			}

			predMatrix[i][j] = predValue;
		}
  	}

  	delete2DMatrix(NRMatrix);
    delete ((char*) predMatrix);
    delete ((char*) removedMatrix);
}


double getNR(double *uA, double *uB, double maxA, 
	double maxB, int numService) {
	vector<int> commonIndex;
	int i;
	for (i = 0; i < numService; i++) {
			if(uA[i] > 0 && uB[i] > 0) {
				commonIndex.push_back(i);
			}
	}

	// no common rate items. 
	if(commonIndex.size() < 2) return 0;

	double upperAll = 0;
	
	for (i = 0; i < commonIndex.size(); i++) {
		int key = commonIndex[i];
		double valueA = uA[key];
		double valueB = uB[key];

		double tempA = valueA /(maxA + eps);
		double tempB = valueB /(maxB + eps);
		double temp = tempA - tempB;
		upperAll += temp * temp;
		
	}
	
	double downall=sqrt(commonIndex.size())+eps;
	double NR = 1 - upperAll / downall;

	return NR;
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


double **createMatrix(int row, int col) {
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



bool cmpPairbyValue(const pair<int, double>& lhs, const pair<int, double>& rhs) {  
  return lhs.second > rhs.second;  
}  
  

vector<pair<int, double> > sortMapByValue(const map<int, double>& NRMap) {
	vector<pair<int, double> > vec(NRMap.begin(), NRMap.end());  
    sort(vec.begin(), vec.end(), cmpPairbyValue); 
	return vec;
}

