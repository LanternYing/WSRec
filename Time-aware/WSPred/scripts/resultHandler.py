########################################################
# run_rt.py 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/2/6
# Last updated: 2014/6/8
########################################################

import numpy as np
import linecache
import os, sys, time


def main():
	para = {'metrics': ['MAE', 'NMAE', 'RMSE', 'MRE', 'NPRE']} # delete where appropriate
	resultFolder = '../result/'
	skippedLine = 6
	timeSlice = 64
	density = list(np.arange(0.05, 0.31, 0.05))

	for den in density:
		for timeslice in range(timeSlice):
			inputfile = resultFolder + '%02d_rtResult_%.2f.txt'%(timeslice + 1, den)
			data = np.genfromtxt(inputfile, dtype=np.float64, comments='$', 
        		delimiter='\t', skip_header=skippedLine)
			if timeslice == 0:
				result = np.zeros((data.shape[0], data.shape[1], timeSlice))
			result[:, :, timeslice] = data
		outfile = resultFolder + 'avg_rtResult_%.2f.txt'%(den)
		saveResult(outfile, result, para)
		avgResult = np.average(np.average(result, axis=2), axis=0)
		print avgResult

	for den in density:
		for timeslice in range(timeSlice):
			inputfile = resultFolder + '%02d_tpResult_%.2f.txt'%(timeslice + 1, den)
			data = np.genfromtxt(inputfile, dtype=np.float64, comments='$', 
        		delimiter='\t', skip_header=skippedLine)
			if timeslice == 0:
				result = np.zeros((data.shape[0], data.shape[1], timeSlice))
			result[:, :, timeslice] = data
		outfile = resultFolder + 'avg_tpResult_%.2f.txt'%(den)
		saveResult(outfile, result, para)
		avgResult = np.average(np.average(result, axis=2), axis=0)
		print avgResult


########################################################
# Save the evaluation results into file
#
def saveResult(outfile, result, para):
    fileID = open(outfile, 'w')
    fileID.write('Metric: ')
    for metric in para['metrics']:
        fileID.write('| %s\t'%metric)
    avgOverTime = np.average(result, axis=2)
    avg = np.average(avgOverTime, axis = 0)        
    fileID.write('\nAvg:\t')
    np.savetxt(fileID, np.matrix(avg), fmt='%.4f', delimiter='\t')
    std = np.std(avgOverTime, axis = 0)
    fileID.write('Std:\t')
    np.savetxt(fileID, np.matrix(std), fmt='%.4f', delimiter='\t')
    fileID.write('\n==========================================\n')
    fileID.write('Detailed average results for %d rounds:\n'%avgOverTime.shape[0])
    np.savetxt(fileID, avgOverTime, fmt='%.4f', delimiter='\t')
    fileID.write('\n==========================================\n')
    avgOverRounds = np.average(result, axis=0)
    fileID.write('Detailed average results for %d slices:\n'%avgOverRounds.shape[1])
    np.savetxt(fileID, avgOverRounds.T, fmt='%.4f', delimiter='\t')     
    fileID.close()
########################################################


if __name__ == "__main__": main()