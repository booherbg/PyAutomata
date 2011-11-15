'''
Same as test_suite.py but it only uses the module, not both python and c++

this can be ran with pypy (which will load python) or cpython (which will use c++)

Cpython with c++: the fastest. Chokes on high generations (10,000,000) although
    technically that shouldn't happen.
Cpython with python: the slowest, should never really use. (10x-20x slower)
pypy with python: Only half as slow as cpython/c++, efficient memory that doesn't
    choke on high generations. Increases performance as generations increase due
    to the caching nature of pypy's JIT compiler.
    recommended in most cases except when absolute performance is needed and
    memory is not a concern.

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

print "stress test: 1,000,000 generations"
for i in xrange(1000000):
    if i % 50000 == 0:
        print "%d generations done" % i
    a.iterateAutomata()
    #b.iterateAutomata()
#assert a.chunks_FromCurrentGeneration(8) == b.chunks_FromCurrentGeneration(8)
print '1000000 generations passes'

a.printBuffer()
print '---------------'
#b.printBuffer()
