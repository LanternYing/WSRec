function [error, mae, rmse, mre, error90ile] = errMetric( errorMatrix )
% get MAE, RMSE, MRE
    error = errorMatrix(:);  
    error = error(error > 0);
    error = sort(error);
    mae = sum(error)/length(error);
    rmse = sqrt(error' * error / length(error));
    mre = median(error);
    error90ile = error(floor(0.9*length(error)));  
end
