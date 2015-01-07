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



