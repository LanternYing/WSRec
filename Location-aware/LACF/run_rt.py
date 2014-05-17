########################################################
# run_rt.py 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/5/10
# Implemented approach: LACF [Tang et al, ICWS'2012]
# Evaluation metrics: MAE, NMAE, RMSE, MRE, NPRE
########################################################

import numpy as np
import os, sys, time
sys.path.append('src')
# Build external model
if not os.path.isfile('src/LACF.so'):
	print 'Lack of LACF.so (the built C++ module of core LACF).' 
	print 'Please first build the C++ code into LACF.so by using: '
	print '>> python setup.py build_ext --inplace'
	sys.exit()
from utilities import *
import execute
 

#########################################################
# config area
#
para = {'dataPath': '../../data/dataset#1/rtMatrix.txt',
		'userLocFile': '../../data/dataset#1/userlist.txt',
		'wsLocFile': '../../data/dataset#1/wslist.txt',
		'outPath': 'result/rtResult_',
		'metrics': ['MAE', 'NMAE', 'RMSE', 'MRE', 'NPRE'], # delete where appropriate
		# matrix density
		'density': list(np.arange(0.01, 0.051, 0.01)) 
					+ list(np.arange(0.10, 0.31, 0.05)), 
		'rounds': 20, # how many runs are performed at each matrix density
        'topK': 10, # the parameter of TopK similar users or services, the default value is
					# topK = 10 as in the reference paper
		'lambda': 0.1, # the combination coefficient of UPCC and IPCC. Although the reference
					   # paper uses lambda = 0.7, we find that lambda = 0.1 performs better 
		'saveTimeInfo': False, # whether to keep track of the running time
		'saveLog': False, # whether to save log into file
		'debugMode': False # whether to record the debug info
		}

initConfig(para)
#########################################################


startTime = time.clock() # start timing
logger.info('==============================================')
logger.info('Approach: LACF [Tang et al, ICWS\'2012].')
logger.info('Loading data: %s'%para['dataPath'])
dataMatrix = np.loadtxt(para['dataPath']) 

# get the location groups for users as well as for services
logger.info('Computing location groups...')
(userGroupByAS, userGroupByCountry, wsGroupByAS, wsGroupByCountry) = \
	execute.LACF.getLocGroup(para)
locGroup = (userGroupByAS, userGroupByCountry, wsGroupByAS, wsGroupByCountry)
     
# run for each density
for density in para['density']:
    execute.predict(dataMatrix, locGroup, density, para)

logger.info(time.strftime('All done. Total running time: %d-th day - %Hhour - %Mmin - %Ssec.',
         time.gmtime(time.clock() - startTime)))
logger.info('==============================================')
sys.path.remove('src')



