########################################################
# dataloader.py
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/10/12
########################################################

import numpy as np 
from utilities import *


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
		matrix = np.where(matrix >= 20, -1, matrix)
	elif para['dataType'] == 'tp':
		matrix = np.where(matrix == 0, -1, matrix)
	return matrix
########################################################


#########################################################
# Function to get the inital user regions
#
def getInitalRegion(para): 
    # get the users in a city into a dictionary 
    userRegionDic = {}
    userLocFile = para['dataPath'] + 'userlist.txt'
    data = np.genfromtxt(userLocFile, dtype=np.str, comments='$', 
        delimiter='\t', skip_header=2)
    userCountry = data[:, 2]
    numUser = data.shape[0]
    for i in range(numUser):
        key = userCountry[i]
        if key in userRegionDic:
            userRegionDic[key].append(i)
        else:
            userRegionDic[key] = [i]

    # transform the dictionary into a array
    initUserRegion = np.zeros((len(userRegionDic), numUser))
    i = 0        
    for key in userRegionDic:
        for value in userRegionDic[key]:
            initUserRegion[i, value] = 1
        i = i + 1
    
    return initUserRegion
#########################################################
