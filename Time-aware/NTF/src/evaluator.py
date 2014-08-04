########################################################
# evaluator.py
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/4
########################################################

import numpy as np 
from numpy import linalg as LA
import time
import random
import core
from utilities import *


########################################################
# Function to run the prediction approach at each density
# 
def execute(tensor, density, para):
    startTime = time.clock()
    numService = tensor.shape[1] 
    numUser = tensor.shape[0]
    rounds = para['rounds']
    timeSlice = para['timeSlice']
    logger.info('Data size: %d users * %d services * %d timeslices'\
    	%(numUser, numService, timeSlice))
    logger.info('Run the algorithm for %d rounds: density = %.2f.'%(rounds, density))
    evalResults = np.zeros((rounds, len(para['metrics']), timeSlice)) 
    timeResults = np.zeros((rounds, 1))
    	
    for k in range(rounds):
		logger.info('----------------------------------------------')
		logger.info('%d-round starts.'%(k + 1))
		logger.info('----------------------------------------------')

		# remove the entries of data to generate trainTensor and testTensor
		# use k as random seed	
		(trainTensor, testTensor) = removeTensor(tensor, density, k, para)
		logger.info('Removing data entries done.')

		# invocation to the prediction function
		iterStartTime = time.clock() # to record the running time for one round             
		predictedTensor = core.predict(trainTensor, para)  
		timeResults[k] = time.clock() - iterStartTime

		# calculate the prediction error
		for i in range(timeSlice):
			testMatrix = testTensor[:, :, i]
			predictedMatrix = predictedTensor[:, :, i]
			(testVecX, testVecY) = np.where(testMatrix)		
			testVec = testMatrix[testVecX, testVecY]
			predVec = predictedMatrix[testVecX, testVecY]
			evalResults[k, :, i] = errMetric(testVec, predVec, para['metrics'])

		logger.info('%d-round done. Running time: %.2f sec'%(k + 1, timeResults[k]))
		logger.info('----------------------------------------------')
	
    for i in range(timeSlice):
    	outFile = '%s%02d_%sResult_%.2f.txt'\
    		%(para['outPath'], i + 1, para['dataType'], density)
        saveResult(outFile, evalResults[:, :, i], para)
    if para['saveTimeInfo']:
    	outFile = '%s%sResult_%.2f_time.txt'%(para['outPath'], para['dataType'], density)
        saveTimeResult(outfile, timeResults)

    logger.info('Density = %.2f done. Running time: %.2f sec'
			%(density, time.clock() - startTime))
    logger.info('==============================================')
########################################################


########################################################
# Function to remove the entries of data tensor
# Return the trainTensor and the corresponding testTensor
#
def removeTensor(tensor, density, seedID, para):
	timeSlie = para['timeSlice']
	trainTensor = np.zeros(tensor.shape)
	testTensor = np.zeros(tensor.shape)
	for i in range(timeSlie):
		(trainMatrix, testMatrix) = removeEntries(tensor[:, :, i], density, seedID)
		trainTensor[:, :, i] = trainMatrix
		testTensor[:, :, i] = testMatrix
	return trainTensor, testTensor
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

    # ignore invalid testing             
	idxX = (np.sum(trainMatrix, axis=1) == 0)
	testMatrix[idxX, :] = 0
	idxY = (np.sum(trainMatrix, axis=0) == 0)
	testMatrix[:, idxY] = 0    
	return trainMatrix, testMatrix
########################################################


########################################################
# Function to compute the evaluation metrics
#
def errMetric(realVec, predVec, metrics):
    result = []
    absError = np.abs(predVec - realVec) 
    mae = np.sum(absError)/absError.shape
    for metric in metrics:
	    if 'MAE' == metric:
			result = np.append(result, mae)
	    if 'NMAE' == metric:
		    nmae = mae / (np.sum(realVec) / absError.shape)
		    result = np.append(result, nmae)
	    if 'RMSE' == metric:
		    rmse = LA.norm(absError) / np.sqrt(absError.shape)
		    result = np.append(result, rmse)
	    if 'MRE' == metric or 'NPRE' == metric:
	        relativeError = absError / realVec
	        relativeError = np.sort(relativeError)
	        if 'MRE' == metric:
		    	mre = np.median(relativeError)
		    	result = np.append(result, mre)
	        if 'NPRE' == metric:
		    	npre = relativeError[np.floor(0.9 * relativeError.shape[0])] 
		    	result = np.append(result, npre)
    return result
########################################################