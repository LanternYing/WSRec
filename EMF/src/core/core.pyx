#########################################################
# core.pyx
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/14
#########################################################

import time
import numpy as np
from numpy import linalg as LA
from utilities import *
cimport numpy as np # import C-API


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "EMF.h":
    void EMF(double *removedData, int numUser, int numService, int dim, 
        double lmda, double alpha, int maxIter, double etaInit, int topK_U, 
        int topK_S, double *Udata, double *Sdata)
#########################################################


#########################################################
# Function to perform the prediction algorithm
# Wrap up the C++ implementation
#
def predict(removedMatrix, para):  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0] 
    cdef int dim = para['dimension']
    cdef double lmda = para['lambda']
    cdef int maxIter = para['maxIter']
    cdef double etaInit = para['etaInit']
    cdef int topK_U = para['topK_U']
    cdef int topK_S = para['topK_S']
    cdef double alpha = para['alpha']

    # initialization
    cdef np.ndarray[double, ndim=2, mode='c'] U = np.random.rand(numUser, dim)        
    cdef np.ndarray[double, ndim=2, mode='c'] S = np.random.rand(numService, dim)
    
    logger.info('Iterating...')

    # Wrap up EMF.cpp
    EMF(<double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        numUser,
        numService,
        dim,
        lmda,
        alpha,
        maxIter,
        etaInit,
        topK_U,
        topK_S,
        <double *> U.data,
        <double *> S.data
        )
   
    predMatrix = np.dot(U, S.T)
    return predMatrix
#########################################################

