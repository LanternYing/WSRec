########################################################
# NMF.py
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/5/6
# Last updated: 2014/5/7
########################################################

import time
import numpy as np
from utilities import *
import matplotlib.pyplot as plt
from numpy import linalg as LA


#########################################################
# Function to perform NMF
# return the predicted matrix
#
def NMF(removedMatrix, paraStruct):  
    numService = removedMatrix.shape[1] 
    numUser = removedMatrix.shape[0] 
    dim = paraStruct['dimension']
    maxIter = paraStruct['maxIter']
    lmd = paraStruct['lambda']

    idxMatrix = (removedMatrix > 0)

    # initialization
    U = np.random.rand(numUser, dim)
    S = np.random.rand(numService, dim)

    logger.info('Iterating...')

    for iter in range(maxIter):
		U = U * (np.dot(removedMatrix, S))\
				/ (np.dot(idxMatrix * np.dot(U, S.T), S) + lmd * U + np.spacing(1))
		S = S * (np.dot(removedMatrix.T, U))\
				/ (np.dot((idxMatrix * np.dot(U, S.T)).T, U)  + lmd * S + np.spacing(1))	

		if paraStruct['debugMode']:
			(lossValue, cost, regulariztion) = loss(U, S, removedMatrix, paraStruct)
			plt.plot(iter, lossValue, 'b.')
			logger.debug('iter = %03d, totalLoss = %.4f, cost = %.4f, reg = %.4f'
			    %(iter, lossValue, cost, regulariztion))

    if paraStruct['debugMode']:
        plt.show()
    logger.info('Converged after %d iterations'%(iter))
    predMatrix = np.dot(U, S.T)    
    return predMatrix
#########################################################


#########################################################
# Function to compute the loss
#
def loss( U, S, removedMatrix, paraStruct ):
    lmd = paraStruct['lambda'] # regularization parameter  
    # loss value computation
    I = np.not_equal(removedMatrix, 0)
    cost = 0.5 * (LA.norm(I * (removedMatrix - np.dot(U, S.T)), 'fro') ** 2)
    regulariztion = 0.5 * lmd * (LA.norm(U, 'fro') ** 2 + LA.norm(S, 'fro') ** 2)
    lossValue = cost + regulariztion
    return lossValue, cost, regulariztion
#########################################################




