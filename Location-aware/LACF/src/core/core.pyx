########################################################
# core.pyx
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/16
########################################################

import time
import sys
import numpy as np
from utilities import *
cimport numpy as np # import C-API


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "LACF.h":
    void LACF(double *removedData, double *userGroupByASData, 
        double *userGroupByCountryData, double *wsGroupByASData, 
        double *wsGroupByCountryData, int numUser, int numService, 
        int topK, double lmda, double *predData)
#########################################################


#########################################################
# Function to perform the prediction algorithm
# Wrap up the C++ implementation
#
def predict(removedMatrix, locGroup, para):
    # get the parameters  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0] 
    cdef double lmda = para['lambda']
    cdef int topK = para['topK']
    cdef np.ndarray[double, ndim=2, mode='c'] userGroupByAS = locGroup[0]
    cdef np.ndarray[double, ndim=2, mode='c'] userGroupByCountry = locGroup[1]
    cdef np.ndarray[double, ndim=2, mode='c'] wsGroupByAS = locGroup[2]
    cdef np.ndarray[double, ndim=2, mode='c'] wsGroupByCountry = locGroup[3]

    # initialization
    cdef np.ndarray[double, ndim=2, mode='c'] predMatrix = \
        np.zeros((numUser, numService), dtype=np.float64)
    
    logger.info('Starting LACF...')

    # Wrap up LACF.cpp
    LACF(<double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        <double *> userGroupByAS.data,
        <double *> userGroupByCountry.data,
        <double *> wsGroupByAS.data,
        <double *> wsGroupByCountry.data,
        numUser,
        numService,
        topK,
        lmda,
        <double *> predMatrix.data
        )
   
    return predMatrix
#########################################################



