% produce all the data needed to perform accuracy comparison
clc; clear; close all;

% config area
density = [0.01 : 0.01 : 0.05, 0.1 : 0.05 : 0.5];
rounds = 50; % randomly remove the matrix for 50 times

% create folders
if exist('generatedData\RT', 'dir') == 0
    system('mkdir generatedData\RT');
end
for i = 1 : length(density)
    folder = sprintf('generatedData\\RT\\density_%.2f', density(i));
    if exist(folder, 'dir') == 0
        system(sprintf('mkdir %s', folder));
    end
end

%% RT processing
rtMatrix = load('rawData\rtMatrix.txt');
for i = 1 : rounds    
        [rtVec(:, 1), rtVec(:, 2)] = find(rtMatrix ~= -1);
        numRecords = size(rtVec, 1);
        numAll = numel(rtMatrix);
        s = RandStream('swb2712', 'Seed', i);
        RandStream.setDefaultStream(s); % rand seeds
		randomSequence = randperm(numRecords); % one random sequence per round
        
    % generate training data at different densities    
    for j = 1 : length(density)
        fprintf('Process RT data: round=%02d, density=%.2f\n', i, density(j));
		numTrain = floor( numAll * density(j));               
		numTest = floor( numAll * 0.5); % by default, we set the last 50% of QoS records as testing data
		if numTrain + numTest > numRecords
			numTest = numRecords - numTrain;
		end		
		trainVec = rtVec(randomSequence(1 : numTrain), :);
        testVec = rtVec(randomSequence(end - numTest + 1 : end), :);      
        trainMatrix = zeros(size(rtMatrix));
		trainMatrix(sub2ind(size(trainMatrix), trainVec(:, 1), trainVec(:, 2))) = 1;
        testMatrix = zeros(size(rtMatrix));
		testMatrix(sub2ind(size(testMatrix), testVec(:, 1), testVec(:, 2))) = 1;
        % ignore invalid testing data
        idxX = (sum(trainMatrix, 2) == 0);
        testMatrix(idxX, :) = 0;
        idxY = (sum(trainMatrix, 1) == 0);
        testMatrix(:, idxY) = 0;      
        
        % save into file
        filepath = sprintf('generatedData\\RT\\density_%.2f\\round_%02d_train.txt', density(j), i);
        fid = fopen(filepath, 'wt');
        fprintf(fid, [repmat('%d\t', 1, size(trainMatrix, 2)) '\n'], trainMatrix'); 
        fclose(fid);
        filepath = sprintf('generatedData\\RT\\density_%.2f\\round_%02d_test.txt', density(j), i);
        fid = fopen(filepath, 'wt');
        fprintf(fid, [repmat('%d\t', 1, size(testMatrix, 2)) '\n'], testMatrix'); 
        fclose(fid);
    end
end

fprintf('All done.\n');






