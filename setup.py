from setuptools import setup, Extension

module1 = Extension('streamcpy',
                    sources = ['streamcpy.c'])

with open('README.rst', 'r', encoding='utf-8') as f:
    long_description = f.read()

setup (name='streamcpy',
       version='1.0.1',
       author='littlebutt',
       author_email='luogan1996@icloud.com',
       license='GPL-3.0 license',
       description = "The Stream Api in Python",
       long_description=long_description,
       url='https://github.com/littlebutt/streamcpy',
       ext_modules=[module1])