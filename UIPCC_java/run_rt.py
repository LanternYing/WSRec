# running script format:
# java -Xmx1024m -jar UIPCC.jar rawdataPath dataFolder outputPath numUser numService matrixDensity rounds saveTimeInfo
#
# output evaluation metrics: MAE, NMAE, RMSE, MRE, NPRE

import os
import numpy as np

#########################################################
# config area
#
memoryAlloc = "1024m" 
rawdataPath = "..\\data\\dataset#1\\rawData\\rtMatrix.txt"
dataFolder = "..\\data\\dataset#1\\generatedData\\RT\\density_"
outputPath = "result\\rtResult_"
numUser = 339
numService = 5825
densitySet = list(np.arange(0.10, 0.31, 0.05))
rounds = 50
topK = 10 # the topK number of similar users or services
lambd = 0.1 # default weight of combining UPCC and IPCC
saveTimeInfo = "false"
#########################################################

for density in densitySet:
	cmd = "java -Xmx{0} -jar UIPCC.jar {1} {2}{3:.2f}\\ {4}{3:.2f}.txt\
		  {5} {6} {3:.2f} {7} {8} {9} {10}\n"\
		  .format(memoryAlloc, rawdataPath, dataFolder, density, 
		  	outputPath, numUser, numService, rounds, topK,
		  	lambd, saveTimeInfo)
	print(cmd)
	os.system(cmd) 
   