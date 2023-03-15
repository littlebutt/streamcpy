from distutils.core import setup, Extension

module1 = Extension('streamcpy',
                    sources = ['streamcpy.c'])

setup (name = 'streamcpy',
       version = '1.0',
       description = 'This is a demo package',
       ext_modules = [module1])