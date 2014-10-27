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
    void NMF(double *removedData, int numUser, int numService, 
        int dim, double lmda, int maxIter, double *U, double *S)
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

    # initialization
    cdef np.ndarray[double, ndim=2, mode='c'] U = np.random.rand(numUser, dim)        
    cdef np.ndarray[double, ndim=2, mode='c'] S = np.random.rand(numService, dim) 

    # Wrap up NMF.cpp
    NMF(<double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        numUser,
        numService,
        dim,
        lmda,
        maxIter,
        <double *> U.data,
        <double *> S.data
        )

    predMatrix = np.dot(U, S.T)
    return predMatrix
#########################################################


