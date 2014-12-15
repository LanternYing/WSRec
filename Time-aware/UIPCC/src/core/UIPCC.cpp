/********************************************************
 * UIPCC.cpp
 * C++ implements on PCC and UPCC computations
 * Author: Jamie Zhu <jimzhu@GitHub>
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
#include "UIPCC.h"
using namespace std;


void UPCC_core(double *removedData, double *uMean, int numUser, int numService, 
	int topK, double *predData)
{	
    double **predMatrix = vector2Matrix(predData, numUser, numService);
    double **removedMatrix = vector2Matrix(removedData, numUser, numService);
    
    int i, j; 
    double **pccMatrix = createMatrix(numUser, numUser);

  	for (i = 0; i < numUser; i++) {
  		map<int, double> pccMap;

  		for (j = 0; j < numUser; j++) {
  			if(uMean[i] == 0 || uMean[j] == 0) continue; 
  			double pccValue = 0;
  			if (j > i) {			
				pccValue = getPCC(removedMatrix[i], removedMatrix[j], 
					uMean[i], uMean[j], numService);
				pccMatrix[i][j] = pccValue;
				pccMatrix[j][i] = pccValue;
  			}
  			else if (j < i) {
  				pccValue = pccMatrix[i][j];
  			}
			// find similar users
			if (pccValue > 0) pccMap[j] = pccValue;
  		}

  		vector<pair<int, double> > sortedPccMap = sortMapByValue(pccMap);

  		// predict the value for each entry 
		for (j = 0; j < numService; j++) {
			if(removedMatrix[i][j] != 0) continue; // Skip the remained entries
			
			int k = 0;
			double pccSum = 0; 
			double predValue = 0;
			vector<pair<int, double> >::iterator it = sortedPccMap.begin();
			while (k < topK && it != sortedPccMap.end()) {
				int userID = it->first;
				double userPCCValue = it->second;
				it++;
				// if the similar user does not use this item previously, can not be used. 
				if (removedMatrix[userID][j] == 0) continue;				
				pccSum += userPCCValue;
				k++;
				predValue += (userPCCValue) * (removedMatrix[userID][j] - uMean[userID]);
			}

			// no similar users, use umean. 
			if (pccSum == 0) {
				predValue = uMean[i];
			} 
			else { 
				predValue = predValue / pccSum + uMean[i];
			}

			// will become worst, no need. 
			if (predValue <= 0) predValue = uMean[i];
			predMatrix[i][j] = predValue;
		}
  	}

  	delete2DMatrix(pccMatrix);
    delete ((char*) predMatrix);
    delete ((char*) removedMatrix);
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


double getPCC(double *uA, double *uB, double meanA, double meanB, 
	int numService) {
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
	double downAllA = 0;
	double downAllB = 0;
	for (i = 0; i < commonIndex.size(); i++) {
		int key = commonIndex[i];
		double valueA = uA[key];
		double valueB = uB[key];

		double tempA = valueA - meanA;
		double tempB = valueB - meanB;

		upperAll += tempA * tempB;
		downAllA += tempA * tempA;
		downAllB += tempB * tempB;
	}
	double downValue = sqrt(downAllA * downAllB);

	if(downValue == 0) return 0;
	double pcc = upperAll / downValue;

	return pcc;
}

  
bool cmpPairbyValue(const pair<int, double>& lhs, const pair<int, double>& rhs) {  
  return lhs.second > rhs.second;  
}  
  

vector<pair<int, double> > sortMapByValue(const map<int, double>& pccMap) {
	vector<pair<int, double> > vec(pccMap.begin(), pccMap.end());  
    sort(vec.begin(), vec.end(), cmpPairbyValue); 
	return vec;
}

