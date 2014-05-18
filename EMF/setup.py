########################################################
# setup.py 
# setup script to build extension model
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

ext_modules = [Extension('EMF',
				['src/EMF_core/EMF.pyx', 
				'src/EMF_core/EMF_core.cpp',
				'src/EMF_core/UIPCC_core.cpp'],
				language='c++'
              )]

setup(
	name = 'Extended Cython module for EMF',
	cmdclass = {'build_ext': build_ext},
	ext_modules = ext_modules
)

shutil.move('EMF.so', 'src/EMF.so')
print('==============================================')
print('Build done.\n')


