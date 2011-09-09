import automata as pa
import pyAutomata as pb

a = pa.Automata(110, 16) #Python
b = pb.pyautomata(110, 16) #cython

a.init_seed()
b.init_seed()

assert a.chunks_FromCurrentGeneration(8) == b.chunks_FromCurrentGeneration(8)
print 'chunks_FromCurrentGeneration(8) passes'

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