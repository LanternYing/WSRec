########################################################
# setup.py 
# setup script to build extension model for LACF_core 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/4/20
# Last updated: 2014/5/10
########################################################


import sys
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import shutil

print('Build extension modules...')
print('==============================================')

ext_modules = [Extension('LACF',
				['src/LACF_core/LACF.pyx', 'src/LACF_core/LACF_core.cpp'],
				language='c++'
              )]

setup(
	name = 'Extended Cython module for LACF',
	cmdclass = {'build_ext': build_ext},
	ext_modules = ext_modules
)

shutil.move('LACF.so', 'src/LACF.so')
print('==============================================')
print('Build done.\n')


