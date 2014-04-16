function numgrad = numericalGradient(J, Coord)
%COMPUTENUMERICALGRADIENT Computes the gradient using "finite differences"
%and gives us a numerical estimate of the gradient.
%   numgrad = COMPUTENUMERICALGRADIENT(J, Coord) computes the numerical
%   gradient of the function J around Coord. Calling y = J(Coord) should
%   return the function value at Coord.

% Notes: The following code implements numerical gradient checking, and 
%        returns the numerical gradient.It sets numgrad(i) to (a numerical 
%        approximation of) the partial derivative of J with respect to the 
%        i-th input argument, evaluated at Coord. (i.e., numgrad(i) should 
%        be the (approximately) the partial derivative of J with respect 
%        to Coord(i).)
%                

numgrad = zeros(size(Coord));
perturb = zeros(size(Coord));
e = 1e-8;
for j = 1:size(Coord, 2)
    for i = 1:size(Coord, 1)
        % Set perturbation vector
        perturb(i,j) = e;
        loss1 = J(Coord - perturb);
        loss2 = J(Coord + perturb);
        % Compute Numerical Gradient
        numgrad(i,j) = (loss2 - loss1) / (2*e);
        perturb(i,j) = 0;
    end
end

end
