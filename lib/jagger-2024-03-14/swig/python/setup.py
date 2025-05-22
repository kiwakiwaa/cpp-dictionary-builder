#!/usr/bin/env python
import re
from distutils.core import setup, Extension

cflags = ['-DHAVE_CONFIG_H']
if re.search (r'^#define USE_SSE4_2_POPCNT 1$', open ("../../config.h").read (), re.MULTILINE):
    cflags += ['-msse4.2']

setup (name = "jdepp-python",
       py_modules = ['jdepp'],
       ext_modules = [Extension ('_jdepp',
                                 ['jdepp_wrap.cxx'] + ['../../src/' + cc for cc in ['timer.cc', 'classify.cc', 'kernel.cc', 'pdep.cc']], # linear.cc'
                                 include_dirs=['../..', '../../src'],
                                 libraries=['mecab'],
                                 extra_compile_args=cflags)])
