clc; close all; clear;

addpath('src', '../../data/rtData');
logger('=========================================');
logger('AMF on latency dataset: online-mode gradient descent.');
logger('=========================================');

if exist('result', 'dir')==0
    system('mkdir result');
end

for k = 1
    fileStartTime = tic;
    % import the dataset
    filepath = strcat('rtTimeSlot', num2str(k), '.txt');
    logger(sprintf('Load data: %s', filepath));
    dataMatrix = load(filepath);
    
    % preprocessing
    dataMatrix(dataMatrix == 20 | dataMatrix == 0) = -1;     
    
    % normalization
    normalDataMatrix = dataMatrix;
    transVector = normalDataMatrix(:);
    [transdat] = boxcox(1, transVector(transVector > 0)); %    [transdat, alpha] = boxcox(1, transVector(transVector > 0));
    alpha = 1;
    minValue = min(transdat);
    maxValue = max(transdat);
    normalDataMatrix(normalDataMatrix ~= -1) = boxcox(alpha, normalDataMatrix(normalDataMatrix ~= -1));
    normalDataMatrix(normalDataMatrix ~= -1) = (normalDataMatrix(normalDataMatrix ~= -1) - minValue) / (maxValue - minValue);
    
    for density = 0.5% : 0.05 : 0.5
        outPath = sprintf('resultFolder/%d_rtResult_%.2f.txt', k, density);
        % Case: defalut parameters for rt:  0.002
        AMF( dataMatrix, normalDataMatrix, alpha, minValue, maxValue, outPath, 10, density, 0.000008, 5, 500, 1, 5e-3, 0.3) %lambda = 0.001 learningrate = 0.8
        % Case: alpha = 1
        %AMF( dataMatrix, normalDataMatrix, alpha, minValue, maxValue, outPath, 10, density, 0.01, 0.2, 500, 1, 5e-3, 0.25 )    
    end
    
    logger('=========================================');
    logger(sprintf('Time used: %f s', toc(fileStartTime))); 
    logger(sprintf('File: rtTimeSlot%d.txt done.', k));  
    logger('=========================================');
end

logger('All done.');
logger('=========================================');
rmpath('src', '../../data/rtData');



