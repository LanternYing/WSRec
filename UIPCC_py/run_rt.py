#########################################################
# run_rt.py 
# Developer: Jieming Zhu <jmzhu@cse.cuhk.edu.hk>
# Created: 2014/2/6
# Last updated: 2014/2/7
# Implemented approaches: [UMEAN, IMEAN, UPCC, IPCC,  
# and UIPCC]
# Evaluation metrics: MAE, NMAE, RMSE, MRE, NPRE
#########################################################

import numpy as np
import os, sys, time
sys.path.append('src')
from utilities import *
from predict import *

#########################################################
# config area
#
para = {'dataPath': '..\\data\\dataset#1\\rawData\\rtMatrix.txt',
		'dataFolder': '..\\data\\dataset#1\\generatedData\\RT\\density_',
		'outPath': 'result\\rtResult_',
		'metrics': ['MAE', 'NMAE', 'RMSE', 'MRE', 'NPRE'], # delete where appropriate
		# matrix density
		'density': list(np.arange(0.01, 0.06, 0.01)) + list(np.arange(0.10, 0.31, 0.05)), 
		'rounds': 50, # how many runs are performed at each matrix density
		'topK': 10, # the parameter of TopK similar users or services, the default value is
					# topK = 10 as in the reference paper
		'lambda': 0.1, # the combination coefficient of UPCC and IPCC, the default value is 
					   # lambda = 0.1 as in the reference paper
		'saveTimeInfo': False, # whether to keep track of the running time
		'saveLog': False, # whether to save log into file
		'debugMode': False # whether to record the debug info
		}

initConfig(para)
#########################################################

startTime = time.clock() # start timing
logger.info('==============================================')
logger.info('[UMEAN, IMEAN, UPCC, IPCC, UIPCC]')
logger.info('Load data: %s'%para['dataPath'])
dataPath = np.loadtxt(para['dataPath']) 

# run for each density
for density in para['density']:
    predict(dataPath, density, para)

logger.info(time.strftime('All done. Total running time: %d-th day - %Hhour - %Mmin - %Ssec.',
         time.gmtime(time.clock() - startTime)))
logger.info('==============================================')
sys.path.remove('src')