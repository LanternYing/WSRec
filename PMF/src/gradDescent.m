% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
% Function to perform iterations by gradient descent
function [U, S] = gradDescent(removedMatrix, paraStruct)
   
    numService = size(removedMatrix, 2); % number of services
    numUser = size(removedMatrix, 1); % number of users

    % initialization
    U = rand(paraStruct.dimension, numUser);        
    S = rand(paraStruct.dimension, numService); 
    
    if paraStruct.debugMode
        figure(1); hold on;
    end
    
    iter = 1;
    tic; % record the running time
    logger('Iterating...');
    lossValue = loss( U, S, removedMatrix, paraStruct );
    
    while iter < paraStruct.maxIter                          
        [gradU, gradS] = gradLoss( U, S, removedMatrix, paraStruct );    
        eta = linesearch( U, S, removedMatrix, lossValue, gradU, gradS, paraStruct);
        
        % gradient descent updates 
        U = U - eta * gradU; 
        S = S - eta * gradS;
                   
        lossValue = loss( U, S, removedMatrix, paraStruct );
        if paraStruct.debugMode
            plot(iter, lossValue, '.');
        end
        logger(sprintf('iter = %03d, loss = %.4f', iter, lossValue), paraStruct.debugMode);
        iter = iter + 1;
    end
    exetime = toc;
    logger(sprintf('convergence time: %f s, using %d iterations', exetime, iter));

end
% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
% Function to compute the loss
function lossValue  = loss( U, S, removedMatrix, paraStruct )

    lambda = paraStruct.lambda; % regularization parameter  
    % loss value computation
    I = (removedMatrix ~= 0);
    cost = sum(sum(I.*((removedMatrix - U'*S).^2)));
    regulariztion = lambda * sum(sum(U.^2)) + lambda * sum(sum(S.^2));
    lossValue = 0.5 * (cost + regulariztion);
    
end
% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
% Function to compute the gradients of loss function
function [gradU, gradS] = gradLoss( U, S, removedMatrix, paraStruct )
    % load the parameters
    d = paraStruct.dimension; % dimensionality of coordinates
    lambda = paraStruct.lambda; % regularization parameter
    numService = size(removedMatrix, 2); % number of services
    numUser = size(removedMatrix, 1); % number of users

    %% gradients computation
    % gradients of U
    gradU = zeros(d, numUser);
    I = (removedMatrix ~= 0);

    for i = 1 : numUser
        idxS = (I(i, :) == 1);
        S_tmp = S(:, idxS);
        L_tmp = removedMatrix(i, idxS);
        gradU(:, i) = S_tmp * (U(:, i)' * S_tmp - L_tmp)'  + lambda * U(:, i);
    end

    % gradients of S
    gradS = zeros(d, numService);
    for j = 1 : numService
        idxU = (I(:, j) == 1);
        L_Sj = removedMatrix(idxU, j);
        U_tmp = U(:, idxU);
        gradS(:, j) = U_tmp * (U_tmp' * S(:, j) - L_Sj) + lambda * S(:, j);
    end

end
% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
% Function to perform line search to find the best learning rate
function eta = linesearch( U, S, removedMatrix, lastLossValue, gradU, gradS, paraStruct)

    eta = paraStruct.etaInit;
    for i = 1 : 20
        U1 = U - eta * gradU; 
        S1 = S - eta * gradS;
        fValue = loss( U1 , S1, removedMatrix, paraStruct );

        if fValue <= lastLossValue 
            return;
        end
        eta = eta / 2;
    end

end
% * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


