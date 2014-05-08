########################################################
# NMF.py
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
cdef extern from "NMF_core.h":
    void NMF_core(double *removedData, int numUser, int numService, 
        int dim, double lmda, int maxIter, double *U, double *S)
#########################################################


#########################################################
# Function to perform NMF, as a wrapper of NMF_core
# return the predicted matrix
#
def NMF(removedMatrix, paraStruct):  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0] 
    cdef int dim = paraStruct['dimension']
    cdef int maxIter = paraStruct['maxIter']
    cdef double lmda = paraStruct['lambda']

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

    predMatrix = np.dot(U, S.T)
    return predMatrix
#########################################################


