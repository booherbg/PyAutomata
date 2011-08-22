from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

#Boilerplate setup.py
# Call with python setup.py build_ext --inplace
# Also don't forget about python setup.py build_ext --embed
ext = Extension(
    "pyAutomata",                 # name of extension
    ["pyAutomata.pyx", "automata-1.0.0/Automata.cpp"],     # filename of our Cython source
    language="c++",              # this causes Cython to create C++ source
    include_dirs=['./automata-1.0.0'],          # usual stuff
    libraries=["stdc++"],             # ditto
    extra_link_args=["-lm"],       # if needed
    cmdclass = {'build_ext': build_ext}
    )

setup(
    cmdclass = {'build_ext': build_ext},
    #ext_modules = [Extension("_funcs_cython", ["_funcs_cython.pyx"])]
    ext_modules=[ext]
)
