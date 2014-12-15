########################################################
# core.pyx
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/12/12
########################################################

import time
import numpy as np
from utilities import *
cimport numpy as np # import C-API
from libcpp cimport bool


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "LN_LFM.h":
    void LN_LFM(double *removedData, double *predData, double *wsInfoData, int numUser, 
    int numService, int numProvider, int numCountry, int dim, double lmda, int maxIter, 
    double eta, double alpha, double *Udata, double *Sdata, double *Pdata, double *Cdata, 
    double *bu, double *bs, double *bp, double *bc, bool debugMode)
#########################################################


#########################################################
# Function to perform the prediction algorithm
# Wrap up the C++ implementation
#
def predict(removedMatrix, wsInfoList, para):  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0] 
    cdef int dim = para['dimension']
    cdef double lmda = para['lambda']
    cdef int maxIter = para['maxIter']
    cdef double eta = para['eta']
    cdef double alpha = para['alpha']
    cdef bool debugMode = para['debugMode']
    numProvider = np.amax(wsInfoList[:, 0]) + 1
    numCountry = np.amax(wsInfoList[:, 1]) + 1

    # initialization
    cdef np.ndarray[double, ndim=2, mode='c'] U = np.random.rand(numUser, dim)        
    cdef np.ndarray[double, ndim=2, mode='c'] S = np.random.rand(numService, dim)
    cdef np.ndarray[double, ndim=2, mode='c'] P = np.random.rand(numProvider, dim)        
    cdef np.ndarray[double, ndim=2, mode='c'] C = np.random.rand(numCountry, dim)
    cdef np.ndarray[double, ndim=1, mode='c'] bu = np.random.rand(numUser)
    cdef np.ndarray[double, ndim=1, mode='c'] bs = np.random.rand(numService)
    cdef np.ndarray[double, ndim=1, mode='c'] bp = np.random.rand(numProvider)
    cdef np.ndarray[double, ndim=1, mode='c'] bc = np.random.rand(numCountry)
    cdef np.ndarray[double, ndim=2, mode='c'] predMatrix = np.zeros((numUser, numService))
    
    logger.info('Iterating...')

    # Wrap up LN_LFM.cpp
    LN_LFM(
        <double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        <double *> predMatrix.data,
        <double *> (<np.ndarray[double, ndim=2, mode='c']> wsInfoList).data,
        numUser,
        numService,
        numProvider,
        numCountry,
        dim,
        lmda,
        maxIter,
        eta,
        alpha,
        <double *> U.data,
        <double *> S.data,
        <double *> P.data,
        <double *> C.data,
        <double *> bu.data,
        <double *> bs.data,
        <double *> bp.data,
        <double *> bc.data,
        debugMode
        )
   
    return predMatrix
#########################################################




