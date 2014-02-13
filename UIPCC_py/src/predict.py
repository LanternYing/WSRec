########################################################
# predict.py
# Developer: Jieming Zhu <jmzhu@cse.cuhk.edu.hk>
# Created: 2014/2/6
# Last updated: 2014/2/6
########################################################
import sys
import numpy as np 
from numpy import linalg as LA
import time
from utilities import *
from UIPCC import *


########################################################
# Function to run the [UMEAN, IMEAN, UPCC, IPCC, UIPCC] 
# methods at each density
# 
def predict(matrix, density, para):

    startTime = time.clock()
    numService = matrix.shape[1] 
    numUser = matrix.shape[0] 
    rounds = para['rounds']
    logger.info('Data matrix size: %d users * %d services'%(numUser, numService))
    logger.info('Run for %d rounds: matrix density = %.2f.'%(rounds, density))
    evalResults = np.zeros((5, rounds, len(para['metrics']))) 
    timeResults = np.zeros((5, rounds))
    	
    for k in range(rounds):
		logger.info('----------------------------------------------')
		logger.info('%d-round starts.'%(k + 1))
		logger.info('----------------------------------------------')

		# load the training data, i.e., removed matrix
		dataPath = '%s%.2f\\round_%02d_train.txt'\
					%(para['dataFolder'], density, k + 1)
		logger.info('Load training data: %s'%dataPath)
		trainIndicatorMatrix = np.loadtxt(dataPath)
		trainMatrix = matrix * trainIndicatorMatrix

		# load the testing data
		dataPath = '%s%.2f\\round_%02d_test.txt'%(para['dataFolder'], density, k + 1)
		logger.info('Load testing data: %s'%dataPath)
		testIndicatorMatrix = np.loadtxt(dataPath)
		(testVecX, testVecY) = np.where(testIndicatorMatrix)		
		realVec = matrix[testVecX, testVecY]

		## UMEAN
		iterStartTime1 = time.clock()            
		predMatrixUMEAN = UMEAN(trainMatrix) 	
		timeResults[0, k] = time.clock() - iterStartTime1
		predVecUMEAN = predMatrixUMEAN[testVecX, testVecY]       
		evalResults[0, k, :] = errMetric(realVec, predVecUMEAN, para['metrics'])
		logger.info('UMEAN done.')

		## IMEAN
		iterStartTime2 = time.clock()          
		predMatrixIMEAN = IMEAN(trainMatrix)  	
		timeResults[1, k] = time.clock() - iterStartTime2
		predVecIMEAN = predMatrixIMEAN[testVecX, testVecY]         
		evalResults[1, k, :] = errMetric(realVec, predVecIMEAN, para['metrics'])
		logger.info('IMEAN done.')

		## UPCC
		iterStartTime3 = time.clock()         
		predMatrixUPCC = UPCC(trainMatrix, predMatrixUMEAN, para)  
		timeResults[2, k] = time.clock() - iterStartTime3 + timeResults[0, k]
		predVecUPCC = predMatrixUPCC[testVecX, testVecY]   
		evalResults[2, k, :] = errMetric(realVec, predVecUPCC, para['metrics'])
		logger.info('UPCC done.')
		
		## IPCC
		iterStartTime4 = time.clock()         
		predMatrixIPCC = IPCC(trainMatrix, predMatrixIMEAN, para) 
		timeResults[3, k] = time.clock() - iterStartTime4 + timeResults[1, k]
		predVecIPCC = predMatrixIPCC[testVecX, testVecY]        
		evalResults[3, k, :] = errMetric(realVec, predVecIPCC, para['metrics'])
		logger.info('IPCC done.')

		## UIPCC
		iterStartTime5 = time.clock()       
		predMatrixUIPCC = UIPCC(trainMatrix, predMatrixUPCC, predMatrixIPCC, para)  	
		timeResults[4, k] = time.clock() - iterStartTime5\
				+ timeResults[2, k] + timeResults[3, k]
		predVecUIPCC = predMatrixUIPCC[testVecX, testVecY]           
		evalResults[4, k, :] = errMetric(realVec, predVecUIPCC, para['metrics'])
		logger.info('UIPCC done.')

		logger.info('%d-round done. Running time: %.2f sec'
				%(k + 1, time.clock() - iterStartTime1))
		logger.info('----------------------------------------------')

    outFile = '%s%.2f.txt'%(para['outPath'], density)
    saveResult(outFile, evalResults, timeResults, para)
    logger.info('Config density = %.2f done. Running time: %.2f sec'
			%(density, time.clock() - startTime))
    logger.info('==============================================')
########################################################


########################################################
# Function to compute the evaluation metrics
# Return an array of metric values
#
def errMetric(realVec, predVec, metrics):
    result = []
    absError = np.absolute(predVec - realVec) 
    mae = np.average(absError)
    for metric in metrics:
	    if 'MAE' == metric:
			result = np.append(result, mae)
	    if 'NMAE' == metric:
		    nmae = mae / np.average(realVec)
		    result = np.append(result, nmae)
	    if 'RMSE' == metric:
	    	rmse = LA.norm(absError) / np.sqrt(absError.size)
	    	result = np.append(result, rmse)
	    if 'MRE' == metric or 'NPRE' == metric:
	        relativeError = absError / realVec
	        if 'MRE' == metric:
		    	mre = np.percentile(relativeError, 50)
		    	result = np.append(result, mre)
	        if 'NPRE' == metric:
		    	npre = np.percentile(relativeError, 90)
		    	result = np.append(result, npre)
    return result
########################################################