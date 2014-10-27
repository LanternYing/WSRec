/********************************************************
 * UIPCC.cpp
 * C++ implements on PCC and UPCC computations
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/4/29
 * Last updated: 2014/5/8
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


void UPCC_core(double *removedData, double *UData, double *uMean, int numUser, int numService, 
	int dim, int topK, double *predData)
{	
    double **predMatrix = vector2Matrix(predData, numUser, numService);
    double **removedMatrix = vector2Matrix(removedData, numUser, numService);
    double **U = vector2Matrix(UData, numUser, dim);
    
    int i, j; 
    double **pccMatrix = createMatrix(numUser, numUser);

  	for (i = 0; i < numUser; i++) {
  		map<int, double> pccMap;

  		for (j = 0; j < numUser; j++) {
  			if(uMean[i] == 0 || uMean[j] == 0) continue; 
  			double pccValue = 0;
  			if (j > i) {			
				pccValue = getPCC(U[i], U[j], dim);
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
				predValue = predValue/pccSum + uMean[i];
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


double getPCC(double *uA, double *uB, int dim) {
	
	double uA_mean = getAverage(uA, dim);
	double uB_mean = getAverage(uB, dim);

	vector<int> commonIndex;
	int i;
	for (i = 0; i < dim; i++) {
			if(uA[i] > 0 && uB[i] > 0) {
				commonIndex.push_back(i);
			}
	}
	// no enough common indicies
	if(commonIndex.size() < 2) return 0;

	double up = 0;
	double downA = 0;
	double downB = 0;
	for (i = 0; i < commonIndex.size(); i++) {
		int idx = commonIndex[i];
		double tempA = (uA[idx] - uA_mean);
		double tempB = (uB[idx] - uB_mean);
		up += tempA * tempB;
		downA += tempA * tempA;
		downB += tempB * tempB;
	}

	double down = sqrt(downA * downB);

	if(down == 0) return 0;
	double pcc = up / down;

	return pcc;
}


double getAverage(double *vec, int dim)
{
	int i;
	double sum = 0;
	int count = 0;
	for (i = 0; i < dim; i++) {
		if (vec[i] != 0) {
			sum += vec[i];
			count++;
		}		
	}
	if (count == 0) return 0;
	double avg = sum / count;
	return avg;
}
       
  
bool cmpPairbyValue(const pair<int, double>& lhs, const pair<int, double>& rhs) {  
  return lhs.second > rhs.second;  
}  
  

vector<pair<int, double> > sortMapByValue(const map<int, double>& pccMap) {
	vector<pair<int, double> > vec(pccMap.begin(), pccMap.end());  
    sort(vec.begin(), vec.end(), cmpPairbyValue); 
	return vec;
}

