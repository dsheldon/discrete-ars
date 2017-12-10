from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
from os.path import realpath, dirname
import numpy

parent_dir = dirname(dirname(realpath(__file__)))

setup(
    cmdclass = {'build_ext': build_ext},
    ext_modules = [Extension("discrete_ars",
                             sources=[
                                 "discrete_ars.pyx",
                                 parent_dir + "/ars.c"],
                             include_dirs=[
                                 numpy.get_include(),
                                 parent_dir
                             ])],
    url='',
    license='',
    author='Dan Sheldon',
    author_email='sheldon@cs.umass.edu',
    description='Discrete adaptive rejection sampling for sampling from discrete log-concave distributions'
)
