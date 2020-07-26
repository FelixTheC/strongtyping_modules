from distutils.core import setup, Extension

sttypingmodule = Extension('sttyping', sources=['sttypingmodule.c']
                           )


setup(name="sttyping",
      version="1.0.0",
      description="Utils for strongtyping.",
      author="Felix Eisenmenger",
      author_email="fberndt87@gmail.com",
      ext_modules=[sttypingmodule]
      )
