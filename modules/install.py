#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@created: 17.08.20
@author: felix
"""
import pathlib
import subprocess
import time


def install():
    module = pathlib.Path(__file__).parent.joinpath('strongtyping_modules.pyx')

    try:
        import Cython
    except ImportError:
        print('\n\t\t!!!! Installing Cython !!!!\n')
        print('\n\t\tin...')

        for i in range(1, 5):
            print(f'\n\t\t...{i}')
            time.sleep(1)

        p = subprocess.Popen(['pip', 'install', '--user', 'Cython'])
        p.wait()

    p = subprocess.Popen(['cythonize', '-a', '-i', str(module)])
    p.wait()


if __name__ == '__main__':
    install()
