/********************************************************
 * UIPCC_core.cpp
 * C++ implements on PCC and UPCC computations
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/4/29
 * Last updated: 2014/5/14
********************************************************/

#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <utility>
#include <algorithm> 
#include <cmath>
#include "UIPCC_core.h"
using namespace std;


vector<vector<pair<int, double> > > getPCCMatrix(double **removedMatrix, 
	int numUser, int numService, int topK)
{
	vector<vector<pair<int, double> > > pccUserMatrix;
	pccUserMatrix.resize(numUser);

	// --- create vectors and matrices
	double *uMean = createVector(numUser);
	double **pccMatrix = createMatrix(numUser, numUser);
    
    // --- get average values
    int i, j; 
    for (i = 0; i < numUser; i++) {
    	double avg = 0;
    	int count = 0;
    	for (j = 0; j < numService; j++) {
    		if (removedMatrix[i][j] > 0) {
    			avg += removedMatrix[i][j];
    			count++;
    		}
    	}
    	if (count == 0) continue;
    	uMean[i] = avg / count;
    }

  	for (i = 0; i < numUser; i++) {
  		map<int, double> pccMap;
  		for (j = 0; j < numUser; j++) {
  			if(uMean[i] == 0 || uMean[j] == 0) continue; 
  			double pccValue = 0;
  			if (j > i) {			
				pccValue = getPCC(removedMatrix[i], removedMatrix[j], 
					uMean[i], uMean[j], numService);
				pccMatrix[j][i] = pccValue;
  			}
  			else if (j < i) {
  				pccValue = pccMatrix[i][j];
  			}
			// find similar users
			if (pccValue > 0) pccMap[j] = pccValue;
  		}

  		vector<pair<int, double> > sortedPccMap = sortMapByValue(pccMap);
		
		int k = 0;
		double sum = 0;
		vector<pair<int, double> >::iterator it = sortedPccMap.begin();
		while (k < topK && it != sortedPccMap.end()) {
			double pccValue = it->second;
			sum += pccValue;
			it++;
			k++;
		}

		if (sum == 0) continue;
		k = 0;
		it = sortedPccMap.begin();
		while (k < topK && it != sortedPccMap.end()) {
			int userID = it->first;
			double pccValue = it->second;
			pccUserMatrix[i].push_back(make_pair(userID, pccValue / sum));
			it++;
			k++;
		}
  	}

  	deleteVector(uMean);
  	deleteMatrix(pccMatrix);
  	return pccUserMatrix;
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


double *createVector(int size) 
{
    double *vec = new double[size];
    memset(vec, 0, size * sizeof(double)); // Initialization
    return vec;
}


void deleteVector(double *ptr) {
	delete ptr;
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


void deleteMatrix(double **ptr) {
	delete ptr[0];
	delete ptr;
}