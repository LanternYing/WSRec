########################################################
# core.pyx
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/6/17
########################################################

import time
import sys
import numpy as np
from utilities import *
cimport numpy as np # import C-API


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "CloudRank.h":
    void CloudRank(double *removedData, int numUser, 
        int numService, int topK, int *predData)
#########################################################


#########################################################
# Function to perform the prediction algorithm
# Wrap up the C++ implementation
#
def predict(removedMatrix, para):
    # get the parameters  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0]
    cdef int topK = para['topK']

    # initialization
    cdef np.ndarray[int, ndim=2, mode='c'] predRankMatrix = \
        np.zeros((numUser, numService), dtype=np.int32)
    
    logger.info('Start CloudRank...')

    # Wrap up CloudRank.cpp
    CloudRank(
        <double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        numUser,
        numService,
        topK,
        <int *> predRankMatrix.data
        )
    
    return predRankMatrix
#########################################################


