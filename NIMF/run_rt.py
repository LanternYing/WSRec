#########################################################
# run_rt.py 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/18
# Implemented approaches: NIMF [Zheng et al., TSC'13]
# Evaluation metrics: MAE, NMAE, RMSE, MRE, NPRE
#########################################################

import numpy as np
import os, sys, time
sys.path.append('src')
# Build external model
if not os.path.isfile('src/NIMF.so'):
	print 'Lack of NIMF.so (the built C++ module of core NIMF).' 
	print 'Please first build the C++ code into NIMF.so by using: '
	print '>> python setup.py build_ext --inplace'
	sys.exit()
from utilities import *
import execute


#########################################################
# config area
#
para = {'dataPath': '../data/dataset#1/rtMatrix.txt',
		'outPath': 'result/rtResult_',
		'metrics': ['MAE', 'NMAE', 'RMSE', 'MRE', 'NPRE'], # delete where appropriate
		# matrix density
		'density': list(np.arange(0.01, 0.051, 0.01)) 
					+ list(np.arange(0.10, 0.31, 0.05)), 
		'rounds': 20, # how many runs are performed at each matrix density
		'topK': 10, # the parameter of TopK similar users or services, the default 
					# value is topK = 10 as in the reference paper
		'dimension': 10, # dimenisionality of the latent factors
		'etaInit': 0.01, # inital learning rate. We use line search
						 # to find the best eta at each iteration
		'lambda': 30, # L2 regularization parameter
		'alpha': 0.4,  # the parameter of combination, 0.4 as in the reference paper
		'maxIter': 300, # the max iterations
		'saveTimeInfo': False, # whether to keep track of the running time
		'saveLog': False, # whether to save log into file
		'debugMode': False # whether to record the debug info
		}

initConfig(para)
#########################################################


startTime = time.clock() # start timing
logger.info('==============================================')
logger.info('Approach: NIMF [Zheng et al., TSC\'2013].')
logger.info('Load data: %s'%para['dataPath'])
dataMatrix = np.loadtxt(para['dataPath'])

# run for each density
for density in para['density']:
    execute.predict(dataMatrix, density, para)

logger.info(time.strftime('All done. Total running time: %d-th day - %Hhour - %Mmin - %Ssec.',
         time.gmtime(time.clock() - startTime)))
logger.info('==============================================')
sys.path.remove('src')

