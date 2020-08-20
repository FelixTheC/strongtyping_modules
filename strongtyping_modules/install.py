#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@created: 17.08.20
@author: felix
"""
import pathlib
import subprocess


def install():
    module = pathlib.Path(__file__).parent.joinpath('strongtyping_modules.pyx')
    p = subprocess.Popen(['cythonize', '-a', '-i', str(module)])
    p.wait()


if __name__ == '__main__':
    install()
