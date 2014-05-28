########################################################
# LoRec.pyx
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/23
########################################################

import time
import sys
import numpy as np
from utilities import *
cimport numpy as np # import C-API


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "LoRec_core.h":
    void LoRec_core(double *removedData, double *initUserRegionData, 
        int numUser, int numService, int numInitUserRegion, int topK, 
        double lmda, double omega, double mu_u, double mu_w, 
        double *predData)
#########################################################


#########################################################
# Function to wrap up the C++ function for python
#
def LoRec_wrapper(removedMatrix, initUserRegion, paraStruct):
    # get the parameters  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0]
    cdef int numInitUserRegion = initUserRegion.shape[0]
    cdef double lmda = paraStruct['lambda']
    cdef int topK = paraStruct['topK']
    cdef double omega = paraStruct['omega']
    cdef double mu_u = paraStruct['mu_u']
    cdef double mu_w = paraStruct['mu_w']

    # initialization
    cdef np.ndarray[double, ndim=2, mode='c'] predMatrix = \
        np.zeros((numUser, numService), dtype=np.float64)
    
    logger.info('Starting LoRec...')

    # Wrap up the LoRec_core.cpp
    LoRec_core(
        <double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        <double *> (<np.ndarray[double, ndim=2, mode='c']> initUserRegion).data,
        numUser,
        numService,
        numInitUserRegion,
        topK,
        lmda,
        omega,
        mu_u,
        mu_w,
        <double *> predMatrix.data
        )
   
    return predMatrix
#########################################################


#########################################################
# Function to get the inital user regions
#
def getInitalRegion(paraStruct): 
    # get the users in a city into a dictionary 
    userRegionDic = {}
    userLocFile = paraStruct['userLocFile']
    data = np.genfromtxt(userLocFile, dtype=np.str, comments='$', 
        delimiter='\t', skip_header=2)
    userCity = data[:, 8]
    numUser = data.shape[0]
    for i in range(numUser):
        key = userCity[i]
        if key in userRegionDic:
            userRegionDic[key].append(i)
        else:
            userRegionDic[key] = [i]

    # transform the dictionary into a array
    initUserRegion = np.zeros((len(userRegionDic), numUser))
    i = 0        
    for key in userRegionDic:
        for value in userRegionDic[key]:
            initUserRegion[i, value] = 1
        i = i + 1
    
    return initUserRegion
#########################################################
