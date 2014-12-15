########################################################
# resulthandler.py: get the average values of the results
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/11/14
########################################################

import numpy as np
import linecache
import os, sys, time
 

########################################################
# Get the average statistics
#
def averageStats(para, numTimeSlice):
	# UMEAN
	skippedHeader = 28
	skippedFooter = 4 * (para['rounds'] + 2)
	resultFolder = para['outPath'].split('/')[0] + '/UMEAN/'
	if not os.path.exists(resultFolder):
		os.mkdir(resultFolder)
	print 'UMEAN:'
	extractRaw(para, numTimeSlice, skippedHeader, skippedFooter, resultFolder)

	# IMEAN
	skippedHeader += para['rounds'] + 4
	skippedFooter = 3 * (para['rounds'] + 2)
	resultFolder = para['outPath'].split('/')[0] + '/IMEAN/'
	if not os.path.exists(resultFolder):
		os.mkdir(resultFolder)
	print 'IMEAN:'
	extractRaw(para, numTimeSlice, skippedHeader, skippedFooter, resultFolder)

	# UPCC
	skippedHeader += para['rounds'] + 4
	skippedFooter = 2 * (para['rounds'] + 2)
	resultFolder = para['outPath'].split('/')[0] + '/UPCC/'
	if not os.path.exists(resultFolder):
		os.mkdir(resultFolder)
	print 'UPCC:'
	extractRaw(para, numTimeSlice, skippedHeader, skippedFooter, resultFolder)

	# IPCC
	skippedHeader += para['rounds'] + 4
	skippedFooter = para['rounds'] + 2
	resultFolder = para['outPath'].split('/')[0] + '/IPCC/'
	if not os.path.exists(resultFolder):
		os.mkdir(resultFolder)
	print 'IPCC:'
	extractRaw(para, numTimeSlice, skippedHeader, skippedFooter, resultFolder)

	# UIPCC
	skippedHeader += para['rounds'] + 4
	skippedFooter = 0
	resultFolder = para['outPath'].split('/')[0] + '/UIPCC/'
	if not os.path.exists(resultFolder):
		os.mkdir(resultFolder)
	print 'UIPCC:'
	extractRaw(para, numTimeSlice, skippedHeader, skippedFooter, resultFolder)
########################################################


########################################################
# Extract the raw result files
#
def extractRaw(para, numTimeSlice, skippedHeader, skippedFooter, resultFolder):
	for den in para['density']:
		for timeslice in xrange(numTimeSlice):
			inputfile = para['outPath'] + '%02d_%sResult_%.2f.txt'\
				%(timeslice + 1, para['dataType'], den)
			data = np.genfromtxt(inputfile, dtype=np.float64, comments='$', 
        		delimiter='\t', skip_header=skippedHeader, skip_footer = skippedFooter)
			if timeslice == 0:
				result = np.zeros((data.shape[0], data.shape[1], numTimeSlice))
			result[:, :, timeslice] = data
		resultOfRounds = np.average(result, axis=2)
		outfile = resultFolder + 'avg_%sResult_%.2f.txt'%(para['dataType'], den)
		saveAvgResult(outfile, resultOfRounds, para)
		avgResult = np.average(resultOfRounds, axis=0)
		print avgResult
########################################################


########################################################
# Save the average results into file
#
def saveAvgResult(outfile, result, para):
    fileID = open(outfile, 'w')
    fileID.write('Metric: ')
    for metric in para['metrics']:
        fileID.write('| %s\t'%metric)
    avgResult = np.average(result, axis = 0)         
    fileID.write('\nAvg:\t')
    np.savetxt(fileID, np.matrix(avgResult), fmt='%.4f', delimiter='\t')
    stdResult = np.std(result, axis = 0)
    fileID.write('Std:\t')
    np.savetxt(fileID, np.matrix(stdResult), fmt='%.4f', delimiter='\t')
    fileID.write('\n==========================================\n')
    fileID.write('Detailed results for %d rounds:\n'%result.shape[0])
    np.savetxt(fileID, result, fmt='%.4f', delimiter='\t')     
    fileID.close()
########################################################
