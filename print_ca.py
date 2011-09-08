'''
pyAutomata testbox

to run and test:
python testboyx.py

this is basically a small unit test for pyAutomata.so
'''
from pyAutomata import pyautomata
import sys

def usage():
    print '%s <rule> <width> <generations>' % (sys.argv[0])
    print ' ex: %s 30 60 20' % (sys.argv[0])
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
    rule = 109

try:
    width = int(sys.argv[2])
except IndexError:
    width = 60

try:
    generations = int(sys.argv[3])
except IndexError:
    generations = 30

pa = pyautomata(rule, width)
#print "|%s|" % pa.stringFromCurrentGeneration()
pa.init_seed(0) # initialize with single "on" bit

# for a more complicated seed:
#pa.init_seed([2**16-1,0,2**16-1,0, 2**16-1, 0, 2**16, 0, 2**14+1])
print "|%s|" % pa.stringFromCurrentGeneration()
#pa.iterateAutomata()
#print "|%s|" % pa.stringFromCurrentGeneration()
for i in range(generations-1):
    pa.iterateAutomata()
    print "|%s|" % pa.stringFromCurrentGeneration()
    print pa.chunks_FromCurrentGeneration(8)
