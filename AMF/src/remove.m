function [rmvMatrix] = remove( fullMatrix, density )
% density is how many percents of the data is remained 
    numService = size(fullMatrix, 2);            %number of services
    numUser = size(fullMatrix, 1);              %number of users
    rmvMatrix = fullMatrix;

    % remove entries 
    for i = 1 : numUser
        for j = 1 : numService
                if rand() > density
                    rmvMatrix(i, j) = -1;
                end
        end
    end
end

