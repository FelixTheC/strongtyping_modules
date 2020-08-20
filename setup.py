from setuptools import find_packages
from setuptools import setup

try:
    from Cython.Build import cythonize

    ext_modules = cythonize('strongtyping_modules/strongtyping_modules.pyx',
                            compiler_directives={'language_level': 3, 'embedsignature': True})
except (ImportError, NameError):
      ext_modules = None


setup(name='strongtyping_modules',
      version='0.0.4.4b',
      description='Utils for strongtyping.',
      author='Felix Eisenmenger',
      author_email='fberndt87@gmail.com',
      ext_modules=ext_modules,
      zip_safe=False,
      packages=find_packages(),
      package_data={
            '': ('*.pyx', '*.c'),
      },
      classifiers=[
            'Programming Language :: Python :: 3',
            'License :: OSI Approved :: MIT License',
      ],
      install_requires=[
            'Cython==0.29.21'
      ]
      )
