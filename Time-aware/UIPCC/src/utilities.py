########################################################
# utilities.py
# This is a script containing a bag of useful utilities.
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/2/6
########################################################


import os, sys, time
import numpy as np
import logging

## global
logger = logging.getLogger('logger')   


########################################################
# Config the working paths and set up logger
#
def initConfig(para):
    config = {'exeFile': os.path.basename(sys.argv[0]),  
              'workPath': os.path.abspath('.'),
              'srcPath': os.path.abspath('src/'),
              'dataPath': os.path.abspath('../data/'),
              'logFile': os.path.basename(sys.argv[0]) + '.log'}

    # delete old log file
    if os.path.exists(config['logFile']):
        os.remove(config['logFile'])
    # add result folder
    if not os.path.exists(para['outPath']):
        os.makedirs(para['outPath'])

    ## set up logger to record runtime info
    if para['debugMode']:  
        logger.setLevel(logging.DEBUG)
    else:
        logger.setLevel(logging.INFO) 
    # log to console
    cmdhandler = logging.StreamHandler()  
    cmdhandler.setLevel(logging.DEBUG)       
    formatter = logging.Formatter(
        '%(asctime)s (pid-%(process)d): %(message)s')
    cmdhandler.setFormatter(formatter)
    logger.addHandler(cmdhandler)   
    # log to file
    if para['saveLog']:
        filehandler = logging.FileHandler(config['logFile']) 
        filehandler.setLevel(logging.DEBUG)
        filehandler.setFormatter(formatter)       
        logger.addHandler(filehandler)  
    
    logger.info('==========================================')
    logger.info('Config:')
    config.update(para)
    for name in config:
        logger.info('%s = %s'%(name, config[name]))
########################################################
 

########################################################
# Save the evaluation results into file
#
def saveResult(outfile, result, timeinfo, para):
    approach = ['UMEAN\n', 'IMEAN\n', 'UPCC\n', 'IPCC\n', 'UIPCC\n']
    fileID = open(outfile, 'w')
    fileID.write('Metric: ')
    for metric in para['metrics']:
        fileID.write('| %s\t'%metric)
    fileID.write('\n==========================================\n')
    for i in range(5):
        fileID.write(approach[i])
        avgResult = np.average(result[i, :, :], axis = 0)         
        fileID.write('Avg:\t')
        np.savetxt(fileID, np.matrix(avgResult), fmt='%.4f', delimiter='\t')
        stdResult = np.std(result[i, :, :], axis = 0)
        fileID.write('Std:\t')
        np.savetxt(fileID, np.matrix(stdResult), fmt='%.4f', delimiter='\t')
        fileID.write('==========================================\n')
    fileID.write('\n\n')
    fileID.write('Detailed results for %d rounds:'%result.shape[1])
    fileID.write('\n==========================================\n')
    for i in range(5):
        fileID.write(approach[i])
        fileID.write('==========================================\n')
        np.savetxt(fileID, result[i, :, :], fmt='%.4f', delimiter='\t')  
        fileID.write('\n\n')   
    fileID.close()

    if para['saveTimeInfo']:
        fileID = open(outfile + '_time.txt', 'w')
        fileID.write('Running time:\n')
        fileID.write('==========================================\n')
        for i in range(5):
            fileID.write(approach[i])
            fileID.write('Avg:\t%.4f\n'%np.average(timeinfo[i, :]))
            fileID.write('Std:\t%.4f\n'%np.std(timeinfo[i, :]))
            fileID.write('==========================================\n')

        fileID.write('\n\nDetailed results for %d rounds:\n'%timeinfo.shape[1])
        fileID.write('==========================================')
        for i in range(5):
            fileID.write('\n')
            fileID.write(approach[i])
            fileID.write('==========================================\n')           
            np.savetxt(fileID, np.matrix(timeinfo[i, :]).T, fmt='%.4f')
        fileID.close()
########################################################