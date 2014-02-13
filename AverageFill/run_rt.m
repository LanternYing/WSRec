clc; close all; clear;

addpath('src');
logger('===============================');
logger('Baseline: Average filling (AF).');
logger('===============================');

if exist('resultFolder', 'dir')==0
    system('mkdir resultFolder');
end

% load the dataset
dataPath = '../data/dataset#1/rawData/rtMatrix.txt';
logger(sprintf('Load data: %s', dataPath));
dataMatrix = load(dataPath);

% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
% config area 
rounds = 20;
density = 0.1 : 0.05 : 0.3;
saveTimeInfo = false; % whether to keep track of the running time
% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

% run for each density
runTime = tic; % start timing
for i = 1 : length(density)
    dataFolder = sprintf('../data/dataset#1/generatedData/RT/density_%.2f', density(i));
    outPath = sprintf('resultFolder/rtResult_%.2f.txt', density(i));
    averagefill(dataMatrix, dataFolder, outPath, density(i), rounds, saveTimeInfo) 
end  

logger('===============================');
logger(sprintf('All done. Total running time: %f s', toc(runTime)));
logger('===============================');
rmpath('src');



