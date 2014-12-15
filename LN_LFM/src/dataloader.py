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
# Function to load the service provider and country information
#
def loadServInfo(para):
    wsLocFile = para['dataPath'] + 'wslist.txt'
    data = np.genfromtxt(wsLocFile, dtype=np.str, comments='$', 
    	delimiter='\t', skip_header=2)
    wsProvider = data[:, 2]
    wsCountry = data[:, 4]
    providerSet = set(wsProvider)
    countrySet = set(wsCountry)
    providerDict = {}
    countryDict = {}
    cnt = 0
    for provider in providerSet:
        providerDict[provider] = cnt
        cnt += 1
    cnt = 0
    for country in countrySet:
        countryDict[country] = cnt
        cnt += 1
        
    wsInfoList = np.zeros((data.shape[0], 2))
    for i in xrange(data.shape[0]):
        wsInfoList[i, :] = [providerDict[wsProvider[i]], countryDict[wsCountry[i]]]
    return wsInfoList
#########################################################