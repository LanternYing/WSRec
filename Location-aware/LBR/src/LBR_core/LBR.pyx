########################################################
# LBR.pyx
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/9
########################################################

import time
import numpy as np
from utilities import *
cimport numpy as np # import C-API


#########################################################
# Make declarations on functions from cpp file
#
cdef extern from "LBR_core.h":
    void getLocSim_core(double *geoData, double *locSimData, 
        int numLine, double theta)
    void LBR_core(double *removedData, double *locSimData, 
        int numUser, int numService, int dim, double lmda, 
        double alpha, int maxIter, double etaInit, 
        double *Udata, double *Sdata)
#########################################################


#########################################################
# Function to perform iterations by gradient descent
#
def LBR(removedMatrix, locSim, paraStruct):  
    cdef int numService = removedMatrix.shape[1] 
    cdef int numUser = removedMatrix.shape[0] 
    cdef int dim = paraStruct['dimension']
    cdef double lmda = paraStruct['lambda']
    cdef double alpha = paraStruct['alpha']
    cdef int maxIter = paraStruct['maxIter']
    cdef double etaInit = paraStruct['etaInit']

    # initialization
    cdef np.ndarray[double, ndim=2, mode='c'] U = np.random.rand(numUser, dim)        
    cdef np.ndarray[double, ndim=2, mode='c'] S = np.random.rand(numService, dim)
    
    logger.info('Iterating...')

    # Wrap the LBR_core.cpp
    LBR_core(
        <double *> (<np.ndarray[double, ndim=2, mode='c']> removedMatrix).data,
        <double *> (<np.ndarray[double, ndim=2, mode='c']> locSim).data,
        numUser,
        numService,
        dim,
        lmda,
        alpha,
        maxIter,
        etaInit,
        <double *> U.data,
        <double *> S.data
        )
   
    predMatrix = np.dot(U, S.T)
    return predMatrix
#########################################################


#########################################################
# Function to compute the location similarity
#
def getLocSim(paraStruct):
    geoFile = paraStruct['geoFile']
    data = np.genfromtxt(geoFile, comments='$', delimiter='\t', 
        skip_header=2)
    geoAxis = data[:, -2:] # the last two columns
    geoAxis[np.isnan(geoAxis)] = 999999 #unavailable geo data
    geoAxis = geoAxis.copy() # keep memory consist into C++

    cdef double theta = paraStruct['theta']
    cdef int numLine = geoAxis.shape[0]
    cdef np.ndarray[double, ndim=2, mode='c'] locSim \
        = np.zeros((numLine, numLine), dtype=np.float64)

    getLocSim_core(
        <double *> (<np.ndarray[double, ndim=2, mode='c']> geoAxis).data,
        <double *> locSim.data,
        numLine,
        theta
        )
    print locSim[0,232], locSim[0,233], locSim[299,298]
    return locSim
#########################################################
