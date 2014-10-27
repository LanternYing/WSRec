/********************************************************
 * CloudRank.cpp
 * C++ implements on CloudRank [Zheng et al, SRDS'2010]
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/6/16
********************************************************/

#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <algorithm>
#include "CloudRank.h"
using namespace std;


/********************************************************
 * predData is the output variable
********************************************************/
void CloudRank(double *removedData, int numUser, int numService, int topK, int *predData)
{	
	// --- transform the 1D pointer to 2D array pointer
    double **removedMatrix = vector2Matrix(removedData, numUser, numService);
    int **predRankMatrix = vector2Matrix(predData, numUser, numService);

    // --- create temporal matrices
    double **krccMatrix = createZeroMatrix(numUser, numUser);
    double **preferenceMatrix = createZeroMatrix(numService, numService);
	double *imean = createVector(numService);

	int i, j;
	// calculate imean
	for (j = 0; j < numService; j++) {
		double sum = 0;
		int count = 0;
		for (i = 0; i < numUser; i++) {
			if (removedMatrix[i][j] > 0) {
				sum += removedMatrix[i][j];
				count++;
			}
		}
		if (count > 0) {
			imean[j] = sum / count;
		}
		else imean[j] = 0;
	}
    
    // rank prediction
    for (i = 0; i < numUser; i++) {
    	map<int, double> krccMap;

    	for (j = 0; j < numUser; j++) {
    		double krcc;
    		if (j > i) {			
				krcc = getKRCC(removedMatrix[i], removedMatrix[j], numService);
				krccMatrix[i][j] = krcc;
				krccMatrix[j][i] = krcc;
  			}
  			else if (j < i) {
  				krcc = krccMatrix[i][j];
  			}
			// find similar users
			if (krcc > 0) krccMap[j] = krcc;
		}

		vector<pair<int, double> > sortedkrccMap = sortMapByValue(krccMap);

		getPreferenceMatrix(removedMatrix, imean, i, sortedkrccMap, topK, preferenceMatrix, 
			numUser, numService);

		greedyRank(removedMatrix[i], preferenceMatrix, predRankMatrix[i], numService, true);
    }
  	
    deleteVector(imean);
    deleteMatrix(krccMatrix);
    deleteMatrix(preferenceMatrix);
    delete ((char*) removedMatrix);
    delete ((char*) predRankMatrix);
}


double getKRCC(double *uA, double *uB, int numService)
{
	double krcc = 0;

	vector<int> commonIndex;
	int i, j;
	for (i = 0; i < numService; i++) {
			if(uA[i] > 0 && uB[i] > 0) {
				commonIndex.push_back(i);
			}
	}

	// no common rate items. 
	if(commonIndex.size() < 2) return 0;

	int numPairs = 0;
	for (i = 0; i < commonIndex.size(); i++) {
		int key1 = commonIndex[i];
		for (j = i + 1; j < commonIndex.size(); j++) {
			int key2 = commonIndex[j];
			if ((uA[key1] - uA[key2]) * (uB[key1] - uB[key2]) < 0) {
				numPairs++;
			}
		}
	}

	krcc = 1 - (4.0 * numPairs) / (commonIndex.size() * (commonIndex.size() - 1));

	return krcc;
}


void getPreferenceMatrix(double **removedMatrix, double *imean, int uid, 
	vector<pair<int, double> > &sortedkrccMap, int topK, double **preferenceMatrix, 
	int numUser, int numService) 
{
	int i, j;
	// calculate preference matrix
	for (i = 0; i < numService; i++) {
		for (j = i + 1; j < numService; j++) {
			// has QoS values of both services
			if (removedMatrix[uid][i] > 0 && removedMatrix[uid][j] > 0) {
				preferenceMatrix[i][j] = removedMatrix[uid][i] - removedMatrix[uid][j];
			}
			// use information of similar users
			else {
				int k = 0;
				double krccSum = 0; 
				double preference = 0;
				vector<pair<int, double> >::iterator it = sortedkrccMap.begin();
				while (k < topK && it != sortedkrccMap.end()) {
					int userid = it -> first;
					double krcc = it -> second;
					it++;
					// if the similar user does not use this item previously, can not be used. 
					if (removedMatrix[userid][i] > 0 && removedMatrix[userid][j] > 0) {		
						krccSum += krcc;						
						preference += (krcc) * (removedMatrix[userid][i] - removedMatrix[userid][j]);
						k++;
					}
				}

				// no similar user has rating of both i and j
				// use imean for the prediction
				if (krccSum == 0) {
					if (imean[i] > 0 && imean[j] > 0) {
						preferenceMatrix[i][j] = imean[i] - imean[j];
					}
					else preferenceMatrix[i][j] = 0;
				}
				// has informaiton from similar users 
				else {
					preferenceMatrix[i][j] = preference / krccSum;
				}	
			}
			preferenceMatrix[j][i] = -preferenceMatrix[i][j];
		}
	}
}


void greedyRank(double *removedQoSVector, double **preferenceMatrix, int *rankVector, 
	int numService, bool cloudrank)
{
	double *preferenceSum = createVector(numService);
	double *serviceFlag = createVector(numService);

	int i, j;
	for (i = 0; i < numService; i++) {
		for (j = 0; j < numService; j++) {
			preferenceSum[i] += preferenceMatrix[i][j];
		}
	}

	for (i = 0; i < numService; i++) {
		int maxIdx = -1;
		for (j = 0; j < numService; j++) {
			if (serviceFlag[j] == 0) {
				if (maxIdx == -1) {
					maxIdx = j;
				}
				else if (preferenceSum[j] > preferenceSum[maxIdx]) {
					maxIdx = j;
				}
			}
		}

		// update serviceFlag
		serviceFlag[maxIdx] = 1;
		rankVector[i] = maxIdx;
		// update preferenceSum
		for (j = 0; j < numService; j++) {
			preferenceSum[j] -= preferenceMatrix[j][maxIdx];
		}
	}

	// enhance the rank (guarantee the employed services are correctly ranked)
	if (cloudrank == true) {
		map<int, double> observedQoSMap;
		for (j = 0; j < numService; j++) {
			if (removedQoSVector[j] > 0) {
				observedQoSMap[j] = removedQoSVector[j];
			}
		}
		vector<pair<int, double> > sortedQoSMap = sortMapByValue(observedQoSMap);

		int idx = 0;
		for (j = 0; j < numService; j++) {
			if (removedQoSVector[rankVector[j]] > 0) {
				rankVector[j] = sortedQoSMap[idx].first;
				idx++;
			}
		}
	}

	deleteVector(preferenceSum);
	deleteVector(serviceFlag);
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


int **vector2Matrix(int *vector, int row, int col)  
{
	int **matrix = new int *[row];
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


double **createZeroMatrix(int row, int col) 
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


void deleteMatrix(double **ptr) 
{
	delete ptr[0];
	delete ptr;
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


bool cmpPairbyValue(const pair<int, double>& lhs, const pair<int, double>& rhs) {  
  return lhs.second > rhs.second;  
}  
  

vector<pair<int, double> > sortMapByValue(const map<int, double>& pccMap) {
	vector<pair<int, double> > vec(pccMap.begin(), pccMap.end());  
    sort(vec.begin(), vec.end(), cmpPairbyValue); 
	return vec;
}

