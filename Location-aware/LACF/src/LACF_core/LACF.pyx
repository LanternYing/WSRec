########################################################
# LACF.pyx
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
cdef extern from "LACF_core.h":
    void LACF_core(double *removedData, double *userGroupByASData, 
        double *userGroupByCountryData, double *wsGroupByASData, 
        double *wsGroupByCountryData, int numUser, int numService, 
        int topK, double lmda, double *predData)
#########################################################


#########################################################
# Function to perform iterations by gradient descent
#
def LACF_wrapper(removedMatrix, locGroup, paraStruct):
    # get the parameters  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0] 
    cdef double lmda = paraStruct['lambda']
    cdef int topK = paraStruct['topK']
    cdef np.ndarray[double, ndim=2, mode='c'] userGroupByAS = locGroup[0]
    cdef np.ndarray[double, ndim=2, mode='c'] userGroupByCountry = locGroup[1]
    cdef np.ndarray[double, ndim=2, mode='c'] wsGroupByAS = locGroup[2]
    cdef np.ndarray[double, ndim=2, mode='c'] wsGroupByCountry = locGroup[3]

    # initialization
    cdef np.ndarray[double, ndim=2, mode='c'] predMatrix = \
        np.zeros((numUser, numService), dtype=np.float64)
    
    logger.info('Starting LACF...')

    # Wrap up the LACF_core.cpp
    LACF_core(
        <double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
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


#########################################################
# Function to compute the location similarity
#
def getLocGroup(paraStruct):
    # Group users by AS and country
    userLocFile = paraStruct['userLocFile']
    data = np.genfromtxt(userLocFile, dtype=np.str, comments='$', 
        delimiter='\t', skip_header=2)
    userCountry = data[:, 2] 
    userAS = data[:, 4]
    numUser = userAS.shape[0]
    userGroupByAS = np.zeros((numUser, numUser))
    userGroupByCountry = np.zeros((numUser, numUser))
    for i in range(numUser):
        for j in range(numUser):
            if i < j:
                userAS_i = userAS[i].split(" ", 1)[0]
                userAS_j = userAS[j].split(" ", 1)[0]
                if (userAS_i == userAS_j) and \
                    (userAS_i != 'null') and \
                    (userAS_j != 'null'):
                    userGroupByAS[i, j] = 1
                    userGroupByAS[j, i] = 1
                if userCountry[i] == userCountry[j]:
                    userGroupByCountry[i, j] = 1
                    userGroupByCountry[j, i] = 1

    # Group services by AS and country
    wsLocFile = paraStruct['wsLocFile']
    data = np.genfromtxt(wsLocFile, dtype=np.str, comments='$', 
        delimiter='\t', skip_header=2)
    wsCountry = data[:, 4] 
    wsAS = data[:, 6]
    numService = wsAS.shape[0]
    wsGroupByAS = np.zeros((numService, numService))
    wsGroupByCountry = np.zeros((numService, numService))
    for i in range(numService):
        for j in range(numService):
            if i < j:
                wsAS_i = wsAS[i].split(" ", 1)[0]
                wsAS_j = wsAS[j].split(" ", 1)[0]
                if (wsAS_i == wsAS_j) and \
                    (wsAS_i != 'null') and \
                    (wsAS_j != 'null'):
                    wsGroupByAS[i, j] = 1
                    wsGroupByAS[j, i] = 1
                elif wsCountry[i] == wsCountry[j]:
                    wsGroupByCountry[i, j] = 1
                    wsGroupByCountry[j, i] = 1
    
    return userGroupByAS, userGroupByCountry, wsGroupByAS, wsGroupByCountry
#########################################################
