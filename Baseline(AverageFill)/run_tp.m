clc; close all; clear;

addpath('src');
logger('===============================');
logger('Baseline: Average filling (AF).');
logger('===============================');

if exist('resultFolder', 'dir')==0
    system('mkdir resultFolder');
end

% load the dataset
dataPath = '../data/rawData/tpMatrix.txt';
logger(sprintf('Load data: %s', dataPath));
dataMatrix = load(dataPath);

% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
% config area density = [0.01 : 0.01 : 0.05, 0.1 : 0.05 : 0.5];
rounds = 20;
density = [0.45 : 0.05 : 0.5];
saveTimeInfo = false; % whether to keep track of the running time
% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

% run for each density
runTime = tic; % start timing
for i = 1 : length(density)
    dataFolder = sprintf('../data/generatedData/TP/density_%.2f', density(i));
    outPath = sprintf('resultFolder/tpResult_%.2f.txt', density(i));
    averagefill(dataMatrix, dataFolder, outPath, density(i), rounds, saveTimeInfo) 
end  

logger('===============================');
logger(sprintf('All done. Total running time: %f s', toc(runTime)));
logger('===============================');
rmpath('src');



