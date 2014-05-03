from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import shutil

ext_modules = [Extension('UIPCC',
				['src/UIPCC_core/UIPCC.pyx', 'src/UIPCC_core/UIPCC_core.cpp'],
				language='c++'
              )]

setup(
	name = 'Extended Cython module for UIPCC',
	cmdclass = {'build_ext': build_ext},
	ext_modules = ext_modules
)

shutil.move('UIPCC.so', 'src/UIPCC.so')