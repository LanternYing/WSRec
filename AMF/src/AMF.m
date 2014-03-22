function [] = AMF( matrix, normalMatrix, alpha, minValue, maxValue, outPath, dimension, density, eta, lambda, maxGdIter, rounds, deltaError, belta )

    startTime = tic;
    logger('=========================================');
    logger(sprintf('Run AMF for %d rounds: matrix density = %.2f.', rounds, density));

    numService = size(matrix, 2);            % number of services
    numUser = size(matrix, 1);                % number of users
    logger(sprintf('Data matrix size:  %d users ¡Á %d services', numUser, numService));
    evalResults = zeros(rounds, 4);  
    time = zeros(1, rounds);
    
    for k = 1 : rounds
        logger('----------------------------------------------');
        logger(sprintf('%d-round AMF starts.', k));
        logger('----------------------------------------------');
        iterStartTime = tic; % to record the running time
        paramStruct = struct('dim', dimension, 'eta', eta, 'lambda', lambda, 'MAXITER', maxGdIter, 'deltaError', deltaError, 'belta', belta);          % parameters of MF algorithms

        %% stochastic gradient descent algorithm (SGD)
        % randomly remove some entries
        removedNormalMatrix = remove(normalMatrix, density);     % density is the percentage of remained data 
        % save('removedData30.mat', 'removedL');
        
        removedNormalMatrix(removedNormalMatrix == -2) = -1;
        removedNormalMatrix(normalMatrix == -1) = -1;
        
        [U, S, time(k)] = sgd( removedNormalMatrix, paramStruct ); % sgd
        logger('AMF updating ends.');
        
        %% calculate the prediction error
        predictedNormalMatrix = sigmoid(U' * S);
        
        % recover from normalization
        predictedMatrix = predictedNormalMatrix;
        predictedMatrix = (maxValue - minValue) * predictedMatrix + minValue;
        predictedMatrix = argBoxcox(alpha, predictedMatrix);
        predictedMatrix (normalMatrix == -1) = -1;
        
        removedMatrix = matrix;
        removedMatrix(removedNormalMatrix == -1) = -1;
        predictedMatrix (removedMatrix >= 0) = removedMatrix(removedMatrix >= 0);    
        
        % regularization check: if \lambda is too large, predicted value will be zero
%         if ~isempty(find(predictedMatrix(:) == 0, 1))
%             logger('Exception: Regularization parameter is too large. Predicted value is zero.\n Program terminates.');
%             exit;
%         end

        absError = abs(predictedMatrix - matrix);
        absError(matrix == -1) = -1;
        relativeError = absError ./ matrix; 
        relativeError(matrix == -1) = -1;

        %% evaluation metrics: sortError, MAE, RMSE, MRE, RE90ile
        [~, MAE, RMSE, ~, ~] = errMetric(absError);
        [~, ~, ~, MRE, RE90ile] = errMetric(relativeError);
        evalResults(k, :) = [MAE, RMSE, MRE, RE90ile];

        logger(sprintf('One round time used: %f s', toc(iterStartTime)))
        logger(sprintf('%d-round done.', k));
        logger('----------------------------------------------');  
    end
    
    eval = mean(evalResults, 1);         
    dlmwrite(outPath, eval, 'delimiter','\t','precision','%.3f'); 
    %dlmwrite(strcat(outPath, '_time.txt'), time, 'delimiter','\t','precision','%.3f');
    logger(sprintf('One config time used: %f s', toc(startTime)))
    logger(sprintf('Cofig: density = %.2f done.', density));
    logger('=========================================');
end




