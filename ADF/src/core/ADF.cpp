/********************************************************
 * ADF.cpp
 * C++ implements on ADF
 * Author: Yifei Lu <koplyf@gmail.com>, Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/4/29
 * Last updated: 2014/7/29
********************************************************/

#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <utility>
#include <algorithm> 
#include <cmath>
#include "ADF.h"
using namespace std;


void SIM(double *removedData, double *uMean, int numUser, 
	int numService, int topK, double *simData, int type)
{	
    double **simMatrix = vector2Matrix(simData, numUser, numUser);
    double **removedMatrix = vector2Matrix(removedData, numUser, numService);
    
    int i, j; 
    double **ADFMatrix = createMatrix(numUser, numUser);
	
  	for (i = 0; i < numUser; i++) {
		
  		for (j = 0; j < numUser; j++) {
  			if(uMean[i] == 0 || uMean[j] == 0) continue; 
  			double ADFValue = 0;
  			if (j > i) {		
                if(type == 0)	
				    ADFValue = getSIMU(removedMatrix[i], removedMatrix[j], uMean[i], 
				    	uMean[j], numService);
                if(type == 1)
                    ADFValue = getSIMI(removedMatrix[i], removedMatrix[j], uMean, 
                    	numService);
				ADFMatrix[i][j] = ADFValue;
				ADFMatrix[j][i] = ADFValue;
  			}
  			else if (j < i) {
  				ADFValue = ADFMatrix[i][j];
  			}
			// find similar users
			if (ADFValue > 0) simMatrix[i][j] = ADFValue;
  		}
        
        
  	}

  	delete2DMatrix(ADFMatrix);
    delete ((char*) simMatrix);
    delete ((char*) removedMatrix);
}


void PRED(double *removedData,double *removedRData, double *uMean, int numUser, 
	int numService, int topK, double *predData, double *simData)
{
    double **predMatrix = vector2Matrix(predData, numUser, numService);
    double **simMatrix = vector2Matrix(simData, numUser, numUser);
    double **removedMatrix = vector2Matrix(removedData, numUser, numService);
    double **removedRMatrix = vector2Matrix(removedRData, numUser, numService);
    
    int i, j; 
	
  	for (i = 0; i < numUser; i++) {
        
        map<int, double> ADFMap;
        double *simUser = simMatrix[i];
        
        for(j = 0; j < numUser; j++){
            if(simUser[j] > 0)
                ADFMap[j] = simUser[j]; 
        }
  		vector<pair<int, double> > sortedADFMap = sortMapByValue(ADFMap);

  		// predict the value for each entry 
		for (j = 0; j < numService; j++) {
			if(removedRMatrix[i][j] != 0) continue; // Skip the remained entries
			
			int k = 0;
			double ADFSum = 0; 
			double predValue = 0;
			vector<pair<int, double> >::iterator it = sortedADFMap.begin();
			while (k < topK && it != sortedADFMap.end()) {
				int userID = it->first;
				double userADFValue = it->second;
				it++;
				// if the similar user does not use this item previously, can not be used. 
				if (removedRMatrix[userID][j] == 0) continue;				
				ADFSum += userADFValue;
				k++;
				predValue += (userADFValue) * (removedMatrix[userID][j] - uMean[userID]);
			}

			// no similar users, use umean. 
			if (ADFSum == 0) {
				predValue = uMean[i];
			} 
			else { 
				predValue = predValue/ADFSum + uMean[i];
			}

			// will become worst, no need. 
			if (predValue <= 0) predValue = uMean[i];
			predMatrix[i][j] = predValue;
		}
  	}

    delete ((char*) simMatrix);
    delete ((char*) predMatrix);
    delete ((char*) removedMatrix);
    delete ((char*) removedRMatrix);
    
}


double getSIMU(double *uA, double *uB, double meanA, double meanB, int numService) {
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

	double SIMU = upperAll / downValue;

	return SIMU;
}


double getSIMI(double *uA, double *uB, double *uMean, int numService) {
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
        double mean = uMean[key];

		double tempA = valueA - mean;
		double tempB = valueB - mean;
		upperAll += tempA * tempB;
        downAllA += tempA * tempA;
        downAllB += tempB * tempB;
		
	}
    
    double downValue = sqrt(downAllA * downAllB);
    
    if(downValue == 0) return 0;

	double SIMI = upperAll / downValue;

	return SIMI;
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
  

vector<pair<int, double> > sortMapByValue(const map<int, double>& ADFMap) {
	vector<pair<int, double> > vec(ADFMap.begin(), ADFMap.end());  
    sort(vec.begin(), vec.end(), cmpPairbyValue); 
	return vec;
}