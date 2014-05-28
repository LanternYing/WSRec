########################################################
# setup.py 
# setup script to build extension model for RegionKNN_core 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/4/20
# Last updated: 2014/5/26
########################################################


import sys
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import shutil

print('Build extension modules...')
print('==============================================')

ext_modules = [Extension('RegionKNN',
				['src/RegionKNN_core/RegionKNN.pyx', 'src/RegionKNN_core/RegionKNN_core.cpp'],
				language='c++'
              )]

setup(
	cmdclass = {'build_ext': build_ext},
	ext_modules = ext_modules
)

shutil.move('RegionKNN.so', 'src/RegionKNN.so')
print('==============================================')
print('Build done.\n')


