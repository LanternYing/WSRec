########################################################
# setup.py 
# setup script to build extension model for NRCF_core 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/4/20
# Last updated: 2014/5/3
########################################################


import sys
import numpy
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import shutil

print('Build extension modules...')
print('==============================================')

ext_modules = [Extension('NRCF',
				['src/NRCF_core/NRCF.pyx', 'src/NRCF_core/NRCF_core.cpp'],
				language = 'c++',
				include_dirs = [numpy.get_include()]
              )]

setup(
	name = 'Extended Cython module for NRCF',
	cmdclass = {'build_ext': build_ext},
	ext_modules = ext_modules
)

shutil.move('NRCF.so', 'src/NRCF.so')
print('==============================================')
print('Build done.\n')

