########################################################
# dataloader.py
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/10/12
########################################################

import numpy as np 
from utilities import *
from scipy.cluster.vq import *


########################################################
# Function to load the dataset
#
def load(para):
	datafile = para['dataPath'] + para['dataType'] + 'Matrix.txt'
	logger.info('Load data: %s'%datafile)
	dataMatrix = np.loadtxt(datafile) 
	dataMatrix = preprocess(dataMatrix, para)
	return dataMatrix
########################################################


########################################################
# Function to preprocess the dataset
# delete the invalid values
# 
def preprocess(matrix, para):
	if para['dataType'] == 'rt':
		matrix = np.where(matrix == 0, -1, matrix)
		matrix = np.where(matrix >= 19.9, -1, matrix)
	elif para['dataType'] == 'tp':
		matrix = np.where(matrix == 0, -1, matrix)
	return matrix
########################################################


#########################################################
# Function to cluster users and services into groups
#
def getLocGroup(para):
    dataPoints = []
    logger.info('Clustering location groups...')

    # --- add users: tag = 0
    userLocFile = para['dataPath'] + 'userlist.txt'
    data = np.genfromtxt(userLocFile, dtype=np.float64, comments='$', 
        delimiter='\t', skip_header=2)
    for i in range(data.shape[0]):
        # get the last two columns and
        # filter the unavailable geo data
        if np.isnan(data[i, 5]) == False\
            and np.isnan(data[i, 6]) == False:
            dataPoints.append([0, i, data[i, 5], data[i, 6]])

    # --- add users: tag = 1
    wsLocFile = para['dataPath'] + 'wslist.txt'
    data = np.genfromtxt(wsLocFile, dtype=np.float64, comments='$', 
        delimiter='\t', skip_header=2)
    for i in range(data.shape[0]):
        # get the last two columns and
        # filter the unavailable geo data
        if np.isnan(data[i, 7]) == False\
            and np.isnan(data[i, 8]) == False:
            dataPoints.append([1, i, data[i, 7], data[i, 8]])

    # --- clustering
    # inital centroids
    initial = [[-30,-50], [-30,150], [25,110], [50,30], [40,-100]]
    initial = np.array(initial)
    dataPoints = np.array(dataPoints)
    (centroids, idx) = kmeans2(dataPoints[:, 2:], initial)

    # only keep two large clusters, ignore the others   
    userGroup1 = []
    userGroup2 = []
    wsGroup1 = []
    wsGroup2 = []
    myiter = 0
    for i in idx:
        # keep cluster[3] and cluster[4]
        if i == 3:
            if dataPoints[myiter, 0] == 0:
                userGroup1.append(dataPoints[myiter, 1])
            else:
                wsGroup1.append(dataPoints[myiter, 1])
        elif i == 4:
            if dataPoints[myiter, 0] == 0:
                userGroup2.append(dataPoints[myiter, 1])
            else:
                wsGroup2.append(dataPoints[myiter, 1])
        myiter += 1
    
    locGroup = (np.array(userGroup1), np.array(wsGroup1), 
        np.array(userGroup2), np.array(wsGroup2))
    return locGroup    
#########################################################