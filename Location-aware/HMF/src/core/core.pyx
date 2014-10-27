########################################################
# core.pyx
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/20
########################################################

import time
import sys
import numpy as np
from utilities import *
cimport numpy as np # import C-API


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "HMF.h":
    void HMF(double *removedData, double *userGroup1, double *wsGroup1,
        double *userGroup2, double *wsGroup2, int numUser, int numService, 
        int numUser1, int numService1, int numUser2, int numService2, int dim, 
        double lmda, double lmda1, double lmda2, double alpha, int maxIter, 
        double etaInit, double *predData)
#########################################################


#########################################################
# Function to perform the prediction algorithm
# Wrap up the C++ implementation
#
def predict(removedMatrix, locGroup, para):
    # get the parameters  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0] 
    cdef int dim = para['dimension']
    cdef double lmda = para['lambda']
    cdef double lmda1 = para['lambda1']
    cdef double lmda2 = para['lambda2']
    cdef double alpha = para['alpha']
    cdef int maxIter = para['maxIter']
    cdef double etaInit = para['etaInit']
    cdef np.ndarray[double, ndim=1, mode='c'] userGroup1 = locGroup[0]
    cdef np.ndarray[double, ndim=1, mode='c'] wsGroup1 = locGroup[1]
    cdef np.ndarray[double, ndim=1, mode='c'] userGroup2 = locGroup[2]
    cdef np.ndarray[double, ndim=1, mode='c'] wsGroup2 = locGroup[3]
    cdef int numUser1 = userGroup1.shape[0]
    cdef int numService1 = wsGroup1.shape[0] 
    cdef int numUser2 = userGroup2.shape[0] 
    cdef int numService2 = wsGroup2.shape[0] 

    # initialization
    cdef np.ndarray[double, ndim=2, mode='c'] predMatrix = \
        np.zeros((numUser, numService), dtype=np.float64)
    
    logger.info('Start iterating...')

    # Wrap up HMF.cpp
    HMF(<double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        <double *> userGroup1.data,
        <double *> wsGroup1.data,
        <double *> userGroup2.data,
        <double *> wsGroup2.data,
        numUser,
        numService,
        numUser1,
        numService1,
        numUser2,
        numService2,
        dim,
        lmda,
        lmda1, 
        lmda2,
        alpha,
        maxIter,
        etaInit,
        <double *> predMatrix.data
        )
   
    return predMatrix
#########################################################




