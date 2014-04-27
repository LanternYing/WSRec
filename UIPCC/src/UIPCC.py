#########################################################
# UIPCC.py
# Developer: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/2/6
#########################################################
import sys
import time
import numpy as np
from numpy import linalg as LA
import matplotlib.pyplot as plt
from utilities import *


#########################################################
# Function to perform UMEAN
# return the predicted matrix
#
def UMEAN(removedMatrix):  
    predMatrix = IMEAN(removedMatrix.T).T
    return predMatrix
#########################################################


#########################################################
# Function to perform IMEAN
# return the predicted matrix
#
def IMEAN(removedMatrix):  
    numUser = removedMatrix.shape[0] 
    idxMatrix = (removedMatrix > 0) 
    columnSum = np.sum(idxMatrix, axis=0)   
    imean = np.sum(removedMatrix, axis=0) / (columnSum
            + np.spacing(1)) # avoid divide-by-zero
    predMatrix = np.tile(imean, (numUser, 1))
    return predMatrix
#########################################################


#########################################################
# Function to perform UPCC
# return the predicted matrix
#
def UPCC(removedMatrix, predMatrixUMEAN, para):
    numUser = removedMatrix.shape[0]
    numService = removedMatrix.shape[1]
    idx = (removedMatrix > 0)
    topK = para['topK']
    predMatrix = np.zeros((numUser, numService))
    pccMatrix = PCC(removedMatrix, predMatrixUMEAN)
    for i in range(numUser):
        pcc = idx * (pccMatrix[i, :].reshape(numUser, 1))
        # partial sort to find the topK largest vaules
        similarUsers = np.argpartition(-pcc, topK, axis=0)     
        for j in range(numService):
            topKUsers = similarUsers[0:topK, j]
            offset = (removedMatrix[topKUsers, j] - predMatrixUMEAN[topKUsers, j])\
                    * pcc[topKUsers, j]
            predMatrix[i, j] = predMatrixUMEAN[i, j] + np.sum(offset)\
                    / (np.sum(pcc[topKUsers, j]) + np.spacing(1)) # avoid divide-by-zero
            if predMatrix[i, j] < 0:
                predMatrix[i, j] = predMatrixUMEAN[i, j]
    return predMatrix
#########################################################


#########################################################
# Function to perform IPCC
# return the predicted matrix
#
def IPCC(removedMatrix, predMatrixIMEAN, para):  
    predMatrix = UPCC(removedMatrix.T, predMatrixIMEAN.T, para)
    predMatrix = predMatrix.T
    return predMatrix
#########################################################


#########################################################
# Function to perform UIPCC
# return the predicted matrix
#
def UIPCC(removedMatrix, predMatrixUPCC, predMatrixIPCC, para)  :
    lmd = para['lambda']
    predMatrix = lmd * predMatrixUPCC + (1 - lmd) * predMatrixIPCC
    return predMatrix
#########################################################


#########################################################
# Function to compute the PCC coefficient
#
def PCC(removedMatrix, predMatrixUMEAN):
    numUser = removedMatrix.shape[0]
    pccMatrix = np.zeros((numUser, numUser))
    idxMatrix = (removedMatrix > 0)
    normalizedMatrix = removedMatrix - predMatrixUMEAN
    for i in range(numUser):
        idxUserA = idxMatrix[i, :]
        if np.sum(idxUserA) < 2:
            continue
        for j in range(numUser):
            idxUserB = idxMatrix[j, :]
            if np.sum(idxUserB) < 2:
                continue
            if i < j:
                idxCommon = idxUserA & idxUserB
                vectorA = normalizedMatrix[i, idxCommon]
                vectorB = normalizedMatrix[j, idxCommon]
                normAB = LA.norm(vectorA) * LA.norm(vectorB)
                if np.sum(idxCommon) > 1 and normAB != 0:
                    pcc = np.dot(vectorA, vectorB) / normAB
                    if pcc > 0: # discard dissimilar users
                        pccMatrix[i, j] = pcc
            elif i > j and pccMatrix[j, i] > 0:
                pccMatrix[i, j] = pccMatrix[j, i] 
    return pccMatrix
#########################################################