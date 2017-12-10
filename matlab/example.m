
lambda = 100;
bounds = [0 inf];
points = [50 200];
logp = @(k) (k.*log(lambda) - lambda - gammaln(k+1));

samples = discrete_ars( logp, bounds, points, 1000000);

bins = (1:ceil(3*lambda))';
n = hist(samples, bins);

p = exp(logp(bins));

bar(bins, [n(:)/sum(n) p/sum(p)]);