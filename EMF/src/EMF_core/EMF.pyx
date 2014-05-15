#########################################################
# EMF.pyx
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
cdef extern from "EMF_core.h":
    void EMF(double *removedData, int numUser, int numService, int dim, 
        double lmda, double alpha, int maxIter, double etaInit, int topK_U, 
        int topK_S, double *Udata, double *Sdata)
#########################################################


#########################################################
# Function to perform EMF
#
def EMF_wrapper(removedMatrix, paraStruct):  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0] 
    cdef int dim = paraStruct['dimension']
    cdef double lmda = paraStruct['lambda']
    cdef int maxIter = paraStruct['maxIter']
    cdef double etaInit = paraStruct['etaInit']
    cdef int topK_U = paraStruct['topK_U']
    cdef int topK_S = paraStruct['topK_S']
    cdef double alpha = paraStruct['alpha']

    # initialization
    cdef np.ndarray[double, ndim=2, mode='c'] U = np.random.rand(numUser, dim)        
    cdef np.ndarray[double, ndim=2, mode='c'] S = np.random.rand(numService, dim)
    
    logger.info('Iterating...')

    # Wrap up the EMF_core.cpp
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

