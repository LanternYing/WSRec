########################################################
# core.pyx
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/26
########################################################

import time
import sys
import numpy as np
from utilities import *
cimport numpy as np # import C-API


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "RegionKNN.h":
    void RegionKNN(double *removedData, double *initUserRegionData, 
    	int numUser, int numService, int numInitUserRegion, int topK, 
    	double lmda, double mu_u, double *predData)
#########################################################


#########################################################
# Function to perform the prediction algorithm
# Wrap up the C++ implementation
#
def predict(removedMatrix, initUserRegion, para):
    # get the parameters  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0]
    cdef int numInitUserRegion = initUserRegion.shape[0]
    cdef double lmda = para['lambda']
    cdef int topK = para['topK']
    cdef double mu_u = para['mu_u']

    # initialization
    cdef np.ndarray[double, ndim=2, mode='c'] predMatrix = \
        np.zeros((numUser, numService), dtype=np.float64)
    
    logger.info('Starting RegionKNN...')

    # Wrap up RegionKNN.cpp
    RegionKNN(
        <double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        <double *> (<np.ndarray[double, ndim=2, mode='c']> initUserRegion).data,
        numUser,
        numService,
        numInitUserRegion,
        topK,
        lmda,
        mu_u,
        <double *> predMatrix.data
        )
   
    return predMatrix
#########################################################



