########################################################
# predict.py
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/2/6
########################################################

import numpy as np 
from numpy import linalg as LA
import time
import random
from utilities import *
from NRCF import *


########################################################
# Function to run the prediction approach at each density
# 
def predict(matrix, density, para):

    startTime = time.clock()
    numService = matrix.shape[1] 
    numUser = matrix.shape[0] 
    rounds = para['rounds']
    logger.info('Data matrix size: %d users * %d services'%(numUser, numService))
    logger.info('Run the algorithm for %d rounds: matrix density = %.2f.'%(rounds, density))
    evalResults = np.zeros((rounds, len(para['metrics']))) 
    timeResults = np.zeros((rounds, 1))
    	
    for k in range(rounds):
		logger.info('----------------------------------------------')
		logger.info('%d-round starts.'%(k + 1))
		logger.info('----------------------------------------------')

		# remove the entries of data matrix to generate trainMatrix and testMatrix
		# use k as random seed		
		(trainMatrix, testMatrix) = removeEntries(matrix, density, k) 
		logger.info('Removing data entries done.')
		(testVecX, testVecY) = np.where(testMatrix)		
		testVec = testMatrix[testVecX, testVecY]
		# read the training data, i.e., removed matrix

		# invocation to the prediction function
		iterStartTime = time.clock() # to record the running time for one round             
		predictedMatrix = NRCF_wrapper(trainMatrix, para)  		
		timeResults[k] = time.clock() - iterStartTime

		# calculate the prediction error
		predVec = predictedMatrix[testVecX, testVecY]
		evalResults[k, :] = errMetric(testVec, predVec, para['metrics'])
        
		logger.info('%d-round done. Running time: %.2f sec'%(k + 1, timeResults[k]))
		logger.info('----------------------------------------------')

    outFile = '%s%.2f.txt'%(para['outPath'], density)
    saveResult(outFile, evalResults, timeResults, para)
    logger.info('Config density = %.2f done. Running time: %.2f sec'
			%(density, time.clock() - startTime))
    logger.info('==============================================')
########################################################


########################################################
# Function to remove the entries of data matrix
# Return the trainMatrix and the corresponding testing data
#
def removeEntries(matrix, density, seedID):
	(vecX, vecY) = np.where(matrix > 0)
	vecXY = np.c_[vecX, vecY]
	numRecords = vecX.size
	numAll = matrix.size
	random.seed(seedID)
	randomSequence = range(0, numRecords)
	random.shuffle(randomSequence) # one random sequence per round
	numTrain = int( numAll * density)
	# by default, we set the remaining QoS records as testing data 		               
	numTest = numRecords - numTrain
	trainXY = vecXY[randomSequence[0 : numTrain], :]
	testXY = vecXY[randomSequence[- numTest :], :]

	trainMatrix = np.zeros(matrix.shape)
	trainMatrix[trainXY[:, 0], trainXY[:, 1]] = matrix[trainXY[:, 0], trainXY[:, 1]]
	testMatrix = np.zeros(matrix.shape)
	testMatrix[testXY[:, 0], testXY[:, 1]] = matrix[testXY[:, 0], testXY[:, 1]]

    # ignore invalid testing data
	idxX = (np.sum(trainMatrix, axis=1) == 0)
	testMatrix[idxX, :] = 0
	idxY = (np.sum(trainMatrix, axis=0) == 0)
	testMatrix[:, idxY] = 0    
	return trainMatrix, testMatrix
########################################################


########################################################
# Function to compute the evaluation metrics
# Return an array of metric values
#
def errMetric(testVec, predVec, metrics):
    result = []
    absError = np.absolute(predVec - testVec) 
    mae = np.average(absError)
    for metric in metrics:
	    if 'MAE' == metric:
			result = np.append(result, mae)
	    if 'NMAE' == metric:
		    nmae = mae / np.average(testVec)
		    result = np.append(result, nmae)
	    if 'RMSE' == metric:
	    	rmse = LA.norm(absError) / np.sqrt(absError.size)
	    	result = np.append(result, rmse)
	    if 'MRE' == metric or 'NPRE' == metric:
	        relativeError = absError / testVec
	        if 'MRE' == metric:
		    	mre = np.percentile(relativeError, 50)
		    	result = np.append(result, mre)
	        if 'NPRE' == metric:
		    	npre = np.percentile(relativeError, 90)
		    	result = np.append(result, npre)
    return result
########################################################