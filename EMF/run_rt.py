#########################################################
# run_rt.py 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/14
# Implemented approaches: EMF [Lo et al., SCC'12]
# Evaluation metrics: MAE, NMAE, RMSE, MRE, NPRE
#########################################################

import numpy as np
import os, sys, time
sys.path.append('src')
# Build external model
if not os.path.isfile('src/EMF.so'):
	print 'Lack of EMF.so (the built C++ module of core EMF).' 
	print 'Please first build the C++ code into EMF.so by using: '
	print '>> python setup.py build_ext --inplace'
	sys.exit()
from utilities import *
from predict import *


#########################################################
# config area
#
para = {'dataPath': '../data/dataset#1/rtMatrix.txt',
		'outPath': 'result/rtResult_',
		'metrics': ['MAE', 'NMAE', 'RMSE', 'MRE', 'NPRE'], # delete where appropriate
		# matrix density
		'density': list(np.arange(0.01, 0.06, 0.01)) 
					+ list(np.arange(0.10, 0.31, 0.05)), 
		'rounds': 20, # how many runs are performed at each matrix density
		'topK_U': 60, # the parameter of TopK similar users, the default value is
					# topK_U = 60 as in the reference paper
		'topK_S': 300, # the parameter of TopK similar users, the default value is
					# topK_S = 300 as in the reference paper
		'dimension': 10, # dimenisionality of the latent factors
		'etaInit': 0.01, # inital learning rate. We use line search
						 # to find the best eta at each iteration
		'lambda': 30, # L2 regularization parameter
		'alpha': 15,  # parameter of user and service regularization
		'maxIter': 300, # the max iterations
		'saveTimeInfo': False, # whether to keep track of the running time
		'saveLog': False, # whether to save log into file
		'debugMode': False # whether to record the debug info
		}

initConfig(para)
#########################################################


startTime = time.clock() # start timing
logger.info('==============================================')
logger.info('EMF: [Lo et al., SCC\'2012].')
logger.info('Load data: %s'%para['dataPath'])
dataMatrix = np.loadtxt(para['dataPath'])

# run for each density
for density in para['density']:
    predict(dataMatrix, density, para)

logger.info(time.strftime('All done. Total running time: %d-th day - %Hhour - %Mmin - %Ssec.',
         time.gmtime(time.clock() - startTime)))
logger.info('==============================================')
sys.path.remove('src')

