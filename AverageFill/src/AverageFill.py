########################################################
# AverageFill.py
# Author: Yifei Lu, Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/7/16
########################################################

import time
import numpy as np
from utilities import *


#########################################################
# Function to perform AverageFill
#
def AverageFill(removedMatrix, paraStruct):  
    numUser = np.shape(removedMatrix)[0]
    numService = np.shape(removedMatrix)[1]
    globalAverage = np.sum(removedMatrix) / (np.sum(removedMatrix > 0) + np.spacing(1))
    
    columnSum = np.sum(removedMatrix > 0, axis=0) + np.spacing(1)
    rowSum = np.sum(removedMatrix > 0, axis=1) + np.spacing(1)
    userAverage = np.sum(removedMatrix, axis=1) / rowSum
    itemAverage = np.sum(removedMatrix, axis=0) / columnSum
    userBias = np.sum((removedMatrix - np.tile(itemAverage, (numUser, 1)))\
        * (removedMatrix > 0), axis=1) / rowSum
    itemBias = np.sum((removedMatrix - np.tile(userAverage, (numService, 1)).T)\
        * (removedMatrix > 0), axis=0) / columnSum
    predMatrix = globalAverage + np.tile(userBias, (numService, 1)).T\
        + np.tile(itemBias, (numUser, 1))
    return predMatrix
#########################################################





