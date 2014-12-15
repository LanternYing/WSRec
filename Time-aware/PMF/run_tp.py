########################################################
# run_tp.py 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/10/12
# Implemented approach: PMF
# Evaluation metrics: MAE, NMAE, RMSE, MRE, NPRE
########################################################

import numpy as np
import os, sys, time
import multiprocessing
sys.path.append('src')
# Build external model
if not os.path.isfile('src/core.so'):
	print 'Lack of core.so (built from the C++ module).' 
	print 'Please first build the C++ code into core.so by using: '
	print '>> python setup.py build_ext --inplace'
	sys.exit()
from utilities import *
import evaluator
import dataloader
import resulthandler
 

#########################################################
# config area
#
para = {'dataType': 'tp', # choose 'tp' for throughput prediction
        'dataPath': '../../data/dataset#2/',
		'outPath': 'result/raw/',
		'metrics': ['MAE', 'NMAE', 'RMSE', 'MRE', 'NPRE'], # delete where appropriate		
		'density': list(np.arange(0.05, 0.31, 0.05)), # matrix density
		'rounds': 20, # how many runs are performed at each matrix density
		'dimension': 10, # dimenisionality of the latent factors
		'etaInit': 0.001, # inital learning rate. We use line search
						 # to find the best eta at each iteration
		'lambda': 200, # regularization parameter
		'maxIter': 300, # the max iterations
		'saveTimeInfo': False, # whether to keep track of the running time
		'saveLog': True, # whether to save log into file
		'debugMode': False, # whether to record the debug info
        'parallelMode': True # whether to leverage multiprocessing for speedup
		}

initConfig(para)
#########################################################


startTime = time.clock() # start timing
logger.info('==============================================')
logger.info('PMF: Probabilistic Matrix Factorization.')

# load the dataset
dataTensor = dataloader.load(para)
logger.info('Loading data done.')

# run for each density
numTimeSlice = dataTensor.shape[2]
if para['parallelMode']: # run on multiple processes
    pool = multiprocessing.Pool()
    for cxtId in xrange(numTimeSlice):
    	dataMatrix = dataTensor[:, :, cxtId]
    	for density in para['density']:
			pool.apply_async(evaluator.execute, (dataMatrix, density, para, cxtId))
    pool.close()
    pool.join()
else: # run on single processes
    for cxtId in xrange(numTimeSlice):
        dataMatrix = dataTensor[:, :, cxtId]
        for density in para['density']:
			evaluator.execute(dataMatrix, density, para, cxtId)

# result handling
resulthandler.averageStats(para, numTimeSlice)

logger.info(time.strftime('All done. Total running time: %d-th day - %Hhour - %Mmin - %Ssec.',
         time.gmtime(time.clock() - startTime)))
logger.info('==============================================')
sys.path.remove('src')