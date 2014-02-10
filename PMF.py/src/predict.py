########################################################
# predict.py
# Developer: Jieming Zhu <jmzhu@cse.cuhk.edu.hk>
# Created: 2014/2/6
# Last updated: 2014/2/6
########################################################

import numpy as np 
from numpy import linalg as LA
import time
from PMF import *
from utilities import *


########################################################
# Function to run the PMF method at each density
# 
def predict(matrix, density, paraStruct):

    startTime = time.clock()
    numService = matrix.shape[1] 
    numUser = matrix.shape[0] 
    rounds = paraStruct['rounds']
    logger.info('Data matrix size: %d users * %d services'%(numUser, numService))
    logger.info('Run PMF for %d rounds: matrix density = %.2f.'%(rounds, density))
    evalResults = np.zeros((rounds, len(paraStruct['metrics']))) 
    timeResults = np.zeros((rounds, 1))
    	
    for k in range(rounds):
		logger.info('----------------------------------------------')
		logger.info('%d-round starts.'%(k + 1))
		logger.info('----------------------------------------------')

		# read the training data, i.e., removed matrix
		dataPath = '%s%.2f\\round_%02d_train.txt'\
					%(paraStruct['dataFolder'], density, k + 1)
		logger.info('Load training data: %s'%dataPath)
		trainIndicatorMatrix = np.loadtxt(dataPath)
		trainMatrix = matrix * trainIndicatorMatrix

		# gradient descent
		iterStartTime = time.clock() # to record the running time for one round             
		predictedMatrix = gradDescent(trainMatrix, paraStruct)  # gradient descent 		
		timeResults[k] = time.clock() - iterStartTime

		# calculate the prediction error
		dataPath = '%s%.2f\\round_%02d_test.txt'%(paraStruct['dataFolder'], density, k + 1)
		logger.info('Load testing data: %s'%dataPath)
		testIndicatorMatrix = np.loadtxt(dataPath)
		(testVecX, testVecY) = np.where(testIndicatorMatrix)		
		realVec = matrix[testVecX, testVecY]
		predVec = predictedMatrix[testVecX, testVecY]
      
        # Evaluation metrics caculation
		evalResults[k, :] = errMetric(realVec, predVec, paraStruct['metrics'])

		logger.info('%d-round done. Running time: %.2f sec'%(k + 1, timeResults[k]))
		logger.info('----------------------------------------------')

    outFile = '%s%.2f.txt'%(paraStruct['outPath'], density)
    saveResult(outFile, evalResults, timeResults, paraStruct)
    logger.info('Config density = %.2f done. Running time: %.2f sec'
			%(density, time.clock() - startTime))
    logger.info('==============================================')
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