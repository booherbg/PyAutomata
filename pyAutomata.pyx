'''
Notes!

 /**********'-0__0-'***************************************-0__o-**************\
 |                                                                             |
 |          And remember, always: It can be easier to think of this file       |
 |              as a real compiled C++ object.  The python code is only        |
 |              a formality.  Everything here turns into cold, hard, C++       |
 |                                                                             |
 \******************************+~.._/''\_..~+*********************************/

# *** This is how to convert from string to character array ***
#Requires:
cdef extern from "<string>" namespace "std":
    cdef cppclass string:
        char* c_str()
        int size()
        
# from http://wiki.cython.org/DynamicMemoryAllocation
cdef extern from "stdlib.h":
    ctypedef unsigned long size_t
    void free(void *ptr)
    void *malloc(size_t size)
    char *strcpy(char *dest, char *src)

string str1 = self.thisptr.some_function_that_returns_std::string()
cdef char *cstr = <char*>malloc(str1.size() + 1) #+1 to compensate for \0
strcpy(cstr, str1.c_str())
print strlen(c) # Returns number of characters before \0 is hit
print sizeof(c) # Returns number of bytes...
ret = str(c) # Convert to python string, to be safe
free(c) # Free up the allocated memory, to be safe (let python handle ret via garbage collection)

# *** How to pass variable to a void func(double &var); ***
#Pass by reference is EASY! So easy that it isn't trivial to figure out!
# Define the class like normal:
cdef extern from "mylib.h" namespace "mylib":
    void func(double) # No special characters needed, just like normal c++!
#to use it:
double d=0 # Creation of double datatype
func(d) # Call funcd pass-by-reference
print d # Could print anything that func set d to!

# *** Notes on <vector> and <bitset> - they might work? ***
#cdef extern from "<vector>" namespace "std":
#    cdef cppclass vector[T]:
#        pass
#cdef extern from "<bitset>" namespace "std":
#    cdef cppclass bitset[T]:
#        pass


'''

cdef extern from "<string>" namespace "std":
    cdef cppclass string:
        char* c_str()
        string()
        string(char*)
        int size()
        
# from http://wiki.cython.org/DynamicMemoryAllocation
cdef extern from "stdlib.h":
    ctypedef unsigned long size_t
    void free(void *ptr)
    void *malloc(size_t size)
    void *realloc(void *ptr, size_t size)
    size_t strlen(char *s)
    char *strcpy(char *dest, char *src)

# Here, define what you want to expose from the c++ object
cdef extern from "Automata.h" namespace "Automata":
# For now, AutomataGeneration is a mystery.  Indirect Access Only (not a problem)
    #ctypedef struct c_index_set "IndexSet":
#    ctypedef struct bitset "AutomataGeneration":
#        bitset *new_automatageneration "new AutomataGeneration" ()
#    cdef cppclass AutomataGeneration: 
#        AutomataGeneration()
        
    cdef cppclass AutomataGenerations:
        pass
    cdef cppclass Automata:
        unsigned int p_rule,p_seedIsRandom,p_seedStartingPosition,p_generationLength
        int _overallIndex
        Automata()
        Automata(int)
        Automata(int, int)
        Automata(Automata, int)
        
        void init_seed() # mode 0
        void init_seed(unsigned int) # mode 1
        void init_seed(unsigned int, unsigned int) # mode 2
        void init_seed(unsigned int, unsigned int, unsigned int) # mode 2
        void init_seed(unsigned int[], unsigned int) # mode 3
        
        void iterateAutomata()
        void printBuffer()
        void fillBuffer()
        int currentGenerationIndex()
        
        string bStringFromCurrentGeneration()
        string stringFromCurrentGeneration()
        unsigned long *chunks_FromCurrentGeneration(unsigned int, unsigned int)
        
        void initializeCurrentGeneration(unsigned int)
        
#        void generationAtIndex(AutomataGeneration, int)
#       string stringFromGeneration(AutomataGeneration)
        #This is about to get crazy
#        Automata* operator=(Automata*)
#        void currentGeneration(AutomataGeneration)
#        void generationAtIndex(AutomataGeneration, int)

# Here, define what you want to use in the Python Class
# REMEMBER! This isn't python - this is C++!
cdef class pyautomata:
    '''
    The goal of this class is to expose the c++ library.  It is NOT, I repeat, NOT 
    meant to add new functionality.  If Automata:: needs new functions, freakin' put 
    it in the c++ class! :) -BB
    
    In order for this class to be able to say:
        self.p_rule = self.thisptr.p_rule
    define self.p_rule here with a cdef public / readonly statement. readonly means
    that outside *this* object, nothing can modify the variable
    '''
    cdef readonly unsigned int p_rule, p_seedIsRandom, p_seedStartingPosition, p_generationLength
    cdef public _overallIndex
    cdef Automata *thisptr
    def __cinit__(self, *args):
        '''
            Constructor
            Creates a pointer to the C++ object at self.thisptr
            
            We can't overload the python function __cinit__ so we use variable **kwds
            
            Automata(unsigned int rule = kDefaultRule,
             unsigned int seedIsRandom = kDefaultSeedInitializerIsRandom,
             unsigned int seedStartingPosition = kDefaultSeedStartingPosition,
             unsigned int generationLength = kAutomataGenerationLength);
        '''
        if len(args) == 0:
            self.thisptr = new Automata()
        elif len(args) == 1:
            self.thisptr = new Automata(<unsigned int>args[0])
        elif len(args) == 2:
            self.thisptr = new Automata(<unsigned int>args[0], <unsigned int>args[1])
#        elif len(args) == 3:
#            self.thisptr = new Automata(<unsigned int>args[0], <unsigned int>args[1], <unsigned int>args[2])
#        elif len(args) == 4:
#            self.thisptr = new Automata(<unsigned int>args[0], <unsigned int>args[1], <unsigned int>args[2], <unsigned int>args[3])
        # Set parameters - readonly from outside this C++ object.
        self._overallIndex = 0
        self.updateParams()
        
    def updateParams(self):
        self.p_rule = self.thisptr.p_rule
        self.p_seedIsRandom = self.thisptr.p_seedIsRandom
        self.p_seedStartingPosition = self.thisptr.p_seedStartingPosition
        self.p_generationLength = self.thisptr.p_generationLength
        
    def init_seed(self, *args):
        '''
            init_seed
        '''
        cdef unsigned int n
        cdef unsigned int *p1
        if len(args) == 0:
            self.thisptr.init_seed()
        if len(args) == 1:
            if isinstance(args[0], list) or isinstance(args[0], tuple):
                n = len(args[0])
                p1 = <unsigned int *>malloc(sizeof(unsigned int)*n)
                for i in range(len(args[0])):
                    p1[i] = args[0][i]
                
                self.thisptr.init_seed(p1, n)
                free(p1)
            else:
                self.thisptr.init_seed(<unsigned int>args[0])
        elif len(args) == 2:
            self.thisptr.init_seed(<unsigned int>args[0], <unsigned int>args[1])
        elif len(args) == 3:
            self.thisptr.init_seed(<unsigned int>args[0], <unsigned int>args[1], <unsigned int>args[2])
        self.updateParams()
        
    def __dealloc__(self):
        '''
            c++ destructor.  Delete the automata and any other memory objects
            that may be floating around
        '''
        del self.thisptr
        
    def iterateAutomata(self):
        '''
            Execute a single iteration of the cellular automata.  Creates a new
            current "generation".
        '''
        #self._overallIndex += 1
        self.thisptr.iterateAutomata()
        self._overallIndex = self.thisptr._overallIndex
        
    def printBuffer(self):
        '''    
            Prints the "buffer".  The buffer is basically the history of all
            generations thus far computed, with a maximum of <kAutomataGenerations>
            in Automata.h
        '''
        self.thisptr.printBuffer()
        
    def fillBuffer(self):
        '''
            Executes iterateAutomata() <kAutomataGenerations> times.  If the number
            of generations is > kAutomataGenerations, I believe the buffer pointer
            is reset back into the beginning of the buffer (and execution is skipped).
        '''
        self.thisptr.fillBuffer()
        self._overallIndex = self.thisptr._overallIndex
        
    def currentGenerationIndex(self):
        '''
            Returns the integer of the current generation index.
            Will be 0 < index < kAutomataGenerations
        '''
        return self.thisptr.currentGenerationIndex()
    
    def __repr__(self):
        '''
            Python Display Functin
        '''
            
        self.thisptr.printBuffer()
        return ''
    
    def bStringFromCurrentGeneration(self):
        '''
            Returns the string representation of the binary values of the current
            generation.  Basically each 0 is converted to an ascii '0', and each 
            binary 1 is converted to an ascii '1'.  Could be used, for example, 
            to dump ascii binary data to stdout for processing
        
            a good "last resort", guaranteed way of accessing computed data
        '''
        cdef string str1 = self.thisptr.bStringFromCurrentGeneration()
        #print str[0]
        cdef char *cstr = <char*>malloc(str1.size() + 1)
        strcpy(cstr, str1.c_str())
        s = str(cstr)
        free(cstr)
        # I feel unsafe simply returning a malloc'd cstr, so copy to str
        #    and free it up.  kind of redundant, oh well. Let python handle
        #    the garbage collection and let me handle explicitely "free"ing the
        #    sucker.
        return s
    
    def stringFromCurrentGeneration(self):
        '''
            Returns the string representation of the current generation, with the 
            characters defined in Automata.h
        '''
        cdef string str1 = self.thisptr.stringFromCurrentGeneration()
        cdef char *cstr = <char*>malloc(str1.size() + 1)
        strcpy(cstr, str1.c_str())
        s = str(cstr)
        free(cstr)
        return s
    
    def chunks_FromCurrentGeneration(self,unsigned int bits=8):
        '''
            Returns the unsigned byte representation of the current generation
        '''
        cdef unsigned long *c
        cdef unsigned int n
        c = self.thisptr.chunks_FromCurrentGeneration(n,bits)
        
        # Convert to list
        l=[]
        #a=PyLong_FromUnsignedLong(c[0]))
        for i in range(n):
            l.append(long(c[i]))
        free(c)
        return l































