########################################################
# setup.py 
# setup script to build extension model for HMF_core 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/4/20
# Last updated: 2014/5/18
########################################################


import sys
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import shutil

print('Build extension modules...')
print('==============================================')

ext_modules = [Extension('HMF',
				['src/HMF_core/HMF.pyx', 'src/HMF_core/HMF_core.cpp'],
				language='c++'
              )]

setup(
	cmdclass = {'build_ext': build_ext},
	ext_modules = ext_modules
)

shutil.move('HMF.so', 'src/HMF.so')
print('==============================================')
print('Build done.\n')


