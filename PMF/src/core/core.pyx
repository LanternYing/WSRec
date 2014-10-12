########################################################
# core.pyx
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/10/12
########################################################

import time
import numpy as np
from utilities import *
cimport numpy as np # import C-API


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "PMF.h":
    void PMF_core(double *removedData, int numUser, int numService, 
    	int dim, double lmda, int maxIter, double etaInit, 
        double *Udata, double *Sdata)
#########################################################


#########################################################
# Function to perform the prediction algorithm
# Wrap up the C++ implementation
#
def predict(removedMatrix, paraStruct):  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0] 
    cdef int dim = paraStruct['dimension']
    cdef double lmda = paraStruct['lambda']
    cdef int maxIter = paraStruct['maxIter']
    cdef double etaInit = paraStruct['etaInit']

    # initialization
    cdef np.ndarray[double, ndim=2, mode='c'] U = np.random.rand(numUser, dim)        
    cdef np.ndarray[double, ndim=2, mode='c'] S = np.random.rand(numService, dim)
    
    logger.info('Iterating...')

    # Wrap the PMF_core.cpp
    PMF_core(
        <double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        numUser,
        numService,
        dim,
        lmda,
        maxIter,
        etaInit,
        <double *> U.data,
        <double *> S.data
        )
   
    predMatrix = np.dot(U, S.T)
    return predMatrix
#########################################################




