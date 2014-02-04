# running script format:
# java -Xmx1024m -jar UIPCC.jar rawdataPath dataFolder outputPath numUser numService matrixDensity rounds saveTimeInfo
#
# output evaluation metrics: MAE, NMAE, RMSE, MRE, NPRE

import os
import numpy as np

# ************************** config area **************************
memoryAlloc = "1024m" 
rawdataPath = "..\\data\\rawData\\tpMatrix.txt"
dataFolder = "..\\data\\generatedData\\TP\\density_"
outputPath = "resultFolder\\tpResult_"
userNum = 339
serviceNum = 5825
densitySet = list(np.arange(0.10, 0.31, 0.05))
rounds = 20
saveTimeInfo = "false";
# *****************************************************************

for density in densitySet:
	cmd = "java -Xmx{0} -jar UIPCC.jar {1} {2}{3:.2f}\\ {4}{3:.2f}.txt {5} {6} {3:.2f} {7} {8}\n"\
	.format(memoryAlloc, rawdataPath, dataFolder, density, outputPath, userNum, 
		serviceNum, rounds, saveTimeInfo)
	print(cmd)
	os.system(cmd) 
   