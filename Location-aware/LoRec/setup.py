########################################################
# setup.py 
# setup script to build extension model for LoRec_core 
# Author: Jamie Zhu <jimzhu@GitHub>
# Created: 2014/4/20
# Last updated: 2014/5/23
########################################################


import sys
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import shutil

print('Build extension modules...')
print('==============================================')

ext_modules = [Extension('LoRec',
				['src/LoRec_core/LoRec.pyx', 'src/LoRec_core/LoRec_core.cpp'],
				language='c++'
              )]

setup(
	cmdclass = {'build_ext': build_ext},
	ext_modules = ext_modules
)

shutil.move('LoRec.so', 'src/LoRec.so')
print('==============================================')
print('Build done.\n')


