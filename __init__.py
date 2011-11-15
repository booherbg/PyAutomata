# Detecting pypy, thanks pypy-dev!
try:
    import __pypy__
except ImportError:
    __pypy__ = None

if __pypy__:
    # Import pure python for pypy to munch on
    import automata as pya
else:
    # Import cython and C++ bindings for cpython to munch on
    import pyAutomata as pya
pyautomata = pya.pyautomata
