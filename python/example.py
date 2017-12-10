import numpy as np
import discrete_ars
import scipy.stats
import matplotlib.pyplot as plt

mu = 100.0
y  = 110.0
eps = 0.2

def logp(k):
    return scipy.stats.poisson.logpmf(int(k), mu) + np.abs(k-y)*np.log(eps)

n = 1e6

bounds = np.array([0, np.inf])
points = np.array([mu/2,
                   2*mu,
                   y]).round()

discrete_ars.seed(1)
samples = discrete_ars.sample(logp, bounds, points, n, use_py_rand=True)
print samples.mean()

do_plot = False

if do_plot:
    plt.hist(samples, bins=np.arange(0, np.max(samples)), edgecolor='none')
    plt.show()
