clc; close all; clear;
global DEBUG;
DEBUG = ['debug1', 'debug2', 'plotConvergFig'];

addpath('src', '../../data/ThroughputData');
logger('=========================================');
logger('AMF on throughput dataset: online-mode gradient descent.');
logger('=========================================');
if exist('resultFolder', 'dir')==0
    system('mkdir resultFolder');
end

for k = 1
    tic;
    % import the dataset
    filepath = strcat('tpTimeSlot', num2str(k), '.txt');
    logger(sprintf('Load data: %s', filepath));
    dataMatrix = load(filepath);
    
    % preprocessing
    dataMatrix(dataMatrix == 0) = -1;
%     dataMatrix(dataMatrix >= 1000) = -1;
    
    % normalization
    normalDataMatrix = dataMatrix;
    transVector = normalDataMatrix(:);
    [transdat, alpha] = boxcox(transVector(transVector > 0));
    minValue = min(transdat);
    maxValue = max(transdat);
    normalDataMatrix(normalDataMatrix ~= -1) = boxcox(alpha, normalDataMatrix(normalDataMatrix ~= -1));
    normalDataMatrix(normalDataMatrix ~= -1) = (normalDataMatrix(normalDataMatrix ~= -1) - minValue) / (maxValue - minValue);
   
    for density = 0.45: 0.05 : 0.5
        outPath = sprintf('resultFolder/%d_tpResult_%.2f.txt', k, density);
        AMF( dataMatrix, normalDataMatrix, alpha, minValue, maxValue, outPath, 10, density, 0.8, 0.001, 500, 1, 5e-3, 0.3 )  %lambda = 0.001
    end
    
    logger('=========================================');
    logger(sprintf('Time used: %f s', toc)); 
    logger(sprintf('File: tpTimeSlot%d.txt done.', k));  
    logger('=========================================');
end
logger('All done.');
logger('=========================================');
rmpath('src', '../../data/ThroughputData');




