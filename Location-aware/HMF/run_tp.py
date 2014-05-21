########################################################
# run_rt.py 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/20
# Implemented approach: HMF [He et al., ICWS'2014]
# Evaluation metrics: MAE, NMAE, RMSE, MRE, NPRE
########################################################

import numpy as np
import os, sys, time
sys.path.append('src')
# Build external model
if not os.path.isfile('src/HMF.so'):
	print 'Lack of HMF.so (the built C++ module of core HMF).' 
	print 'Please first build the C++ code into HMF.so by using: '
	print '>> python setup.py build_ext --inplace'
	sys.exit()
from utilities import *
import execute
 

#########################################################
# config area
#
para = {'dataPath': '../../data/dataset#1/tpMatrix.txt',
		'userLocFile': '../../data/dataset#1/userlist.txt',
		'wsLocFile': '../../data/dataset#1/wslist.txt',
		'outPath': 'result/tpResult_',
		'metrics': ['MAE', 'NMAE', 'RMSE', 'MRE', 'NPRE'], # delete where appropriate
		# matrix density
		'density': list(np.arange(0.01, 0.051, 0.01)) 
					+ list(np.arange(0.10, 0.31, 0.05)), 
		'rounds': 20, # how many runs are performed at each matrix density
		'dimension': 10, # dimenisionality of the latent factors
		'etaInit': 0.001, # inital learning rate. We use line search
						 # to find the best eta at each iteration
		'lambda': 800, # L2 regularization parameter for global matrix
		'lambda1': 100, # L2 regularization parameter for local cluster 1
		'lambda2': 100, # L2 regularization parameter for local cluster 2
		'alpha': 0.8, # the parameter of location regularization
		'maxIter': 300, # the max iterations 
		'saveTimeInfo': False, # whether to keep track of the running time
		'saveLog': False, # whether to save log into file
		'debugMode': False # whether to record the debug info
		}

initConfig(para)
#########################################################


startTime = time.clock() # start timing
logger.info('==============================================')
logger.info('Approach: HMF [He et al., ICWS\'2014].')
logger.info('Load data: %s'%para['dataPath'])
dataMatrix = np.loadtxt(para['dataPath']) 

# get the location groups for users as well as for services
logger.info('Clustering location groups...')
locGroup = execute.HMF.getLocGroup(para)
     
# run for each density
for density in para['density']:
    execute.predict(dataMatrix, locGroup, density, para)

logger.info(time.strftime('All done. Total running time: %d-th day - %Hhour - %Mmin - %Ssec.',
         time.gmtime(time.clock() - startTime)))
logger.info('==============================================')
sys.path.remove('src')