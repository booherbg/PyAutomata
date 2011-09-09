'''
Compare the outputs from the pure python to the pure c++ to ensure that they 
are sane and both are implemented correctly.

The pure python is so that we can use pypy to see if it is faster with pyEvolve, 
the c++ is much quicker than all implementations in python.

'''
import automata as pa
import pyAutomata as pb

a = pa.pyautomata(110, 16) #Python
b = pb.pyautomata(110, 16) #cython

a.init_seed()
b.init_seed()

assert a.chunks_FromCurrentGeneration(8) == b.chunks_FromCurrentGeneration(8)
print 'chunks_FromCurrentGeneration(8) passes'

a.init_seed((42, 42, 42))
b.init_seed((42, 42, 42))
assert a.chunks_FromCurrentGeneration(8) == b.chunks_FromCurrentGeneration(8)
assert a.chunks_FromCurrentGeneration(8) == [42, 42]
print "init_seed([(42, 42, 42)) passes"

for i in xrange(10):
    a.iterateAutomata()
    b.iterateAutomata()
assert a.chunks_FromCurrentGeneration(8) == b.chunks_FromCurrentGeneration(8)
print '10 generations passes'

for i in xrange(1000):
    a.iterateAutomata()
    b.iterateAutomata()
assert a.chunks_FromCurrentGeneration(8) == b.chunks_FromCurrentGeneration(8)
print '1000 generations passes'

a.printBuffer()
print '---------------'
b.printBuffer()