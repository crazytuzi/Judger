# coding=utf-8
from distutils.core import setup, Extension
import os

os.system("sudo apt-get install python-dev")
os.system("sudo apt-get install libseccomp-dev")
os.system("gcc src/runner.c -o runner -lseccomp")

setup(name='Judger',
      version='1.0',
      packages=["Judger"])
