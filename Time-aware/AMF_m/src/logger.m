function [] = logger( logMessage, debugLevel )
% log the runtime information for behavior monitoring
global DEBUG;
if nargin < 2
    log = true;
elseif ismember(debugLevel, DEBUG) 
        log = true;
else 
    log = false;
end

if log == true
    time = now;
    timeStr = datestr(time, 31);
    fprintf('%s: %s\n', timeStr, logMessage);    
end
end

