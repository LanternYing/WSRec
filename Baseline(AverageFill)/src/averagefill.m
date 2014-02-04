% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
% Function to run the averagefill method at each density for serveral rounds
function [] = averagefill(matrix, dataFolder, outPath, density, rounds, saveTimeInfo)

    startTime = tic;
    numService = size(matrix, 2);            % number of services
    numUser = size(matrix, 1);                % number of users
    logger(sprintf('Data matrix size:  %d users ¡Á %d services', numUser, numService));
    logger(sprintf('Run Baseline for %d rounds: matrix density = %.2f.', rounds, density));
    evalResults = zeros(rounds, 5);  
    timeResults = zeros(rounds, 1);
    
    for k = 1 : rounds
        logger('----------------------------------------------');
        logger(sprintf('%d-round starts.', k));
        logger('----------------------------------------------');

        % randomly remove some entries
        dataPath = sprintf('%s/round_%02d_train.txt', dataFolder, k);
        logger(sprintf('Load data: %s', dataPath));
        fid = fopen(dataPath, 'r');
        celldata = textscan(fid, '%f', 'Delimiter', '\t'); % textscan is faster than load function
        fclose(fid);
        trainIndicatorMatrix = reshape(celldata{1}, numService, numUser);
        trainIndicatorMatrix = trainIndicatorMatrix';
        trainMatrix = matrix .* trainIndicatorMatrix;     
        
        % average filling  
        iterStartTime = tic; % to record the running time for one round      
        predictedMatrix = compAveragefill(trainMatrix, trainIndicatorMatrix);
        timeResults(k) = toc(iterStartTime);    

        %% calculate the prediction error
        dataPath = sprintf('%s/round_%02d_test.txt', dataFolder, k);
        logger(sprintf('Load data: %s', dataPath));
        fid = fopen(dataPath, 'r');
        celldata = textscan(fid, '%f', 'Delimiter', '\t');
        fclose(fid);
        testIndicatorMatrix = reshape(celldata{1}, numService, numUser);
        testIndicatorMatrix = testIndicatorMatrix';
        [testVec(:, 1), testVec(:, 2)] = find(testIndicatorMatrix);		
		realVec = matrix(sub2ind(size(matrix), testVec(:,1), testVec(:,2)));
        predVec = predictedMatrix(sub2ind(size(matrix), testVec(:,1), testVec(:,2)));
      
        % Evaluation metrics: MAE, NMAE, RMSE, MRE, NPRE
        [MAE, NMAE, RMSE, MRE, NPRE] = errMetric(realVec, predVec);
        evalResults(k, :) = [MAE, NMAE, RMSE, MRE, NPRE];

        logger(sprintf('%d-round done. Running time: %f s', k, timeResults(k)));
        logger('----------------------------------------------');  
    end
          
    saveResult(outPath, evalResults);
    if saveTimeInfo
        saveResult(strcat(outPath, '_time.txt'), timeResults, 'time');
    end
    logger(sprintf('Config density = %.2f done. Running time: %f s', density, toc(startTime)));
    logger('===============================');
end
% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
% Function for averagefill method: predict = globalAverage + userBias + itemBias
function [predictMatrix] = compAveragefill(trainMatrix, indicatorMatrix)

    numUser = size(trainMatrix, 1);
    numService = size(trainMatrix, 2);
    globalAverage = sum(trainMatrix(:)) / (sum(indicatorMatrix(:)) + eps); % avoid divide-by-zero exception
    
    columnSum = sum(indicatorMatrix, 1) + eps;
    rowSum = sum(indicatorMatrix, 2) + eps; 
    userAverage = sum(trainMatrix, 2) ./ rowSum;
    itemAverage = sum(trainMatrix, 1) ./ columnSum;
    userBias = sum((trainMatrix - repmat(itemAverage, numUser, 1)) .* indicatorMatrix, 2) ./ rowSum;
    itemBias = sum((trainMatrix - repmat(userAverage, 1, numService)) .* indicatorMatrix, 1) ./ columnSum;
    predictMatrix = globalAverage + repmat(userBias, 1, numService) + repmat(itemBias, numUser, 1);
end
% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
% Function to compute the evaluation metrics
function [mae, nmae, rmse, mre, npre] = errMetric(realVec, predVec)
    % get mae, nmae, rmse, mre, npre
    absError = abs(predVec - realVec);  
    mae = sum(absError)/length(absError);
    nmae = mae / (sum(realVec) / length(absError));
    rmse = sqrt(absError' * absError / length(absError));
    relativeError = absError ./ realVec;
    relativeError = sort(relativeError);
    mre = median(relativeError);
    npre = relativeError(floor(0.9*length(relativeError)));  
end
% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


