########################################################
# run_rt.py 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/12/12
# Implemented approach: LN_LFM [Yu et al., SCC'14]
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
 

#########################################################
# config area
#
para = {'dataType': 'rt', # set 'rt' for response-time prediction
		'dataPath': '../data/dataset#1/',
		'outPath': 'result/',
		'metrics': ['MAE', 'NMAE', 'RMSE', 'MRE', 'NPRE'], # delete where appropriate		
		'density': list(np.arange(0.05, 0.31, 0.05)), # matrix density
		'rounds': 20, # how many runs are performed at each matrix density
		'dimension': 10, # dimenisionality of the latent factors
		'eta': 0.001, # learning rate
		'alpha': 0.6, # the combination coefficient
		'lambda': 0.1, # regularization parameter
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
logger.info('Approach: LN_LFM [Yu et al., SCC\'2014]')

# load the dataset
dataMatrix = dataloader.load(para)
logger.info('Loading data done.')

# load the service location information
wsInfoList = dataloader.loadServInfo(para)
logger.info('Loading service location information done.')

# run for each density
if para['parallelMode']: # run on multiple processes
    pool = multiprocessing.Pool()
    for density in para['density']:
		pool.apply_async(evaluator.execute, (dataMatrix, wsInfoList, density, para))
    pool.close()
    pool.join()
else: # run on single processes
	for density in para['density']:
		evaluator.execute(dataMatrix, wsInfoList, density, para)

logger.info(time.strftime('All done. Total running time: %d-th day - %Hhour - %Mmin - %Ssec.',
         time.gmtime(time.clock() - startTime)))
logger.info('==============================================')
sys.path.remove('src')