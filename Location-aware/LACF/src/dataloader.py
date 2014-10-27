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
		matrix = np.where(matrix >= 19.9, -1, matrix)
	elif para['dataType'] == 'tp':
		matrix = np.where(matrix == 0, -1, matrix)
	return matrix
########################################################


#########################################################
# Function to compute the location similarity
#
def getLocGroup(para):
    logger.info('Computing location groups...')
    # Group users by AS and country
    userLocFile = para['dataPath'] + 'userlist.txt'
    data = np.genfromtxt(userLocFile, dtype=np.str, comments='$', 
        delimiter='\t', skip_header=2)
    userCountry = data[:, 2] 
    userAS = data[:, 4]
    numUser = userAS.shape[0]
    userGroupByAS = np.zeros((numUser, numUser))
    userGroupByCountry = np.zeros((numUser, numUser))
    for i in range(numUser):
        for j in range(numUser):
            if i < j:
                userAS_i = userAS[i].split(" ", 1)[0]
                userAS_j = userAS[j].split(" ", 1)[0]
                if (userAS_i == userAS_j) and \
                    (userAS_i != 'null') and \
                    (userAS_j != 'null'):
                    userGroupByAS[i, j] = 1
                    userGroupByAS[j, i] = 1
                if userCountry[i] == userCountry[j]:
                    userGroupByCountry[i, j] = 1
                    userGroupByCountry[j, i] = 1

    # Group services by AS and country
    wsLocFile = para['dataPath'] + 'wslist.txt'
    data = np.genfromtxt(wsLocFile, dtype=np.str, comments='$', 
        delimiter='\t', skip_header=2)
    wsCountry = data[:, 4] 
    wsAS = data[:, 6]
    numService = wsAS.shape[0]
    wsGroupByAS = np.zeros((numService, numService))
    wsGroupByCountry = np.zeros((numService, numService))
    for i in range(numService):
        for j in range(numService):
            if i < j:
                wsAS_i = wsAS[i].split(" ", 1)[0]
                wsAS_j = wsAS[j].split(" ", 1)[0]
                if (wsAS_i == wsAS_j) and \
                    (wsAS_i != 'null') and \
                    (wsAS_j != 'null'):
                    wsGroupByAS[i, j] = 1
                    wsGroupByAS[j, i] = 1
                elif wsCountry[i] == wsCountry[j]:
                    wsGroupByCountry[i, j] = 1
                    wsGroupByCountry[j, i] = 1
    
    return userGroupByAS, userGroupByCountry, wsGroupByAS, wsGroupByCountry
#########################################################