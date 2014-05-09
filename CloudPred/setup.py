########################################################
# setup.py 
# setup script to build extension model for UIPCC_core 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/4/20
# Last updated: 2014/5/8
########################################################


import sys
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import shutil

print('Build extension modules...')
print('==============================================')

ext_modules = [Extension('CloudPred',
				['src/CloudPred_core/CloudPred.pyx', 
				'src/CloudPred_core/NMF_core.cpp',
				'src/CloudPred_core/UIPCC_core.cpp'],
				language='c++'
              )]

setup(
	name = 'Extended Cython module for CloudPred',
	cmdclass = {'build_ext': build_ext},
	ext_modules = ext_modules
)

shutil.move('CloudPred.so', 'src/CloudPred.so')
print('==============================================')
print('Build done.\n')


