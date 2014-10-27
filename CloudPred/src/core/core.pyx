########################################################
# core.pyx
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/5/6
# Last updated: 2014/5/6
########################################################

import time
import numpy as np
from utilities import *
cimport numpy as np # import C-API


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "NMF.h":
    void NMF_core(double *removedData, int numUser, int numService, 
        int dim, double lmda, int maxIter, double *U, double *S)

cdef extern from "UIPCC.h":
    void UPCC_core(double *removedData, double *UData, double *uMean, int numUser, 
        int numService, int dim, int topK, double *predData)
#########################################################


#########################################################
# Function to perform NMF, as a wrapper of NMF_core
# return the predicted matrix
#
def NMF(removedMatrix, para):  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0] 
    cdef int dim = para['dimension']
    cdef int maxIter = para['maxIter']
    cdef double lmda = para['lambda']

    # initialization
    cdef np.ndarray[double, ndim=2, mode='c'] U = np.random.rand(numUser, dim)        
    cdef np.ndarray[double, ndim=2, mode='c'] S = np.random.rand(numService, dim) 

    # Wrap the NMF_core.cpp
    NMF_core(<double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        numUser,
        numService,
        dim,
        lmda,
        maxIter,
        <double *> U.data,
        <double *> S.data
        )

    return U, S
#########################################################


#########################################################
# Function to perform the prediction algorithm
# Wrap up the C++ implementation
#
def predict(removedMatrix, para):  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0] 
    cdef int dim = para['dimension']
    cdef int maxIter = para['maxIter']
    cdef double lmda = para['lambda']

    # Perform NMF
    (U, S) = NMF(removedMatrix, para)
    logger.info('NMF phase done.')

    # Perform UIPCC          
    uMean = UMEAN(removedMatrix)    
    iMean = IMEAN(removedMatrix)     
    predMatrixUPCC = UPCC(removedMatrix, U, uMean, para)
    logger.info('UPCC phase done.')  
    predMatrixIPCC = IPCC(removedMatrix, S, iMean, para)
    logger.info('IPCC phase done.')
    predMatrixUIPCC = UIPCC(predMatrixUPCC, predMatrixIPCC, para)
    logger.info('UIPCC phase done.')

    return predMatrixUIPCC
#########################################################


#########################################################
# Function to perform UPCC
# return the predicted matrix
#
def UPCC(removedMatrix, U, uMean, para):
    cdef int numUser = removedMatrix.shape[0]
    cdef int numService = removedMatrix.shape[1]
    cdef int topK = para['topK']
    cdef int dim = para['dimension']
    cdef np.ndarray[double, ndim=2, mode='c'] predMatrix = \
        np.zeros((numUser, numService), dtype=np.float64)

    UPCC_core(<double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        <double *> (<np.ndarray[double, ndim=2, mode='c']> U).data, 
        <double *> (<np.ndarray[double, ndim=1, mode='c']> uMean).data, 
        numUser, 
        numService,
        dim, 
        topK, 
        <double *> predMatrix.data)
    return predMatrix
#########################################################


#########################################################
# Function to perform IPCC
# return the predicted matrix
#
def IPCC(removedMatrix, S, iMean, para):
    # use copy() to make sure the transpose makes effect in memory
    predMatrix = UPCC(removedMatrix.T.copy(), S, iMean, para)
    predMatrix = predMatrix.T
    return predMatrix
#########################################################


#########################################################
# Function to perform UIPCC
# return the predicted matrix
#
def UIPCC(predMatrixUPCC, predMatrixIPCC, para)  :
    weight = para['weight']
    predMatrix = weight * predMatrixUPCC + (1 - weight) * predMatrixIPCC
    return predMatrix
#########################################################


#########################################################
# Function to perform UMEAN
# return the average value of users
#
def UMEAN(removedMatrix):
    uMean = IMEAN(removedMatrix.T).T
    return uMean
#########################################################


#########################################################
# Function to perform IMEAN
# return the average value of services
#
def IMEAN(removedMatrix):  
    numUser = removedMatrix.shape[0] 
    idxMatrix = (removedMatrix > 0) 
    columnSum = np.sum(idxMatrix, axis=0)   
    iMean = np.sum(removedMatrix, axis=0) / (columnSum
            + np.spacing(1)) # avoid divide-by-zero
    return iMean
#########################################################

