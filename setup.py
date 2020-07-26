from distutils.core import setup, Extension

sttypingmodule = Extension('sttyping',
                           define_macros=[('MAJOR_VERSION', '1'),
                                          ('MINOR_VERSION', '0')],
                           include_dirs=['/usr/include/python3.7'],
                           sources=['sttypingmodule.c']
                           )


setup(name="sttyping",
      version="1.0.0",
      description="Utils for strongtyping.",
      author="Felix Eisenmenger",
      author_email="fberndt87@gmail.com",
      ext_modules=[sttypingmodule]
      )
