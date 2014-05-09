########################################################
# run_tp.py 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/8
# Implemented approach: CloudPred
# Evaluation metrics: MAE, NMAE, RMSE, MRE, NPRE
########################################################

import numpy as np
import os, sys, time

sys.path.append('src')
# Build external model
if not os.path.isfile('src/CloudPred.so'):
	print 'Lack of CloudPred.so. Please first build the cpp code into CloudPred.so: '
	print 'python setup.py build_ext --inplace'
	sys.exit()

from utilities import *
from predict import *
 

#########################################################
# config area
#
para = {'dataPath': '../data/dataset#1/tpMatrix.txt',
		'outPath': 'result/tpResult_',
		'metrics': ['MAE', 'NMAE', 'RMSE', 'MRE', 'NPRE'], # delete where appropriate
		# matrix density
		'density': list(np.arange(0.01, 0.06, 0.01)) + list(np.arange(0.10, 0.31, 0.05)), 
		'rounds': 20, # how many runs are performed at each matrix density
		'dimension': 20, # dimenisionality of the latent factors
		'lambda': 800, # regularization parameter
		'topK': 10, # the parameter of TopK similar users or services, the default value is
					# topK = 10 as in the reference paper
		'weight': 0.5, # the combination weight of UPCC and IPCC, the default value is 
		   				# weight = 0.5 as in the reference paper
		'maxIter': 300, # the max iterations
		'saveTimeInfo': False, # whether to keep track of the running time
		'saveLog': False, # whether to save log into file
		'debugMode': False # whether to record the debug info
		}

initConfig(para)
#########################################################


startTime = time.clock() # start timing
logger.info('==============================================')
logger.info('CloudPred: [Zhang et al, SRDS\'2011].')
logger.info('Load data: %s'%para['dataPath'])
dataMatrix = np.loadtxt(para['dataPath']) 

# run for each density
for density in para['density']:
    predict(dataMatrix, density, para)

logger.info(time.strftime('All done. Total running time: %d-th day - %Hhour - %Mmin - %Ssec.',
         time.gmtime(time.clock() - startTime)))
logger.info('==============================================')
sys.path.remove('src')