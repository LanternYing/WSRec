function x = argBoxcox( alpha, y )
%% the reflection of function boxcox(x)
if alpha ~= 0
    x = (alpha * y + 1) .^ (1 / alpha);
else
    x = exp(y);

end

