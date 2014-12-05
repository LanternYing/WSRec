#########################################################
# core.pyx
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/7/16
#########################################################

import numpy as np
from utilities import *
cimport numpy as np # import C-API


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "ADF.h":
    void SIM(double *removedData, double *uMean, int numUser, 
    	int numService, int topK, double *simData, int type)
    void PRED(double *removedData, double *removedRData, 
    	double *uMean, int numUser, int numService, int topK, 
    	double *predData, double *simData)
#########################################################


#########################################################
# Function to perform the prediction algorithm
# Wrap up the C++ implementation
#
def predict(removedMatrix, para):
    lmd = para['lambda']
    uMean = UMEAN(removedMatrix)
    iMean = IMEAN(removedMatrix)
    uSim = SIMU(removedMatrix, para, 0)
    logger.info('User-based simularity computation done.')
    iSim = SIMU(removedMatrix.T.copy(), para, 1)
    logger.info('Item-based simularity computation done.')
    removedMatrixT = removedMatrix.copy()
    data_smoothing(removedMatrixT,uSim,para)
    logger.info('Data smoothing done.')
    predMatrixUADF = PRD(removedMatrixT, removedMatrix, uSim, uMean, para)   
    logger.info('User-based prediction done.')
    predMatrixIADF = PRD(removedMatrixT.T.copy(), removedMatrix.T.copy(), 
    	iSim, iMean, para)
    predMatrixIADF = predMatrixIADF.T
    logger.info('Item-based prediction done.')	
    predMatrix = lmd * predMatrixUADF + (1 - lmd) * predMatrixIADF
    logger.info('ADF done.')
    return predMatrix
#########################################################


#########################################################
# Function to perform UMEAN
# 
def UMEAN(removedMatrix):
    predMatrix = IMEAN(removedMatrix.T).T
    return predMatrix
#########################################################


#########################################################
# Function to perform IMEAN
# return the predicted matrix
#
def IMEAN(removedMatrix):
    numUser = removedMatrix.shape[0]
    idxMatrix = (removedMatrix > 0)
    columnSum = np.sum(idxMatrix, axis=0)
    imean = np.sum(removedMatrix, axis=0) / (columnSum
            + np.spacing(1)) # avoid divide-by-zero
    predMatrix = np.tile(imean, (numUser, 1))
    return predMatrix
#########################################################


#########################################################
# Function to compute simularity
# return the simularity matrix
#
def SIMU(removedMatrix, para, ADFtype):
    uMean = UMEAN(removedMatrix)
    cdef int numUser = removedMatrix.shape[0]
    cdef int numService = removedMatrix.shape[1]
    cdef int topK = para['topK']
    cdef int ADFType = ADFtype      
    cdef np.ndarray[double, ndim=2, mode='c'] simMatrix = \
        np.zeros((numUser, numUser), dtype=np.float64)
    SIM(<double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        <double *> (<np.ndarray[double, ndim=1, mode='c']> uMean).data,
    	numUser,
    	numService,
    	topK,
        <double *> simMatrix.data,
        ADFType)
    for i in xrange(simMatrix.shape[0]):
        simMatrix[i][i] = 1
    return simMatrix
#########################################################


#########################################################
# Function to perform user-based ADF
# return the predicted matrix
#   
def PRD(removedMatrix, removedRMatrix, simMatrix, Mean, para):
    cdef int numUser = removedMatrix.shape[0]
    cdef int numService = removedMatrix.shape[1]
    cdef int topK = para['topK']
    cdef np.ndarray[double, ndim=2, mode='c'] predMatrix = \
        np.zeros((numUser, numService), dtype=np.float64)
    PRED(<double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        <double *> (<np.ndarray[double, ndim=2, mode='c']> removedRMatrix).data,
        <double *> (<np.ndarray[double, ndim=1, mode='c']> Mean).data,
    	numUser,
    	numService,
    	topK,
        <double *> predMatrix.data,
        <double *> (<np.ndarray[double, ndim=2, mode='c']> simMatrix).data)
    return predMatrix  
#########################################################


#########################################################
# Function to perform data smoothing
#
def data_smoothing(removedMatrix, simMatrix, para):
    from scipy.cluster.vq import vq, kmeans2, whiten
    randidx = range(329)
    np.random.shuffle(randidx)
    numC = para['numCluster']    
    [ClusUser, idx] = kmeans2(simMatrix, simMatrix[randidx[0:numC],:])
    uMean = UMEAN(removedMatrix)[0]
    [numUser, numService] = removedMatrix.shape
    for i in xrange(numUser):
        mean = uMean[i]
        for j in xrange(numService):
            if(removedMatrix[i][j]==0):
                tmp = np.where(idx == idx[i])
                tmpa = np.where(removedMatrix[:,j] > 0)
                tmpb = np.setxor1d(tmp[0],tmpa[0])
                down = tmpb.size
                upAll = np.sum(removedMatrix[tmpb,j] - uMean[tmpb])
                removedMatrix[i][j] = mean + upAll / (down + np.spacing(1))
#########################################################