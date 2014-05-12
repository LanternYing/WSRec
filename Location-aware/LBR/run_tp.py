########################################################
# run_rt.py 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/10
# Implemented approach: LBR [Lo et al, ICWS'2012]
# Evaluation metrics: MAE, NMAE, RMSE, MRE, NPRE
########################################################

import numpy as np
import os, sys, time
sys.path.append('src')
# Build external model
if not os.path.isfile('src/LBR.so'):
	print 'Lack of LBR.so. Please first build the cpp code into LBR.so: '
	print 'python setup.py build_ext --inplace'
	sys.exit()
from utilities import *
from predict import *
 

#########################################################
# config area
#
para = {'dataPath': '../../data/dataset#1/tpMatrix.txt',
		'geoFile': '../../data/dataset#1/userlist.txt',
		'outPath': 'result/tpResult_',
		'metrics': ['MAE', 'NMAE', 'RMSE', 'MRE', 'NPRE'], # delete where appropriate
		# matrix density
		'density': list(np.arange(0.01, 0.06, 0.01)) 
					+ list(np.arange(0.10, 0.31, 0.05)), 
		'rounds': 20, # how many runs are performed at each matrix density
		'dimension': 10, # dimenisionality of the latent factors
		'etaInit': 0.001, # inital learning rate. We use line search
						 # to find the best eta at each iteration
		'lambda': 800, # regularization parameter
		'alpha': 900, # the parameter of location regularization
		'theta': 100, # the distance threshold to control the neighborhood size
					  # the default value is theta = 100 as in the reference paper
		'maxIter': 300, # the max iterations
		'saveTimeInfo': False, # whether to keep track of the running time
		'saveLog': False, # whether to save log into file
		'debugMode': False # whether to record the debug info
		}

initConfig(para)
#########################################################


startTime = time.clock() # start timing
logger.info('==============================================')
logger.info('Approach: LBR [Lo et al, ICWS\'2012].')
logger.info('Load data: %s'%para['dataPath'])
dataMatrix = np.loadtxt(para['dataPath']) 

# get the location similarity between users
locSim = getLocSim(para)

# run for each density
for density in para['density']:
    predict(dataMatrix, locSim, density, para)

logger.info(time.strftime('All done. Total running time: %d-th day - %Hhour - %Mmin - %Ssec.',
         time.gmtime(time.clock() - startTime)))
logger.info('==============================================')
sys.path.remove('src')