#########################################################
# NRCF.pyx
# Author: Yifei Lu <koplyf@gmail.com>, Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/7/16
#########################################################

import numpy as np
from utilities import *
cimport numpy as np # import C-API


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "NRCF_core.h":
    void U_NRCF_core(double *removedData, double *uMax, double *uMean, 
        int numUser, int numService, int topK, double *predData)
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
# Function to perform IMAX
# 
def IMAX(removedMatrix):
    iMax = UMAX(removedMatrix.T)
    return iMax
#########################################################


#########################################################
# Function to perform UMAX
#
def UMAX(removedMatrix):
    uMax = np.max(removedMatrix, axis=1)
    return uMax
#########################################################


#########################################################
# Function to perform U_NRCF
# return the predicted matrix
#
def U_NRCF(removedMatrix, uMax, uMean, para):
     cdef int numUser = removedMatrix.shape[0]
     cdef int numService = removedMatrix.shape[1]
     cdef int topK = para['topK']
     cdef np.ndarray[double, ndim=2, mode='c'] predMatrix = \
         np.zeros((numUser, numService), dtype=np.float64)
     U_NRCF_core(
        <double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
		<double *> (<np.ndarray[double, ndim=1, mode='c']> uMax).data,
        <double *> (<np.ndarray[double, ndim=1, mode='c']> uMean).data,
    	numUser,
    	numService,
    	topK,
    	<double *> predMatrix.data)
     return predMatrix
#########################################################


#########################################################
# Function to perform I_NRCF
# return the predicted matrix
#
def I_NRCF(removedMatrix, iMax, iMean, para):
    # use copy() to make sure the transpose makes effect in memory
    predMatrix = U_NRCF(removedMatrix.T.copy(), iMax, iMean, para)
    predMatrix = predMatrix.T
    return predMatrix
#########################################################


#########################################################
# Function to perform NRCF
# return the predicted matrix
#
def NRCF_wrapper(removedMatrix, para):
    lmd = para['lambda']
    (uMax, iMax, uMean, iMean) = (UMAX(removedMatrix), IMAX(removedMatrix), 
        UMEAN(removedMatrix), IMEAN(removedMatrix))
    predMatrixUPCC = U_NRCF(removedMatrix, uMax, uMean, para)
    logger.info('User-based NRCF done.')
    predMatrixIPCC = I_NRCF(removedMatrix, iMax, iMean, para)
    logger.info('Item-based NRCF done.')	
    predMatrix = lmd * predMatrixUPCC + (1 - lmd) * predMatrixIPCC
    logger.info('Hybrid NRCF done.')
    return predMatrix
#########################################################


