########################################################
# run_tp.py 
# Developer: Jieming Zhu <jmzhu@cse.cuhk.edu.hk>
# Created: 2014/2/6
# Last updated: 2014/2/11
# Implemented approaches: [UMEAN, IMEAN, UPCC, IPCC,  
# and UIPCC]
# Evaluation metrics: MAE, NMAE, RMSE, MRE, NPRE
########################################################


import sys
sys.path.append('src')

from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

logger.info('Build extension modules...')

ext_modules=[Extension('UIPCC_module', ['UIPCC_module.pyx'])]

setup(
    name = 'UIPCC extension module',
    cmdclass = {'build_ext': build_ext},
    ext_modules = ext_modules
)

logger.info('Build done.')
sys.path.remove('src')