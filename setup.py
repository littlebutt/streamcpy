from distutils.core import setup, Extension

module1 = Extension('streampy',
                    sources = ['streampy.c'])

setup (name = 'streampy',
       version = '1.0',
       description = 'This is a demo package',
       ext_modules = [module1])