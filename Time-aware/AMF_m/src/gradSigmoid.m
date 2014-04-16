function g_prime = gradSigmoid( x )
%% gradients of logistic function g(x)
g_prime = 1./ (2 + exp(-x) + exp(x));

end

