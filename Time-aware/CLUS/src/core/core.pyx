########################################################
# core.pyx
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/10/22
########################################################

import time
import numpy as np
from utilities import *
cimport numpy as np # import C-API
from libcpp cimport bool
from libcpp.vector cimport vector
from libcpp cimport *
import scipy.cluster # import K-means


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "CLUS.h":
    void CLUS(double *removedData, double *predData, int numUser, 
        int numService, int numTimeSlice, vector[int] attrEv, 
        vector[int] attrUs, vector[int] attrWs, vector[vector[int]] clusterEv, 
        vector[vector[int]] clusterUs, vector[vector[int]] clusterWs, 
        bool debugMode)
#########################################################


#########################################################
# Function to perform the prediction algorithm
# Wrap up the C++ implementation
#
def predict(removedTensor, para):
    [numUser, numService, numTimeSlice] = removedTensor.shape
    numEvClus = para['numEvClus']
    numUsClus = para['numUsClus']
    numWsClus = para['numWsClus']

    cdef np.ndarray[double, ndim=3, mode='c'] predTensor =\
        np.zeros((numUser, numService, numTimeSlice))
    vecEv = np.zeros(numTimeSlice)
    vecUs = np.zeros((numUser, numEvClus))
    vecWs = np.zeros((numService, numEvClus))
    clusterEv = [[] for i in xrange(numEvClus)]
    clusterUs = [[] for i in xrange(numUsClus)]
    clusterWs = [[] for i in xrange(numWsClus)]

    for i in xrange(numTimeSlice):
        vecEv[i] = np.sum(removedTensor[:, :, i]) / (np.sum(removedTensor[:, :, i] > 0) + np.spacing(1))
    [_, attrEv] = scipy.cluster.vq.kmeans2(np.matrix(vecEv).T, numEvClus, minit = 'points')
    
    for i in xrange(numTimeSlice):
        clusterEv[attrEv[i]].append(i)
    logger.info('Time side clustering done.')

    for i in xrange(numUser):
        for j in xrange(numEvClus):
            vecUs[i, j] = np.sum(removedTensor[i, :, clusterEv[j]])\
            / (np.sum(removedTensor[i, :, clusterEv[j]] > 0) + np.spacing(1))
    [_, attrUs] = scipy.cluster.vq.kmeans2(vecUs, numUsClus, minit = 'points')
    for i in xrange(numUser):
        clusterUs[attrUs[i]].append(i)
    logger.info('User side clustering done.')
        
    for i in xrange(numService):
        for j in xrange(numEvClus):
            vecWs[i, j] = np.sum(removedTensor[:, i, clusterEv[j]])\
            / (np.sum(removedTensor[:, i, clusterEv[j]] > 0) + np.spacing(1))
    [_, attrWs] = scipy.cluster.vq.kmeans2(vecWs, numWsClus, minit = 'points')
    for i in xrange(numService):
        clusterWs[attrWs[i]].append(i)
    logger.info("Service side clustering done.")

    cdef vector[vector[int]] vecClusterEv = clusterEv
    cdef vector[vector[int]] vecClusterUs = clusterUs
    cdef vector[vector[int]] vecClusterWs = clusterWs
    cdef vector[int] vecAttrEv = attrEv
    cdef vector[int] vecAttrUs = attrUs
    cdef vector[int] vecAttrWs = attrWs
    cdef bool debugMode = para['debugMode']

    # wrap up CLUS.cpp
    CLUS(
        <double *> (<np.ndarray[double, ndim=3, mode='c']> removedTensor).data,
        <double *> predTensor.data,
        <int> numUser,
        <int> numService,
        <int> numTimeSlice,
        vecAttrEv,
        vecAttrUs,
        vecAttrWs,
        vecClusterEv,
        vecClusterUs,
        vecClusterWs,
        debugMode  
        )
    
    return predTensor
#########################################################  

