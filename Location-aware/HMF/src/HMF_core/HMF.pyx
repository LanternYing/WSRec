########################################################
# HMF.pyx
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/20
########################################################

import time
import sys
import numpy as np
from utilities import *
from scipy.cluster.vq import *
cimport numpy as np # import C-API


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "HMF_core.h":
    void HMF_core(double *removedData, double *userGroup1, double *wsGroup1,
        double *userGroup2, double *wsGroup2, int numUser, int numService, 
        int numUser1, int numService1, int numUser2, int numService2, int dim, 
        double lmda, double lmda1, double lmda2, double alpha, int maxIter, 
        double etaInit, double *predData)
#########################################################


#########################################################
# Function to wrap up the C++ functions
#
def HMF_wrapper(removedMatrix, locGroup, paraStruct):
    # get the parameters  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0] 
    cdef int dim = paraStruct['dimension']
    cdef double lmda = paraStruct['lambda']
    cdef double lmda1 = paraStruct['lambda1']
    cdef double lmda2 = paraStruct['lambda2']
    cdef double alpha = paraStruct['alpha']
    cdef int maxIter = paraStruct['maxIter']
    cdef double etaInit = paraStruct['etaInit']
    cdef np.ndarray[double, ndim=1, mode='c'] userGroup1 = locGroup[0]
    cdef np.ndarray[double, ndim=1, mode='c'] wsGroup1 = locGroup[1]
    cdef np.ndarray[double, ndim=1, mode='c'] userGroup2 = locGroup[2]
    cdef np.ndarray[double, ndim=1, mode='c'] wsGroup2 = locGroup[3]
    cdef int numUser1 = userGroup1.shape[0]
    cdef int numService1 = wsGroup1.shape[0] 
    cdef int numUser2 = userGroup2.shape[0] 
    cdef int numService2 = wsGroup2.shape[0] 

    # initialization
    cdef np.ndarray[double, ndim=2, mode='c'] predMatrix = \
        np.zeros((numUser, numService), dtype=np.float64)
    
    logger.info('Start iterating...')

    # Wrap up the HMF_core.cpp
    HMF_core(
        <double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        <double *> userGroup1.data,
        <double *> wsGroup1.data,
        <double *> userGroup2.data,
        <double *> wsGroup2.data,
        numUser,
        numService,
        numUser1,
        numService1,
        numUser2,
        numService2,
        dim,
        lmda,
        lmda1, 
        lmda2,
        alpha,
        maxIter,
        etaInit,
        <double *> predMatrix.data
        )
   
    return predMatrix
#########################################################


#########################################################
# Function to cluster users and services into groups
#
def getLocGroup(paraStruct):
    dataPoints = []

    # --- add users: tag = 0
    userLocFile = paraStruct['userLocFile']
    data = np.genfromtxt(userLocFile, dtype=np.float64, comments='$', 
        delimiter='\t', skip_header=2)
    for i in range(data.shape[0]):
        # get the last two columns and
        # filter the unavailable geo data
        if np.isnan(data[i, -2]) == False\
            and np.isnan(data[i, -1]) == False:
            dataPoints.append([0, i, data[i, -2], data[i, -1]])

    # --- add users: tag = 1
    wsLocFile = paraStruct['wsLocFile']
    data = np.genfromtxt(wsLocFile, dtype=np.float64, comments='$', 
        delimiter='\t', skip_header=2)
    for i in range(data.shape[0]):
        # get the last two columns and
        # filter the unavailable geo data
        if np.isnan(data[i, -2]) == False\
            and np.isnan(data[i, -1]) == False:
            dataPoints.append([1, i, data[i, -2], data[i, -1]])

    # --- clustering
    # inital centroids
    initial = [[-30,-50], [-30,150], [25,110], [50,30], [40,-100]]
    initial = np.array(initial)
    dataPoints = np.array(dataPoints)
    (centroids, idx) = kmeans2(dataPoints[:, 2:], initial)

    # only keep two large clusters, ignore the others   
    userGroup1 = []
    userGroup2 = []
    wsGroup1 = []
    wsGroup2 = []
    myiter = 0
    for i in idx:
        # keep cluster[3] and cluster[4]
        if i == 3:
            if dataPoints[myiter, 0] == 0:
                userGroup1.append(dataPoints[myiter, 1])
            else:
                wsGroup1.append(dataPoints[myiter, 1])
        elif i == 4:
            if dataPoints[myiter, 0] == 0:
                userGroup2.append(dataPoints[myiter, 1])
            else:
                wsGroup2.append(dataPoints[myiter, 1])
        myiter += 1
    
    locGroup = (np.array(userGroup1), np.array(wsGroup1), 
        np.array(userGroup2), np.array(wsGroup2))
    return locGroup    
#########################################################

