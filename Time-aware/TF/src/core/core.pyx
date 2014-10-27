########################################################
# core.pyx
# Author: Yuwen Xiong, Jamie Zhu <jimzhu@GitHub>
# Created: 2014/7/9
# Last updated: 2014/7/19
########################################################

import time
import numpy as np
from utilities import *
cimport numpy as np # import C-API
from libcpp cimport bool


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "TF.h":
    void TF(double *removedData, double *predData, int numUser, 
        int numService, int numTimeSlice, int dim, double etaInit, double lmda, 
        int maxIter, bool debugMode, double *Udata, double *Sdata, 
        double *Tdata)
#########################################################


#########################################################
# Function to perform the prediction algorithm
# Wrap up the C++ implementation
#
def predict(removedTensor, para):                         
    cdef int numService = removedTensor.shape[1] 
    cdef int numUser = removedTensor.shape[0]
    cdef int numTimeSlice = removedTensor.shape[2]
    cdef int dim = para['dimension']
    cdef double etaInit = para['etaInit']
    cdef double lmda = para['lambda']
    cdef int maxIter = para['maxIter']
    cdef bool debugMode = para['debugMode']

    # initialization
    cdef np.ndarray[double, ndim=2, mode='c'] U = np.random.rand(dim, numUser)        
    cdef np.ndarray[double, ndim=2, mode='c'] S = np.random.rand(dim, numService)
    cdef np.ndarray[double, ndim=2, mode='c'] T = np.random.rand(dim, numTimeSlice)
    cdef np.ndarray[double, ndim=3, mode='c'] predTensor =\
        np.zeros((numUser, numService, numTimeSlice))
    
    logger.info('Iterating...')
      
    # Wrap up TF.cpp
    TF(<double *> (<np.ndarray[double, ndim=3, mode='c']> removedTensor).data,
        <double *> predTensor.data,
        numUser,
        numService,
        numTimeSlice,
        dim,
        etaInit,
        lmda,
        maxIter,
        debugMode,
        <double *> U.data,
        <double *> S.data,
        <double *> T.data
    )

    return predTensor
#########################################################
