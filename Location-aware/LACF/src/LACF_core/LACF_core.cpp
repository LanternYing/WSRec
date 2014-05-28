/********************************************************
 * LACF_core.cpp
 * C++ implements on LACF [Tang et al, ICWS'2012]
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/17
********************************************************/

#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <utility>
#include <algorithm> 
#include <cmath>
#include "LACF_core.h"
using namespace std;


/********************************************************
 * predData is the output variable
********************************************************/
void LACF_core(double *removedData, double *userGroupByASData, double *userGroupByCountryData,
	double *wsGroupByASData, double *wsGroupByCountryData, int numUser, int numService, 
	int topK, double lmda, double *predData)
{	
	// --- transform the 1D pointer to 2D array pointer
    double **removedMatrix = vector2Matrix(removedData, numUser, numService);
    double **predMatrix = vector2Matrix(predData, numUser, numService);
    double **userGroupByAS = vector2Matrix(userGroupByASData, numUser, numUser);
    double **userGroupByCountry = vector2Matrix(userGroupByCountryData, numUser, numUser);
    double **wsGroupByAS = vector2Matrix(wsGroupByASData, numService, numService);
    double **wsGroupByCountry = vector2Matrix(wsGroupByCountryData, numService, numService);

    // --- transform user and service group matrix into dictionary (data structures)
    vector<vector<int> > userGroupDicByAS = matrix2Dic(userGroupByAS, numUser);
    vector<vector<int> > userGroupDicByCountry = matrix2Dic(userGroupByCountry, numUser);
    vector<vector<int> > wsGroupDicByAS = matrix2Dic(wsGroupByAS, numService);
    vector<vector<int> > wsGroupDicByCountry = matrix2Dic(wsGroupByCountry, numService);
   
    // --- create temporal matrices
    double **removedMatrix_T = createMatrix(numService, numUser);
    double **IPCCMatrix = createMatrix(numUser, numService);

    // --- UPCC 
    double **UPCCMatrix = UPCC(removedMatrix, userGroupDicByAS, userGroupDicByCountry, numUser, 
    	numService, topK);
    
    // --- IPCC
    transpose(removedMatrix, removedMatrix_T, numUser, numService);
	double **IPCCMatrix_T = UPCC(removedMatrix_T, wsGroupDicByAS, wsGroupDicByCountry, numService, 
    	numUser, topK);
	transpose(IPCCMatrix_T, IPCCMatrix, numService, numUser);

    // --- UIPCC
    int i, j;
	for (i = 0; i < numUser; i++) {
		for (j = 0; j < numService; j++) {
			if (UPCCMatrix[i][j] != 0 && IPCCMatrix[i][j] != 0) {
				predMatrix[i][j] = lmda * UPCCMatrix[i][j] + (1 - lmda) * IPCCMatrix[i][j];
			}
			else if (UPCCMatrix[i][j] != 0 && IPCCMatrix[i][j] == 0) {
				predMatrix[i][j] = UPCCMatrix[i][j];
			}
			else if (UPCCMatrix[i][j] == 0 && IPCCMatrix[i][j] != 0) {
				predMatrix[i][j] = IPCCMatrix[i][j];
			}
			// cold-start problem
			else if (UPCCMatrix[i][j] == 0 && IPCCMatrix[i][j] == 0) {
				predMatrix[i][j] = coldstartFill(removedMatrix, userGroupDicByAS, 
					userGroupDicByCountry, wsGroupDicByAS, wsGroupDicByCountry, 
					i, j, numUser, numService);
			}
		}
	}

    deleteMatrix(UPCCMatrix);
    deleteMatrix(removedMatrix_T);
    deleteMatrix(IPCCMatrix);
    deleteMatrix(IPCCMatrix_T);
    delete ((char*) userGroupByAS);
    delete ((char*) userGroupByCountry);
    delete ((char*) wsGroupByAS);
    delete ((char*) wsGroupByCountry);
    delete ((char*) removedMatrix);
    delete ((char*) predMatrix);
}


double **UPCC(double **removedMatrix, vector<vector<int> > &userGroupDicByAS, 
	vector<vector<int> > &userGroupDicByCountry, int numUser, int numService, int topK)
{	 
    int i, j;
    double **predMatrix = createMatrix(numUser, numService); 
    double **pccMatrix = createMatrix(numUser, numUser);
    double *uMean = createVector(numUser);

    // --- compute the uMean values
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

    // --- compute the UPCC
  	for (i = 0; i < numUser; i++) {
  		map<int, double> pccMap;

  		// --- extract similar users
  		// extract similar users from AS group
  		for (j = 0; j < userGroupDicByAS[i].size(); j++) {
  			int id = userGroupDicByAS[i][j];
  			if(uMean[i] == 0 || uMean[id] == 0) continue; 
  			double pccValue = 0;
  			if (pccMatrix[i][id] == 0) {			
				pccValue = getPCC(removedMatrix[i], removedMatrix[id], 
					uMean[i], uMean[id], numService);
				pccMatrix[i][id] = pccValue;
				pccMatrix[id][i] = pccValue;
  			}
  			else {
  				pccValue = pccMatrix[i][id];
  			}
			// find similar users
			if (pccValue > 0) pccMap[id] = pccValue;
  		}

  		// extract similar users from country group
  		if (pccMap.size() < topK) {
	  		for (j = 0; j < userGroupDicByCountry[i].size(); j++) {
	  			int id = userGroupDicByCountry[i][j];
	  			if(uMean[i] == 0 || uMean[id] == 0) continue; 
	  			double pccValue = 0;
	  			if (pccMatrix[i][id] == 0) {			
					pccValue = getPCC(removedMatrix[i], removedMatrix[id], 
						uMean[i], uMean[id], numService);
					pccMatrix[i][id] = pccValue;
					pccMatrix[id][i] = pccValue;
	  			}
	  			else {
	  				pccValue = pccMatrix[i][id];
	  			}
				// find similar users
				if (pccValue > 0) pccMap[id] = pccValue;
	  		}
  		}

  		// extract similar users from all
  		if (pccMap.size() < topK) {
  			for (j = 0; j < numUser; j++) {
	  			if(uMean[i] == 0 || uMean[j] == 0) continue; 
	  			double pccValue = 0;
	  			if (pccMatrix[i][j] == 0) {			
					pccValue = getPCC(removedMatrix[i], removedMatrix[j], 
						uMean[i], uMean[j], numService);
					pccMatrix[i][j] = pccValue;
					pccMatrix[j][i] = pccValue;
	  			}
	  			else {
	  				pccValue = pccMatrix[i][j];
	  			}
				// find similar users
				if (pccValue > 0) pccMap[j] = pccValue;
  			} 			
  		}

  		// --- compute the missing values
  		if (pccMap.size() > 0) {
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

				// if pccSum == 0, ignore it.
				if (pccSum > 0) {
					predValue = predValue/pccSum + uMean[i];
				}
				// negative value, ignore it. 
				if (predValue < 0) predValue = 0;
				predMatrix[i][j] = predValue;
			}
		}
  	}

	deleteVector(uMean);
  	deleteMatrix(pccMatrix);
  	return predMatrix;
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


double coldstartFill(double **removedMatrix, vector<vector<int> > &userGroupDicByAS, 
	vector<vector<int> > &userGroupDicByCountry, vector<vector<int> > &wsGroupDicByAS, 
	vector<vector<int> > &wsGroupDicByCountry, int userId, int wsId, int numUser, 
	int numService)
{
	double predValue = 0;
	int count = 0;
	int i, j;

	// --- return as the average value from userGroupDicByAS
	for (i = 0; i < userGroupDicByAS[userId].size(); i++) {
		int id = userGroupDicByAS[userId][i];
		if (removedMatrix[id][wsId] > 0) {
			predValue += removedMatrix[id][wsId];
			count++;
		}
	}
	if (count > 0) return predValue / count;

	// --- return as the average value from userGroupDicByCountry
	for (i = 0; i < userGroupDicByCountry[userId].size(); i++) {
		int id = userGroupDicByCountry[userId][i];
		if (removedMatrix[id][wsId] > 0) {
			predValue += removedMatrix[id][wsId];
			count++;
		}
	}
	if (count > 0) return predValue / count;

	// --- return as the average value from all users
    for (i = 0; i < numUser; i++) {
    	if (removedMatrix[i][wsId] > 0) {
    		predValue += removedMatrix[i][wsId];
    		count++;
		}
    }
    if (count > 0) return predValue / count;

	// --- return as the average value from wsGroupDicByAS
    for (j = 0; j < wsGroupDicByAS[wsId].size(); j++) {
    	int id = wsGroupDicByAS[wsId][j];
    	if (removedMatrix[userId][id] > 0) {
    		predValue += removedMatrix[userId][id];
    		count++;
		}
    }
    if (count > 0) return predValue / count;

	// --- return as the average value from wsGroupDicByCountry
    for (j = 0; j < wsGroupDicByCountry[wsId].size(); j++) {
    	int id = wsGroupDicByCountry[wsId][j];
    	if (removedMatrix[userId][id] > 0) {
    		predValue += removedMatrix[userId][id];
    		count++;
		}
    }
    if (count > 0) return predValue / count;

	// --- return as the average value from all services
    for (j = 0; j < numService; j++) {
    	if (removedMatrix[userId][j] > 0) {
    		predValue += removedMatrix[userId][j];
    		count++;
		}
    }
    if (count > 0) return predValue / count;
    else return 0;
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


double dotProduct(double *vec1, double *vec2, int len)  
{
	double product = 0;
	int i;
	for (i = 0; i < len; i++) {
		product += vec1[i] * vec2[i];
	}
	return product;
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


void transpose(double **matrix, double **matrix_T, int row, int col) 
{
	int i, j;
	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			matrix_T[j][i] = matrix[i][j];
		}
	}
}


vector<vector<int> > matrix2Dic(double **matrix, int size) 
{
	int i, j;
	vector<vector<int> > dic;
	dic.resize(size);

	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			if (j != i && matrix[i][j] > 0) {
				dic[i].push_back(j);
			}
		}
	}
	return dic;
}


bool cmpPairbyValue(const pair<int, double>& lhs, const pair<int, double>& rhs) {  
  return lhs.second > rhs.second;  
}  
  

vector<pair<int, double> > sortMapByValue(const map<int, double>& pccMap) {
	vector<pair<int, double> > vec(pccMap.begin(), pccMap.end());  
    sort(vec.begin(), vec.end(), cmpPairbyValue); 
	return vec;
}

