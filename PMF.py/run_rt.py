########################################################
# run_rt.py 
# Developer: Jieming Zhu <jmzhu@cse.cuhk.edu.hk>
# Created: 2014/2/6
# Last updated: 2014/2/7
# Implemented approach: PMF
# Evaluation metrics: MAE, NMAE, RMSE, MRE, NPRE
########################################################

import numpy as np
import os, sys, time
sys.path.append('src')
from utilities import *
from PMF import *
 

#########################################################
# config area
#
para = {'dataPath': '..\\data\\dataset#1\\rawData\\rtMatrix.txt',
		'dataFolder': '..\\data\\dataset#1\\generatedData\\RT\\density_',
		'outPath': 'result\\rtResult_',
		'metrics': ['MAE', 'NMAE', 'RMSE', 'MRE', 'NPRE'], # delete where appropriate
		# matrix density
		'density': list(np.arange(0.01, 0.06, 0.01)) + list(np.arange(0.10, 0.51, 0.05)), 
		'rounds': 20, # how many runs are performed at each matrix density
		'dimension': 10, # dimenisionality of the latent factors
		'etaInit': 0.01, # inital learning rate. We use line search
						 # to find the best eta at each iteration
		'lambda': 30, # regularization parameter
		'maxIter': 500, # the max iterations
		'saveTimeInfo': False, # whether to keep track of the running time
		'saveLog': False, # whether to save log into file
		'debugMode': False # whether to record the debug info
		}

initConfig(para)
#########################################################

startTime = time.clock() # start timing
logger.info('==============================================')
logger.info('PMF: Probabilistic Matrix Factorization.')
logger.info('Load data: %s'%para['dataPath'])
dataPath = np.loadtxt(para['dataPath']) 

# run for each density
for density in para['density']:
    PMF(dataPath, density, para)

logger.info(time.strftime('All done. Total running time: %d-th day - %Hhour - %Mmin - %Ssec.',
         time.gmtime(time.clock() - startTime)))
logger.info('==============================================')
sys.path.remove('src')