function [U, S, exetime] = sgd( removedMatrix, paramStruct )
    global DEBUG;
    %% get the parameters
    d = paramStruct.dim;                           % dimensionality of coordinates
    eta = paramStruct.eta;                         % learning rate eta
    MAXITER = paramStruct.MAXITER;                 % max iteration rounds
    deltaError = paramStruct.deltaError;           % control the number of iterations
    lambda = paramStruct.lambda;
    belta = paramStruct.belta;

    numService = size(removedMatrix, 2);            %number of services
    numUser = size(removedMatrix, 1);                %number of users

    % get the data samples in triplets
    [idxU, idxS] = find(removedMatrix > 0);
    dataTriples = [idxU, idxS, removedMatrix(removedMatrix > 0)];  

    % initialization
    U = rand(numUser, d)';        
    S = rand(numService, d)';
    gradU = zeros(numUser, d)';
    gradS = zeros(numService, d)';
    
    eu = ones(numUser, 1);
    es = ones(numService, 1);
    
    if ismember('plotConvergFig', DEBUG)
        figure(1); hold on;
    end
    
    iter = 1;
    stop = 0;
    MRELast = inf;
    points = 0;
    iterStartTime = tic; % to record the running time
    excludeTime = 0;
       
    while iter <= MAXITER
        if stop == 1   % the objective value has converged
            break;
        end
        randDataTriples = dataTriples(randperm(size(dataTriples, 1)), :);       % randomly permutate the data samples
        for dataID = 1:length(randDataTriples)
            % one data sample
            i = randDataTriples(dataID, 1);
            j = randDataTriples(dataID, 2);
            Rij = randDataTriples(dataID, 3);
            
           % confidence update
            eij = abs(sigmoid(U(:, i)' * S(:, j)) - Rij) / Rij;
            wi = eu(i) / (eu(i) + es(j));
            wj = es(j) / (eu(i) + es(j));
            
            eu(i) = belta * wi * eij + (1 - belta * wi) * eu(i);
            es(j) = belta * wj * eij + (1 - belta * wj) * es(j);
                      
            % convergence check
            if mod(dataID, 5000) == 0 %&& iter > 30
                excludeStartTime = tic;
                predictedMatrix = sigmoid(U'*S);
                predictedMatrix (removedMatrix == -1) = -1;

                absError = abs(predictedMatrix - removedMatrix);
                relativeError = absError ./ removedMatrix; 
                relativeError(removedMatrix == -1) = -1;
                [~, ~, ~, MRE, ~] = errMetric(relativeError);

                if (iter > 50) %abs(MRELast - MRE) / MRELast < deltaError &&
                    stop = 1;
                    excludeTime = excludeTime + toc(excludeStartTime);
                    break;
                end
                logger(sprintf('cost = %f, regularization = %f, eu1 = %f, es1 = %f', (((Rij - sigmoid(U(:, i)' * S(:, j))) ./ Rij).^2),  lambda * sum(U(:, i).^2) + lambda * sum(S(:, j).^2), eu(1), es(1)), 'debug2');
                logger(sprintf('iter = %d, len = %d, points = %d, MRE = %f, diffMRE = %f', iter, dataID, points, MRE, (MRELast - MRE) / MRELast), 'debug1');
                MRELast = MRE;
                if ismember('plotConvergFig', DEBUG)
                    plot(points, MRE, '.');
                    points = points + 1;
                end         
                excludeTime = excludeTime + toc(excludeStartTime);
            end
                       
            % gradient descent update 
            gradU(:, i) =  wi * (((sigmoid(U(:, i)' * S(:, j)) - Rij).*gradSigmoid(U(:, i)' * S(:, j))) ./ (Rij.^2))' * S(:, j) + lambda * U(:, i);
            gradS(:, j) = wj * (((sigmoid(U(:, i)' * S(:, j)) - Rij).*gradSigmoid(U(:, i)' * S(:, j))) ./ (Rij.^2)) * U(:, i) + lambda * S(:, j);
            U(:, i) = U(:, i) - eta * gradU(:, i);
            S(:, j) = S(:, j) - eta * gradS(:, j);
        end
        iter = iter + 1;
    end
    exetime = toc(iterStartTime) - excludeTime;
    logger(sprintf('convergence time = %f s after %d iterations\n', exetime, iter));    
end




