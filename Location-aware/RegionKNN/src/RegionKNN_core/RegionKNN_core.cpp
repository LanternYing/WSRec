/********************************************************
 * RegionKNN_core.cpp
 * C++ implements on RegionKNN [Chen et al, ICWS'2010]
 * Author: Jamie Zhu <jimzhu@GitHub>
 * Created: 2014/5/6
 * Last updated: 2014/5/26
********************************************************/

#include <iostream>
#include <cstring>
#include <map>
#include <vector>
#include <utility>
#include <algorithm> 
#include <cmath>
#include "RegionKNN_core.h"
using namespace std;


/********************************************************
 * predData is the output variable
********************************************************/
void RegionKNN_core(double *removedData, double *initUserRegionData, int numUser, int numService, 
	int numInitUserRegion, int topK, double lmda, double mu_u, double *predData)
{	
	// --- transform the 1D pointer to 2D array pointer
    double **removedMatrix = vector2Matrix(removedData, numUser, numService);
    double **predMatrix = vector2Matrix(predData, numUser, numService);
    double **initUserRegion = vector2Matrix(initUserRegionData, numInitUserRegion, numUser);

    // --- transform user and service group matrix into dictionary (data structures)
    vector<vector<int> > userRegionDic = matrix2Dic(initUserRegion, numInitUserRegion, numUser);

	// --- create temporal matrices 
 	double **removedMatrix_T = createZeroMatrix(numService, numUser);
	transpose(removedMatrix, removedMatrix_T, numUser, numService);

    // --- create user regions
	double **userRegionCenter = createZeroMatrix(numInitUserRegion, numService);
	double **sensitiveServiceFlag = createZeroMatrix(numInitUserRegion, numService);
    vector<int> userRegionIdx = createUserRegion(userRegionDic, removedMatrix, userRegionCenter, 
    	sensitiveServiceFlag, numUser, numService, numInitUserRegion, lmda, mu_u);
    int numUserRegion = userRegionIdx.size();
    map<int, int> userRegion; // store the region idx for each user
    int i, j;
    for (i = 0; i < numUserRegion; i++) {
    	int id = userRegionIdx[i];
		for (j = 0; j < userRegionDic[id].size(); j++) {
			userRegion[userRegionDic[id][j]] = id;
		}
	}

    // --- UPCC
	double **UPCCMatrix = UPCC(removedMatrix, userRegion, userRegionIdx, userRegionCenter, 
		sensitiveServiceFlag, numUser, numService, numInitUserRegion, numUserRegion, topK);

    // get the global average
    int count = 0;
    double allMean = 0;
    for (i = 0; i < numUser; i++) {
    	for (j = 0; j < numService; j++) {
    		if (removedMatrix[i][j] > 0) {
    			allMean += removedMatrix[i][j];
    			count++;
    		}
    	}
    }
    if (count > 0) {
    	allMean = allMean / count;
    }

    for (i = 0; i < numUser; i++) {
    	for (j = 0; j < numService; j++) {
    		if (UPCCMatrix[i][j] != 0) {
				predMatrix[i][j] = UPCCMatrix[i][j];
			}
			else {
				double uMean = getAverage(removedMatrix[i], numService);
				double iMean = getAverage(removedMatrix_T[j], numUser);
				if (uMean != 0 && iMean != 0) {
					predMatrix[i][j] = (uMean + iMean) / 2;
				}
				else if (uMean == 0 && iMean != 0) {
					predMatrix[i][j] = iMean;
				}
				else if (uMean != 0 && iMean == 0) {
					predMatrix[i][j] = uMean;
				}
				else {
					predMatrix[i][j] = allMean;
				}
			}
    	}
    }

    deleteMatrix(userRegionCenter);
    deleteMatrix(sensitiveServiceFlag);
    deleteMatrix(UPCCMatrix);
    deleteMatrix(removedMatrix_T);
    delete ((char*) initUserRegion);
    delete ((char*) removedMatrix);
    delete ((char*) predMatrix);
}


vector<int> createUserRegion(vector<vector<int> > &userRegionDic, double **removedMatrix, 
	double **userRegionCenter, double **sensitiveServiceFlag, int numUser, int numService, 
	int numInitUserRegion, double lmda, double mu_u)
{
	vector<int> regionIdx;
	double **simMatrix = createZeroMatrix(numInitUserRegion, numInitUserRegion);
	// region flags: -1 as the deleted region, +1 as the remained region, 0 as the region
	// to be aggregated.
	double *regionFlag = createVector(numInitUserRegion);
	double *regionMean = createVector(numInitUserRegion);

	// --- initialization
	int i, j, k;
	for (i = 0; i < numInitUserRegion; i++) {
		double sensitivity = 0;
		for (j = 0; j < numService; j++) {
			vector<double> region;
			for (k = 0; k < userRegionDic[i].size(); k++) {
				int uid = userRegionDic[i][k];
				if (removedMatrix[uid][j] > 0) {
					region.push_back(removedMatrix[uid][j]);
				}
			}
			if (region.empty() == false) {
				userRegionCenter[i][j] = median(region);
				if (isSensitiveService(region, userRegionCenter[i][j])) {
					sensitiveServiceFlag[i][j] = 1;
					sensitivity++;
				}
			}
		}
		if (sensitivity / numService > lmda) {
			// remove sensitive region from the aggregation
			regionFlag[i] = 1;
		}
	}

	for (i = 0; i < numInitUserRegion; i++) {
		regionMean[i] = getAverage(userRegionCenter[i], numService);
	}
	// similarity computation
	for (i = 0; i < numInitUserRegion - 1; i++) {
		if (regionFlag[i] != 0) continue;
		for (j = i + 1; j < numInitUserRegion; j++) {
			if (regionFlag[j] != 0) continue;
			simMatrix[i][j] = getPCC(userRegionCenter[i], userRegionCenter[j], regionMean[i], 
				regionMean[j], numService);
		}
	}

	// --- region aggreagtaion
	bool stop = false;
	while (true) {
		// find the most similar pair
		double max = 0;
		int idx = 0, idy = 0;
		for (i = 0; i < numInitUserRegion - 1; i++) {
			if (regionFlag[i] != 0) continue;
			for (j = i + 1; j < numInitUserRegion; j++) {
				if (regionFlag[j] != 0) continue;
				if (simMatrix[i][j] > max) {
					idx = i;
					idy = j;
					max = simMatrix[i][j];
				}
			}
		}

		if (idx == 0 && idy == 0) stop = true;

		// aggregate two regions
		if (max > mu_u) {
			for (j = 0; j < userRegionDic[idy].size(); j++) {
				userRegionDic[idx].push_back(userRegionDic[idy][j]);
			}
			regionFlag[idy] = -1; // delete region idy
		}
		else stop = true;

		if (stop == true) {
			for (i = 0; i < numInitUserRegion; i++) {
				// flag = 1 is the remained region
				if (regionFlag[i] != -1) {
					regionIdx.push_back(i);
				}
			}
			break;
		}

		// update the new region
		double sensitivity = 0;
		for (j = 0; j < numService; j++) {
			vector<double> region;
			for (k = 0; k < userRegionDic[idx].size(); k++) {
				int uid = userRegionDic[idx][k];
				if (removedMatrix[uid][j] > 0) {
					region.push_back(removedMatrix[uid][j]);
				}
			}

			if (region.empty() == false) {
				userRegionCenter[idx][j] = median(region);
				if (isSensitiveService(region, userRegionCenter[idx][j])) {
					sensitiveServiceFlag[idx][j] = 1;
					sensitivity++;
				}
				else {
					sensitiveServiceFlag[idx][j] = 0;
				}
			}
		}
		if (sensitivity / numService > lmda) {
			// remove sensitive region from the aggregation
			regionFlag[idx] = 1;
		}

		// similarity computation
		if (regionFlag[idx] == 0) {
			for (j = 0; j < numInitUserRegion; j++) {
				if (regionFlag[j] != 0 || j == idx) continue;
				regionMean[idx] = getAverage(userRegionCenter[idx], numService);
				if (j < idx) {
					simMatrix[j][idx] = getPCC(userRegionCenter[idx], userRegionCenter[j], 
						regionMean[idx], regionMean[j], numService);
				}
				else if (j > idx) {
					simMatrix[idx][j] = getPCC(userRegionCenter[idx], userRegionCenter[j], 
						regionMean[idx], regionMean[j], numService);
				}
			}
		}
	}

	deleteMatrix(simMatrix);
	deleteVector(regionFlag);
	deleteVector(regionMean);
	return regionIdx;
}


double **UPCC(double **removedMatrix, map<int, int> &userRegion, vector<int> &regionIdx, 
	double **userRegionCenter, double **sensitiveServiceFlag, int numUser, int numService, 
	int numInitUserRegion, int numUserRegion, int topK)
{
    double **predMatrix = createZeroMatrix(numUser, numService);
	double *regionMean = createVector(numInitUserRegion);
	double *uMean = createVector(numUser);

	int i, j, g;
	for (g = 0; g < numUserRegion; g++) {
		int id = regionIdx[g];
		regionMean[id] = getAverage(userRegionCenter[id], numService);
	}

	for (i = 0; i < numUser; i++) {
		int regionID = userRegion[i];
		uMean[i] = getAverage(removedMatrix[i], numService);

		map<int, double> pccMap;
		for (g = 0; g < numUserRegion; g++) {
			int id = regionIdx[g];
  			double pccValue = getPCC(removedMatrix[i], userRegionCenter[id], uMean[i], 
  				regionMean[id], numService);
			// find similar users
			if (pccValue > 0) pccMap[id] = pccValue;
  		}

  		if (!pccMap.empty()) {
	  		vector<pair<int, double> > sortedPccMap = sortMapByValue(pccMap);
	  		// predict the value for each entry 
			for (j = 0; j < numService; j++) {
				if(removedMatrix[i][j] != 0) continue; // Skip the training data

				if (sensitiveServiceFlag[regionID][j] == 1 && userRegionCenter[regionID][j] != 0) {
					predMatrix[i][j] = userRegionCenter[regionID][j];
				}
				else {
					int k = 0;
					double pccSum = 0; 
					double predValue1 = 0;
					double predValue2 = 0;
					vector<pair<int, double> >::iterator it = sortedPccMap.begin();
					while (k < topK && it != sortedPccMap.end()) {
						int id = it->first;
						double pcc = it->second;
						it++;
						// if the similar user does not use this item previously, can not be used. 
						if (userRegionCenter[id][j] == 0) continue;				
						pccSum += pcc;
						k++;
						predValue1 += pcc * (userRegionCenter[id][j] - regionMean[id]);
						predValue2 += pcc * userRegionCenter[id][j];
					}

					// if pccSum == 0, ignore it.
					double predValue = 0;
					if (pccSum > 0) {
						if (uMean[i] != 0) {
							predValue = predValue1 / pccSum + uMean[i];
						}
						else if (userRegionCenter[regionID][j] != 0) {
							predValue = predValue1 / pccSum + userRegionCenter[regionID][j];						
						}
						else {
							predValue = predValue2 / pccSum;
						}
					}

					// negative value, ignore it. 
					if (predValue > 0) {
						predMatrix[i][j] = predValue;
					}
				}
			}
		}		
	}

	deleteVector(regionMean);
	deleteVector(uMean);
	return predMatrix;
}


double getPCC(double *uA, double *uB, double uA_mean, double uB_mean, int size)
{
	vector<int> commonIndex;
	int uA_size = 0;
	int uB_size = 0;
	int i;
	for (i = 0; i < size; i++) {
			if (uA[i] > 0) {
				uA_size++;
			}
			if (uB[i] > 0) {
				uB_size++;
			}
			if (uA[i] > 0 && uB[i] > 0) {
				commonIndex.push_back(i);
			}
	}
	// no enough common indicies
	if (commonIndex.size() < 2) return 0;

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
	pcc = pcc * 2 * commonIndex.size() / (uA_size + uB_size);
	if (pcc < 0) pcc = 0;

	return pcc;
}


double getAverage(double *vec, int size)
{
	int i;
	double sum = 0;
	int count = 0;
	for (i = 0; i < size; i++) {
		if (vec[i] != 0) {
			sum += vec[i];
			count++;
		}		
	}
	if (count == 0) return 0;
	double avg = sum / count;
	return avg;
}


bool isSensitiveService(vector<double> &region, double mu)
{
	vector<double> offset;

	int i;
	for (i = 0; i < region.size(); i++) {
		offset.push_back(fabs(region[i] - mu));
	}
	double sigma = median(offset);

	// if any value is greater than mu + 3*sigma, it is sensitive servie
	if (region.size() > 2) {
		for (i = 0; i < region.size(); i++) {
			if (region[i] > mu + 3 * sigma) {
				return true;
			}
		}
	}

	return false;
}


double median(vector<double> &vec)
{
	size_t mid = vec.size() / 2;
    nth_element(vec.begin(), vec.begin() + mid, vec.end());
	if (vec.size() % 2 == 0) {
		return (vec[mid - 1] + vec[mid]) / 2;
	}
	else {
    	return vec[mid];
    }
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


void transpose(double **matrix, double **matrix_T, int row, int col) 
{
	int i, j;
	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			matrix_T[j][i] = matrix[i][j];
		}
	}
}


vector<vector<int> > matrix2Dic(double **matrix, int row, int col) 
{
	int i, j;
	vector<vector<int> > dic;
	dic.resize(row);

	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			if (matrix[i][j] > 0) {
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

