from setuptools import setup
from Cython.Build import cythonize


setup(name='strongtyping_modules',
      version='0.0.2b',
      description='Utils for strongtyping.',
      author='Felix Eisenmenger',
      author_email='fberndt87@gmail.com',
      ext_modules=cythonize('strongtyping_modules.pyx'),
      zip_safe=False,
      install_requires=('Cython==0.29.21', )
      )
