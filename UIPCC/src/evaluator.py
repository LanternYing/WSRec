########################################################
# evaluator.py
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2015/1/7
########################################################

import numpy as np 
from numpy import linalg as LA
import time
import random
import math
from utilities import *
import core


########################################################
# Function to run the [UMEAN, IMEAN, UPCC, IPCC, UIPCC] 
# methods at each density
# 
def execute(matrix, density, para):
    startTime = time.clock()
    numService = matrix.shape[1] 
    numUser = matrix.shape[0] 
    rounds = para['rounds']
    logger.info('Data matrix size: %d users * %d services'%(numUser, numService))
    logger.info('Run for %d rounds: matrix density = %.2f.'%(rounds, density))
    
    numMetric = 0
    for metric in para['metrics']:
        if isinstance(metric, tuple):
            numMetric += len(metric[1])
        else:
            numMetric += 1
    evalResults = np.zeros((5, rounds, numMetric)) 
    timeResults = np.zeros((5, rounds))
    	
    for k in range(rounds):
		logger.info('----------------------------------------------')
		logger.info('%d-round starts.'%(k + 1))
		logger.info('----------------------------------------------')

		# remove the entries of data matrix to generate trainMatrix and testMatrix		
		(trainMatrix, testMatrix) = removeEntries(matrix, density, k)
		logger.info('Removing data entries done.')

        ## UMEAN
		iterStartTime1 = time.clock()            
		predMatrixUMEAN = core.UMEAN(trainMatrix) 	
		timeResults[0, k] = time.clock() - iterStartTime1
		logger.info('UMEAN done.')

		## IMEAN
		iterStartTime2 = time.clock()          
		predMatrixIMEAN = core.IMEAN(trainMatrix)  	
		timeResults[1, k] = time.clock() - iterStartTime2
		logger.info('IMEAN done.')

		## UPCC
		iterStartTime3 = time.clock()         
		predMatrixUPCC = core.UPCC(trainMatrix, predMatrixUMEAN[:, 0], para)  
		timeResults[2, k] = time.clock() - iterStartTime3 + timeResults[0, k]
		logger.info('UPCC done.')
		
		## IPCC
		iterStartTime4 = time.clock()         
		predMatrixIPCC = core.IPCC(trainMatrix, predMatrixIMEAN[0, :], para) 
		timeResults[3, k] = time.clock() - iterStartTime4 + timeResults[1, k]
		logger.info('IPCC done.')

		## UIPCC
		iterStartTime5 = time.clock()       
		predMatrixUIPCC = core.UIPCC(trainMatrix, predMatrixUPCC, predMatrixIPCC, para)  	
		timeResults[4, k] = time.clock() - iterStartTime5\
				+ timeResults[2, k] + timeResults[3, k]
		logger.info('UIPCC done.')

		## evaluation
		predMatrixUMEAN[trainMatrix > 0] = trainMatrix[trainMatrix > 0]
		evalResults[0, k, :] = errMetric(matrix, testMatrix, predMatrixUMEAN, para['metrics'])
		predMatrixIMEAN[trainMatrix > 0] = trainMatrix[trainMatrix > 0]         
		evalResults[1, k, :] = errMetric(matrix, testMatrix, predMatrixIMEAN, para['metrics'])
		predMatrixUPCC[trainMatrix > 0] = trainMatrix[trainMatrix > 0]  
		evalResults[2, k, :] = errMetric(matrix, testMatrix, predMatrixUPCC, para['metrics'])
		predMatrixIPCC[trainMatrix > 0] = trainMatrix[trainMatrix > 0]
		evalResults[3, k, :] = errMetric(matrix, testMatrix, predMatrixIPCC, para['metrics'])
		predMatrixUIPCC[trainMatrix > 0] = trainMatrix[trainMatrix > 0]          
		evalResults[4, k, :] = errMetric(matrix, testMatrix, predMatrixUIPCC, para['metrics'])

		logger.info('%d-round done. Running time: %.2f sec'
				%(k + 1, time.clock() - iterStartTime1))
		logger.info('----------------------------------------------')

    outFile = '%s%sResult_%.2f.txt'%(para['outPath'], para['dataType'], density)
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
def errMetric(matrix, testMatrix, predMatrix, metrics):
    result = []
    (testVecX, testVecY) = np.where(testMatrix)		
    testVec = testMatrix[testVecX, testVecY]
    predVec = predMatrix[testVecX, testVecY]
    absError = np.absolute(predVec - testVec)
    mae = np.average(absError)
    for metric in metrics:
        if isinstance(metric, tuple):
            if 'NDCG' == metric[0]:
                for topK in metric[1]:
                    ndcg_k = getNDCG(matrix, predMatrix, topK)
                    result = np.append(result, ndcg_k)
        elif 'MAE' == metric:
			result = np.append(result, mae)
        elif 'NMAE' == metric:
		    nmae = mae / np.average(testVec)
		    result = np.append(result, nmae)
        elif 'RMSE' == metric:
	    	rmse = LA.norm(absError) / np.sqrt(absError.size)
	    	result = np.append(result, rmse)
        elif 'MRE' == metric or 'NPRE' == metric:
	        relativeError = absError / testVec
	        if 'MRE' == metric:
		    	mre = np.percentile(relativeError, 50)
		    	result = np.append(result, mre)
	        elif 'NPRE' == metric:
		    	npre = np.percentile(relativeError, 90)
		    	result = np.append(result, npre)
    return result
########################################################


########################################################
# Function to compute the NDCG metric
#
def getNDCG(matrix, predMatrix, topK):
	numUser = matrix.shape[0]
	numService = matrix.shape[1]
	ndcg = 0
	for uid in range(numUser):
		realVec = matrix[uid, :]
		predictVec = predMatrix[uid, :]
		predictVecIdx = np.argsort(-predictVec)
		updatedPredictVec = realVec[predictVecIdx]
		# filter out the invalid values (-1)
		updatedPredictVec = updatedPredictVec[updatedPredictVec > 0]
		updatedRealVec = realVec[realVec > 0]
		updatedRealVec = sorted(updatedRealVec, reverse=True)
		
		dcg_k = 0
		idcg_k = 0
		for j in range(min(topK, len(updatedRealVec))):
			if (j == 0):
				dcg_k = dcg_k + updatedPredictVec[0]
				idcg_k = idcg_k + updatedRealVec[0]
			else:
				dcg_k = dcg_k + updatedPredictVec[j] / np.log2(j + 1)
				idcg_k = idcg_k + updatedRealVec[j] / np.log2(j + 1)
		ndcg_k = dcg_k / (idcg_k + np.spacing(1))
		ndcg += ndcg_k
	return ndcg / numUser
########################################################