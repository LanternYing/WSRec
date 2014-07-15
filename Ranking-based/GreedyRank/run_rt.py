########################################################
# run_rt.py 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/6/16
# Implemented approach: Greedy [Zheng et al, SRDS'2010]
# Evaluation metrics: NDCG1, NDCG5, NDCG10, NDCG20, 
# NDCG50, NDCG100
########################################################

import numpy as np
import os, sys, time
sys.path.append('src')

# Build external model
if not os.path.isfile('src/GreedyRank.so'):
	print 'Lack of GreedyRank.so (the built C++ module of core GreedyRank).'
	print 'Please first build the C++ code into GreedyRank.so by using: '
	print '>> python setup.py build_ext --inplace'
	sys.exit()

from utilities import *
import execute
 

#########################################################
# config area
#
para = {'dataPath': '../../data/dataset#1/rtMatrix.txt',
		'outPath': 'result/rtResult_',
		'metrics': [('NDCG', [1, 5, 10, 20, 50, 100])], # evaluation metrics
		'density': list(np.arange(0.05, 0.31, 0.05)), # matrix density
		'rounds': 20, # how many runs are performed at each matrix density
        'topK': 10, # the parameter of TopK similar users, the default value is
					# topK = 10 as in the reference paper
		'saveTimeInfo': False, # whether to keep track of the running time
		'saveLog': False, # whether to save log into file
		'debugMode': False # whether to record the debug info
		}

initConfig(para)
#########################################################


startTime = time.clock() # start timing
logger.info('==============================================')
logger.info('Approach: GreedyRank [Zheng et al, SRDS\'2010].')
logger.info('Loading data: %s'%para['dataPath'])
dataMatrix = np.loadtxt(para['dataPath'])

# run for each density
for density in para['density']:
    execute.predict(dataMatrix, density, para)

logger.info(time.strftime('All done. Total running time: %d-th day - %Hhour - %Mmin - %Ssec.',
         time.gmtime(time.clock() - startTime)))
logger.info('==============================================')
sys.path.remove('src')



