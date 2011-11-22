'''
    Import the python engine, but use python if we find pypy, c++ if we find
    cpython.
    
    To override, use set_cpp() and set_python(). For example, if you want to see
    just how much faster pypy is over cpython for pure python, use set_python()
    to force cpython to use the python version of this library!
    
    Thanks pypy-dev for the ideas on how to find pypy
'''
# By default, use c++ unless we find pypy. Override with set_python()

def autodetect():
    '''
        Detect if we are using pypy or python, act accordingly
        
        If using cpython: use c++
        If using pypy: use python
        
        To override, use set_cpp() or set_python()
    '''
    try:
        import __pypy__
    except ImportError:
        __pypy__ = None
        
    if __pypy__:
        # Import pure python for pypy to munch on
        set_python()
    else:
        # Import cython and C++ bindings for cpython to munch on
        set_cpp()

def set_cpp():
    '''
        Set cpp...
    '''
    global pyautomata, interface
    import pyAutomata as pya
    pyautomata = pya.pyautomata
    interface = "C++"
    
def set_python():
    '''
        Set python...
    '''
    global pyautomata, interface
    import automata as pya
    pyautomata = pya.pyautomata
    interface = "PYTHON"
    
    
pyautomata = None
interface = ''
autodetect()