'''
Same as test_suite.py but it only uses the module, not both python and c++

this can be ran with pypy (which will load python) or cpython (which will use c++)

'''
import __init__ as pa
if pa.__pypy__:
    print "found pypy: using pure python"
else:
    print "pypy not found: using c++ bindings"

a = pa.pyautomata(110, 16) #Python
#b = pb.pyautomata(110, 16) #cython

a.init_seed()
#b.init_seed()

#assert a.chunks_FromCurrentGeneration(8) == b.chunks_FromCurrentGeneration(8)
print 'chunks_FromCurrentGeneration(8) passes'

a.init_seed((42, 42, 42))
#b.init_seed((42, 42, 42))
#assert a.chunks_FromCurrentGeneration(8) == b.chunks_FromCurrentGeneration(8)
assert a.chunks_FromCurrentGeneration(8) == [42, 42]
print "init_seed([(42, 42, 42)) passes"

for i in xrange(10):
    a.iterateAutomata()
    #b.iterateAutomata()
#assert a.chunks_FromCurrentGeneration(8) == b.chunks_FromCurrentGeneration(8)
print '10 generations passes'

for i in xrange(1000):
    a.iterateAutomata()
    #b.iterateAutomata()
#assert a.chunks_FromCurrentGeneration(8) == b.chunks_FromCurrentGeneration(8)
print '1000 generations passes'

a.printBuffer()
print '---------------'
#b.printBuffer()
