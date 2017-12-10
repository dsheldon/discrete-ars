% DISCRETE_ARS Discrete adaptive rejection sampler
%   samples = discrete_ars( logp, bounds, points, nsamples)
%
% INPUT ARGUMENTS
%
%   logp      Anonymous function to compute the log pmf   
%	      
%   bounds    A two-element vector. The pmf support is the set of all k
%             such that bounds(1) <= k <= bounds(2). Use -Inf and +Inf to
%             specify infinite support.
%	      
%   points    The starting points for the ARS routine. Can be empty if the
%             support is finite. If unbounded below, at least one point
%             smaller than the mode is required (log-pmf is
%             increasing). If unbounded above, at least one point greater
%             than the mode is required (log-pmf is decreasing).
%   	      
%   nsamples  How many samples to return.
%
% RETURN VALUE
%
%   samples   A vector of numbers drawn from the distribution.
