########################################################
# dataloader.py
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/7/20
########################################################

import numpy as np 
from utilities import *


########################################################
# Function to load the dataset
#
def load(para):
	for timeslice in range(para['timeSlice']):		
		datafile = '%sprocessedData/%sData/%sTimeSlot%02d.txt'\
			%(para['dataPath'], para['dataType'], para['dataType'], timeslice + 1)
		logger.info('Load data: %s'%datafile)
		dataMatrix = np.loadtxt(datafile)
		if timeslice == 0:
			dataTensor = dataMatrix
		else:
			dataTensor = np.dstack((dataTensor, dataMatrix))
	dataTensor = preprocess(dataTensor, para)
	return dataTensor
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
