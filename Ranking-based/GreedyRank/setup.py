########################################################
# setup.py 
# setup script to build extension model for GreedyRank_core 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/4/20
# Last updated: 2014/6/16
########################################################


import sys
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import shutil

print('Build extension modules...')
print('==============================================')

ext_modules = [Extension('GreedyRank',
				['src/GreedyRank_core/GreedyRank.pyx', 'src/GreedyRank_core/GreedyRank_core.cpp'],
				language='c++'
              )]

setup(
	cmdclass = {'build_ext': build_ext},
	ext_modules = ext_modules
)

shutil.move('GreedyRank.so', 'src/GreedyRank.so')
print('==============================================')
print('Build done.\n')


