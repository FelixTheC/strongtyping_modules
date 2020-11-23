#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@created: 17.08.20
@author: felix
"""
import pathlib
import subprocess


def install(filename: str = 'strongtyping_modules.pyx'):
    module = pathlib.Path(__file__).parent.joinpath(filename)
    p = subprocess.Popen(['cythonize', '-a', '-i', '-3', str(module)])
    p.wait()
