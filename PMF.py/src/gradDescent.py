########################################################
# gradDescent.py
# Developer: Jieming Zhu <jmzhu@cse.cuhk.edu.hk>
# Created: 2014/2/6
# Last updated: 2014/2/6
########################################################

import time
import numpy as np
from numpy import linalg as LA
import matplotlib.pyplot as plt
from utilities import *

#########################################################
# Function to perform iterations by gradient descent
#
def gradDescent(removedMatrix, paraStruct):  
    numService = removedMatrix.shape[1] 
    numUser = removedMatrix.shape[0] 
    dim = paraStruct['dimension']

    # initialization
    U = np.random.rand(dim, numUser)        
    S = np.random.rand(dim, numService) 
    
    iter = 1
    logger.info('Iterating...')
    (lossValue, cost, regulariztion) = loss(U, S, removedMatrix, paraStruct)
    
    while iter <= paraStruct['maxIter']:                        
        (gradU, gradS) = gradLoss(U, S, removedMatrix, paraStruct)    
        eta = linesearch( U, S, removedMatrix, lossValue, gradU, gradS, paraStruct)

        # gradient descent updates 
        U = U - eta * gradU
        S = S - eta * gradS
                   
        (lossValue, cost, regulariztion) = loss(U, S, removedMatrix, paraStruct)

        if paraStruct['debugMode']:
            plt.plot(iter, lossValue, 'b.')
        logger.debug('iter = %03d, totalLoss = %.4f, cost = %.4f, reg = %.4f'
                %(iter, lossValue, cost, regulariztion))
        iter = iter + 1
    if paraStruct['debugMode']:
        plt.show()
    logger.info('Converged after %d iterations'%(iter - 1))
    return U, S
#########################################################


#########################################################
# Function to compute the loss
#
def loss( U, S, removedMatrix, paraStruct ):
    lmd = paraStruct['lambda'] # regularization parameter  
    # loss value computation
    I = np.not_equal(removedMatrix, 0)
    cost = 0.5 * (LA.norm(I * (removedMatrix - np.dot(U.T, S)), 'fro') ** 2)
    regulariztion = 0.5 * lmd * (LA.norm(U, 'fro') ** 2 + LA.norm(S, 'fro') ** 2)
    total = cost + regulariztion
    return total, cost, regulariztion
#########################################################


#########################################################
# Function to compute the gradients of loss function
#
def gradLoss( U, S, removedMatrix, paraStruct ):
    # load the parameters
    dim = paraStruct['dimension'] # dimensionality of coordinates
    lmd = paraStruct['lambda'] # regularization parameter
    numService = removedMatrix.shape[1]
    numUser = removedMatrix.shape[0]

    ## gradients computation
    # gradients of U
    gradU = np.zeros((dim, numUser))
    I = np.not_equal(removedMatrix, 0)

    for i in range(numUser):
        idxS = (I[i, :] == 1)
        S_tmp = S[:, idxS]
        L_tmp = removedMatrix[i, idxS]
        gradU[:, i] = np.dot(S_tmp, (np.dot(U[:, i], S_tmp) - L_tmp))  + lmd * U[:, i]

    # gradients of S
    gradS = np.zeros((dim, numService))
    for j in range(numService):
        idxU = (I[:, j] == 1)
        L_Sj = removedMatrix[idxU, j]
        U_tmp = U[:, idxU]
        gradS[:, j] = np.dot(U_tmp, np.dot(U_tmp.T, S[:, j]) - L_Sj) + lmd * S[:, j]

    return gradU, gradS
#########################################################


#########################################################
# Function to perform line search to find the best learning rate
#
def linesearch( U, S, removedMatrix, lastLossValue, gradU, gradS, paraStruct):
    eta = paraStruct['etaInit']
    for i in range(20):
        U1 = U - eta * gradU
        S1 = S - eta * gradS
        (fValue, cost, reg) = loss( U1 , S1, removedMatrix, paraStruct )

        if fValue <= lastLossValue:
            break
        eta = eta / 2
    return eta
#########################################################


