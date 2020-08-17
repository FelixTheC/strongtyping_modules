from setuptools import find_packages
from setuptools import setup

try:
    from Cython.Build import cythonize

    ext_modules = cythonize('modules/strongtyping_modules.pyx',
                            compiler_directives={'language_level': 3, 'embedsignature': True})
except ImportError:
    ext_modules = None


setup(name='strongtyping_modules',
      version='0.0.3b',
      description='Utils for strongtyping.',
      author='Felix Eisenmenger',
      author_email='fberndt87@gmail.com',
      ext_modules=cythonize('strongtyping_modules.pyx',
                            compiler_directives={'language_level': 3, 'embedsignature': True}),
      zip_safe=False,
      packages=find_packages(),
      classifiers=[
            'Programming Language :: Python :: 3',
            'License :: OSI Approved :: MIT License',
      ],
      )
