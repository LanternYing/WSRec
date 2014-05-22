########################################################
# setup.py 
# setup script to build extended Cython module
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

ext_modules = [Extension('NIMF',
				['src/NIMF_core/NIMF.pyx', 
				'src/NIMF_core/NIMF_core.cpp',
				'src/NIMF_core/UIPCC_core.cpp'],
				language='c++'
              )]

setup(
	cmdclass = {'build_ext': build_ext},
	ext_modules = ext_modules
)

shutil.move('NIMF.so', 'src/NIMF.so')
print('==============================================')
print('Build done.\n')


