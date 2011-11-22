'''
pyAutomata testbox

to run and test:
python testboyx.py

this is basically a small unit test for pyAutomata.so

This is a fun one!


benchmark information!
pure python (pypy) vs pure python (cpython) vs c++ (cython)

pure python with standard cpython 2.6.5
---------------------------------
time python print_ca.py 30 10000 500 --python &>/dev/null
    user: 36.8sec
    
pure python with pypy (trunk [PyPy 1.6.0 with GCC 4.4.3])
time pypy print_ca.py 30 10000 500 --python &>/dev/null
    user: 1.140sec (wow!)
    
c++ with cython bindings (equivalent behavior)
time python print_ca.py 30 10000 500 &>/dev/null
    user: .290sec 
    
So the C++ is much much faster (~100x) than pure python. But only about 10x faster
than pypy. As it turns out, PyEvolve with pypy is at least that fast versus the
same algorithm with cpython, so we'll see which turns out to be overall faster!


'''
import sys
import __init__ as PyAutomata
if "--python" in sys.argv:
    PyAutomata.set_python()
else:
    # No need to call since it's already called on import, but useful for informational purposes
    PyAutomata.autodetect()
pyautomata = PyAutomata.pyautomata

def usage():
    print '%s <ca_rule> <ca_width> <num_generations> [--python] [--quiet]' % (sys.argv[0])
    print "--quiet will not print to console, useful for benchmarks"
    print "--python to force the use of the pure python library"
    print "a useful benchark is this:"
    print "python print_ca.py 30 1000 1000000 --quiet"
    print ""
    print "For rule 110, 1000 cells wide, 20 generations:"
    print ' ex: %s 110 1000 20' % (sys.argv[0])
    print 'python print_ca.py 30 10000 10000 --python for an animation, haha'
    print ' expected output:'
    print '''Initializing automaton... (rule: 30)
|                              #                             |
|                             ###                            |
|                            ##  #                           |
|                           ## ####                          |
|                          ##  #   #                         |
|                         ## #### ###                        |
|                        ##  #    #  #                       |
|                       ## ####  ######                      |
|                      ##  #   ###     #                     |
|                     ## #### ##  #   ###                    |
|                    ##  #    # #### ##  #                   |
|                   ## ####  ## #    # ####                  |
|                  ##  #   ###  ##  ## #   #                 |
|                 ## #### ##  ### ###  ## ###                |
|                ##  #    # ###   #  ###  #  #               |
|               ## ####  ## #  # #####  #######              |
|              ##  #   ###  #### #    ###      #             |
|             ## #### ##  ###    ##  ##  #    ###            |
|            ##  #    # ###  #  ## ### ####  ##  #           |
|           ## ####  ## #  ######  #   #   ### ####          |
|          ##  #   ###  ####     #### ### ##   #   #         |
|         ## #### ##  ###   #   ##    #   # # ### ###        |
'''

rule,width,generations = 0,0,0

if "--help" in sys.argv:
    usage()
    sys.exit(0)

try:
    rule = int(sys.argv[1])
except IndexError:
    rule = 30

try:
    width = int(sys.argv[2])
except IndexError:
    width = 60

try:
    generations = int(sys.argv[3])
except IndexError:
    generations = 30

print "using pyautomata interface: %s" % (PyAutomata.interface)

pa = pyautomata(rule, width)
#print "|%s|" % pa.stringFromCurrentGeneration()
pa.init_seed(0) # initialize with single "on" bit

# for a more complicated seed:
#pa.init_seed([2**16-1,0,2**16-1,0, 2**16-1, 0, 2**16, 0, 2**14+1])

if not "--quiet" in sys.argv:
    print "|%s|" % pa.stringFromCurrentGeneration()
#pa.iterateAutomata()
#print "|%s|" % pa.stringFromCurrentGeneration()
for i in range(generations-1):
    pa.iterateAutomata()
    if not "--quiet" in sys.argv:
        print "|%s|" % pa.stringFromCurrentGeneration()
#    print pa.chunks_FromCurrentGeneration(8)
