'''
A ballsy pure python implementation for experimenting with pypy
'''
import math
import random

# constants, defined in Automata.h
BITS_PER_SEED_VALUE = 2

kNumberOfRules = 8;
kAutomataGenerationLength = 64;#160 //divisible by 8 please (for byte chunking)
                                                                # or divisible by <organism_geneLength>
kAutomataGenerations = 50; #240

#Defaults for Class Constructor
kDefaultRule = 30;
kDefaultSeedStartingPosition =kAutomataGenerationLength/2 - 1; # center
kDefaultSeedInitializerIsRandom =        0;
kDefaultShouldSeedFromEndOfRef =        0;
kDefaultSeedMode =             0;
kDefaultSeed =                 1360;

# Universe Parameters
kLowBoundOnNeighborhood =        0;
kHighBoundOnNeighborhood =    7;
kLowBoundOnOutput =            0;
kHighBoundOnOutput =            1;
kOffPixel =                            0;
kOnPixel =                            1;
kOffPixelChar =                        ' ';
kOnPixelChar =                        '#';
kOffPixelInteger =            0;
kOnPixelInteger =                1; 
kAutomataBadNeighborhoodBoundsException =    "Bad neighborhood bounds!";
kAutomataBadOutputBoundsException =        "Bad output bounds!";
kAutomataBadStringLengthException =        "Automata generation is too long!";

TIME_BASED_SEED =        1;
SRAND_SEED =             1123581321;

#typedef
# AutomataRule = bitset< kNumberOfRules >
# AutomataGenerations = vector< AutomataGeneration >

class kAutomataBadNeighborhoodBoundsException(Exception): pass
class kAutomataBadOutputBoundsException(Exception): pass
class kAutomataBadStringLengthException(Exception): pass

class AutomataGeneration(object):
    generationLength = 0 #int
    _vector = [] # vector<bool>
    def __init__(self, generationLength=kAutomataGenerationLength):
        self.generationLength = generationLength
        self._vector = [0]*generationLength
    def reset(self):
        for i in xrange(self.generationLength):
            self._vector[i] = 0
    def clear(self, index):
        self._vector[index] = 0
    def flip(self, index):
        self._vector[index].flip()
    def set(self, index):
        self._vector[index] = 1
    def size(self):
        return len(self._vector)
    def __len__(self):
        return self.size()
    def __repr__(self):
        return '%s' % (''.join(map(str, self._vector)))
    def __str__(self):
        return self.__repr__()
    def __getitem__(self, pos):
        return self._vector[pos]
    def __setitem__(self, pos, val):
        self._vector[pos] = val
    
class Automata(object):
    # public
    #typedev bitset <10> aaa; # unused?
    p_rule = 30
    p_seedIsRandom = 1
    p_seedStartingPosition = 0
    p_generationLength = 150
    
    #private
    _currentIndex = 0
    _rule = [0]*kNumberOfRules
    _generations = []
    
    def __init__(self, rule, caWidth):
        print "Initializing automaton... (rule: %d)" % rule
        self.p_rule = rule
        self.p_generationLength = caWidth
        
        self._currentIndex = -1
        self._overallIndex = -1
        self._generations = [AutomataGeneration()]*kAutomataGenerations # list of AutomataGenerations of size kAutomataGenerations
        self.setBinaryRuleFromInteger(rule)
        
        # initialize self._generations
        
        if TIME_BASED_SEED:
            random.seed()
        else:
            random.seed(SRAND_SEED)
            
    def init_copy(self, ref, seedFromEndOfRef):
        self._currentIndex = ref._currentIndex
        _overallIndex = ref._overallIndex
        for i in xrange(kNumberOfRules):
            self._rule[i] = ref._rule[i]
            
        if (seedFromEndOfRef):
            self._generations[0] = ref._generations[kAutomataGenerations-1]
        else:
            for i in xrange(kAutomataGenerations):
                self._generations.append(ref._generations[i])
        
    def init_seed(self, *args):
        if len(args) == 0:
            mode = kDefaultSeedMode
            value = kDefaultSeed
            position = kDefaultSeedStartingPosition
        elif len(args) == 1:
            mode = args[0]
            value = kDefaultSeed
            position = kDefaultSeedStartingPosition
        elif len(args) == 2:
            #@TODO this is because of the bitset
            # Special init_seed with full value
            if type(args[0]) == type([]) or type(()):
                self.init_seed_values(args[0])
                return
            else:
                mode = args[0]
                value = args[1]
                position = kDefaultSeedStartingPosition
        elif len(args) == 3:
            mode, value, position = args
            
        self.p_seedIsRandom = 0
        self.p_seedStartingPosition = kDefaultSeedStartingPosition
        
        g_seed = AutomataGeneration(self.p_generationLength)
        
        if (mode == 0):
            self.initializeSeedAtPosition(g_seed, self.p_generationLength/2)
        elif (mode == 1):
            self.randomizeSeed(g_seed)
            self.p_seedIsRandom=1
        elif (mode == 2):
            self.initializeSeedWithValue(g_seed, value, position)
        elif (mode == 3):
            del g_seed
            raise 101
        else:
            del g_seed
            raise 101
        
        self.validateGeneration(g_seed)
        self.appendGeneration(g_seed)
        
    def init_seed_values(self, values):
        n = len(values)
        self.p_seedIsRandom = 0
        self.p_seedStartingPosition = kDefaultSeedStartingPosition
        self._currentIndex = -1
        self._overallIndex = -1
        g_seed = AutomataGeneration(self.p_generationLength)
        position = 0 # hard coded, heads up. originally based on sizeof
        for i in xrange(n):
            value = values[i]
            self.initializeSeedWithValue(g_seed, value, position)
            position += 4*BITS_PER_SEED_VALUE
        self.validateGeneration(g_seed)
        self.appendGeneration(g_seed)
        
    def initializeSeedWithValue(self, seed, value, position):
        n = int(math.ceil(math.log(value, 2)))
        n = 4*BITS_PER_SEED_VALUE #unsigned int*4
        bit = 0
        
        if (n > len(seed)):
            print "truncating seed, too large"
            print "%d bits truncated to %d bits" % (n, len(seed))
            n = len(seed)
        if (position > len(seed) - n):
            position = len(seed) - n
        
        l = reversed(range(1, n+1))
        for i in l:
            bit = value%2
            if (bit == 1):
                seed.set(i+position-1)
            value = value / 2
            
    def __str__(self):
        return self.stringFromGeneration(AutomataGeneration(self.p_generationLength))
        
    def __equals__(self, ref):
        self._currentIndex = ref._currentIndex
        self._overallIndex = ref._overallIndex
        self._rule = ref._rule
        self._generations = ref._generations
        return self
    def iterateAutomata(self):
        current = AutomataGeneration(self.p_generationLength)
#        next = AutomataGeneration(self.p_generationLength)
        
        if self._currentIndex == -1:
            self.init_seed()
        
        current = self.generationAtIndex(self._currentIndex)
        next = self.getNextGeneration(current)
        self.validateGeneration(next)
        self.appendGeneration(next)
        
    def printBuffer(self):
        max_digits = 7
        if (self._currentIndex == -1):
            print "*** Warning: CA not seeded, please use init_seed ***"
            return
        test = AutomataGeneration(self.p_generationLength)
        for i in xrange(kAutomataGenerations):
            test = self.generationAtIndex(i)
            if i == 0:
                digits = 0
            else:
                digits = int(math.log10(i))
            if (digits > max_digits):
                digits = max_digits
            if (digits < 0):
                digits = 0
            print " "*(max_digits-digits),
            print "%d|%s|" % (i, self.stringFromGeneration(test))
            
    def fillBuffer(self):
        for counter in xrange(kAutomataGenerations-1):
            self.iterateAutomata()
            
    def currentGenerationIndex(self):
        return self._currentIndex
    
    def stringFromGeneration(self, g):
        st = []
        for i in xrange(len(g)):
            if g[i] == kOffPixel:
                st.append(kOffPixelChar)
            else:
                st.append(kOnPixelChar)
        return ''.join(st)
    
    def stringFromCurrentGeneration(self):
        #g = AutomataGeneration(self.p_generationLength)
        if self._currentIndex == -1:
            print "***Warning: CA not seeded, please use init_seed ***"
            return ''
        
        g = self.getCurrentGeneration()
        s = self.stringFromGeneration(g)
        return s
            
    def bStringFromGeneration(self, g):
        return str(g)
    
    def bStringFromCurrentGeneration(self):
#        import pdb;pdb.set_trace()
        if (self._currentIndex == -1):
            print "*** Warning: CA not seeded, please use init_seed ***"
            return ''
        g = self.getCurrentGeneration()
        s = self.bStringFromGeneration(g)
        return s
    
    def chunks_FromGeneration(self, g, n):
        k,num,i,j = (0,0,0,0)
        numchunks_out = len(g)/n
        princess = [0]*numchunks_out #unsigned long
        while i < len(g):
            num=0
            for j in xrange(n):
                if (i >= len(g)):
                    break
#                print "%d < %d" % (i, len(g))
                num += g[i]*math.pow(2, (n-1)-j)
                i += 1
            if (k >= numchunks_out):
                pass
            else:
                princess[k] = int(num)
            k += 1
        return princess
    
    def chunks_FromCurrentGeneration(self, n):
        g = self.getCurrentGeneration()
        if (self._currentIndex == -1):
            print "*** Warning: CA not seeded, please use init_seed ***"
            return 0
        c = self.chunks_FromGeneration(g, n)
        return c
    
    def currentIndex(self):
        return self._currentIndex
    
    def overallIndex(self):
        return self._overallIndex
        
    def getGeneration(self, index):
        return self._generations[index]
    
    def getCurrentGeneration(self):
        if (self._currentIndex == -1):
            print "*** Warning: CA not seeded, please use init_seed ***"
            return
        return self.getGeneration(self._currentIndex)
        
    def generationAtIndex(self, index):
        return self.getGeneration(index)
    
    def appendGeneration(self, g):
        self._overallIndex += 1
        self._currentIndex = (self._currentIndex + 1) % kAutomataGenerations
        self._generations[self._currentIndex] = g #copy
        
    def randomizeSeed(self, seed):
        for i in xrange(self.p_generationLength):
            if random.randint(0, 1024) % 2 == 0:
                seed.clear(i)
            else:
                seed.set(i)
                
    def initializeSeedAtPosition(self, seed, position):
        seed.reset()
        seed.set(position)
        
            
    def setBinaryRuleFromInteger(self, numericalCode):
#        x,y = (0,0)
#        import pdb;pdb.set_trace()
        y = kNumberOfRules-1
        while (y >= 0):
            x = numericalCode / (1 << y)
            numericalCode = numericalCode - x*(1<<y)
            if (x == 0):
                self._rule[y] = kOffPixel
            else:
                self._rule[y] = kOnPixel
                
#            print "_rule[%d]: %d" % (y, self._rule[y])
            y -= 1
    
    def getNextGeneration(self, cg):
        ng = AutomataGeneration(self.p_generationLength)
        output, neighborhood = (0,0)
        for position in xrange(len(cg)):
            neighborhood = self.getNeighborhoodForGenerationAtPosition(cg, position)
            self.validateNeighborhood(neighborhood) #redundant
            output = self.getOutputForNeighborhood(neighborhood)
            self.validateOutput(output) #redundant
            if (output == 0):
                ng.clear(position)
            else:
                ng.set(position)
        return ng
                
    def getOutputForNeighborhood(self, neighborhood):
        self.validateNeighborhood(neighborhood)
        output = self._rule[neighborhood]
        self.validateOutput(output)
        return output
    
    def getNeighborhoodForGenerationAtPosition(self, cg, position):
        if (cg[position] == kOffPixel):
            center = kOffPixelInteger
        else:
            center = kOnPixelInteger
        left = 0
        if (position == 0):
            if (cg[-1] == kOffPixel):
                left = kOffPixelInteger
            else:
                left = kOnPixelInteger     
        else:
            if (cg[position-1] == kOffPixel):
                left = kOffPixelInteger
            else:
                left = kOnPixelInteger
        
        right = 0
        if (position == len(cg)-1):
            if (cg[0] == kOffPixel):
                right = kOffPixelInteger
            else:
                right = kOnPixelInteger     
        else:
            if (cg[position+1] == kOffPixel):
                right = kOffPixelInteger
            else:
                right = kOnPixelInteger
                
        neighborhood = (1*right) + (2*center) + (4*left)
        self.validateNeighborhood(neighborhood)
        return neighborhood
    
    def validateNeighborhood(self, neighborhood):
        if ((neighborhood < kLowBoundOnNeighborhood) or (neighborhood > kHighBoundOnNeighborhood)):
            raise kAutomataBadNeighborhoodBoundsException()
        
    def validateOutput(self, output):
        if ((output < kLowBoundOnOutput) or (output > kHighBoundOnOutput)):
            raise kAutomataBadOutputBoundsException()
        
    def validateGeneration(self, g):
        if (len(g) > self.p_generationLength):
            raise kAutomataBadStringLengthException()
        
if __name__ == "__main__":
    generations = 10
    width = 10
    a = Automata(110, width)
    a.init_seed(0)
    print "|%s|" % a.stringFromCurrentGeneration()
    for i in xrange(generations-1):
        a.iterateAutomata()
        print "|%s|" % a.stringFromCurrentGeneration()
        print a.chunks_FromCurrentGeneration(8)