% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
% Function to run the PMF method at each density for serveral rounds
function [] = PMF( matrix, dataFolder, outPath, density, paraStruct )

    startTime = tic;
    numService = size(matrix, 2); % number of services
    numUser = size(matrix, 1);  % number of users
    rounds = paraStruct.rounds;
    logger(sprintf('Data matrix size:  %d users ¡Á %d services', numUser, numService));
    logger(sprintf('Run PMF for %d rounds: matrix density = %.2f.', rounds, density));
    evalResults = zeros(rounds, 5);  
    timeResults = zeros(rounds, 1);
    
    for k = 1 : rounds
        logger('----------------------------------------------');
        logger(sprintf('%d-round starts.', k));
        logger('----------------------------------------------');

        % read the training data, i.e., removed matrix
        trainFile = sprintf('%s%.2f/round_%02d_train.txt', dataFolder, density, k);
        logger(sprintf('Load training data: %s', trainFile));
        fid = fopen(trainFile, 'r');
        celldata = textscan(fid, '%f', 'Delimiter', '\t'); % textscan is faster than load function
        fclose(fid);
        trainIndicatorMatrix = reshape(celldata{1}, numService, numUser);
        trainIndicatorMatrix = trainIndicatorMatrix';
        trainMatrix = matrix .* trainIndicatorMatrix; 
        
        % gradient descent
        iterStartTime = tic; % to record the running time for one round             
        [U, S] = gradDescent(trainMatrix, paraStruct);  % Gradient descent 
        predictedMatrix = U' * S;
        timeResults(k) = toc(iterStartTime); 
        
        % calculate the prediction error
        testFile = sprintf('%s%.2f/round_%02d_test.txt', dataFolder, density, k);
        logger(sprintf('Load testing data: %s', testFile));
        fid = fopen(testFile, 'r');
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
    
    outFile = sprintf('%s%.2f.txt', outPath, density);
    saveResult(outFile, evalResults);
    if paraStruct.saveTimeInfo
        saveResult(strcat(outPath, '_time.txt'), timeResults, 'time');
    end
    logger(sprintf('Config density = %.2f done. Running time: %f s', density, toc(startTime)));
    logger('===============================');
    
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



