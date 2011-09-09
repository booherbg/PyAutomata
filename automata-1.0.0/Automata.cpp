/*
 *  Copyright 2008 Alex Reynolds
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  
 *  http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  Automata.cpp
 *  Automata
 *
 *	Notes about this framework.
 *  Automata has an internal "buffer" that fills up, generation by generation,
 *  	as iterateAutomata() is executed.  This means that you set the number of
 *  	generations in the header, and when you "fill" the buffer, you execute
 *  	the iteration until the buffer is full.  This gives you the benefit of:
 *  		1) Pre-allocated size for known number of generations
 *  		2) Execute one time, get the entire CA history
 *
 *  You can get a single generation by creating a g* = new AutomataGeneration() bitset,
 *  	and passing it into generationAtIndex(*g, int) along with an integer.  To get the
 *  	integer of the current index, call getCurrentIndex().  The call to
 *  	currentGeneration(*g) is simply generationAtIndex(*g, getCurrentIndex());
 *
 *  I have taken the liberty of adding specific interfaces for accessing the
 *  	data contained in the "current generation" (ie last generation computed)
 *  	This framework was originally written by the author to compute and display
 *  	arbitrary 2Dimensional CAs; the only necessary output was the 2D structure.  The
 *  	main public function originally  available for this purpose is Automata.stringFromGeneration(&g,int).
 *  	I have added the trivial Automata.stringFromCurrentGeneration().
 *  	Along with:
 *  	unsigned char *bytes_FromGeneration(AutomataGeneration &, int&)
 *  	unsigned char *bytes_FromCurrentGeneration(int&)
 *  		These create a byte array representation of the given automata
 *  		generation.  The generation is broken up into chunks of 8 bits starting
 *  		from the left-most bit.  Each chunk is converted into an unsigned char (1-byte).
 *  		The character array is malloc's inside the functions, so the user must
 *  		take care to free() any char* array that is created.  The integer
 *  		argument is a second output - it will be set as the length of the byte array.
 *
 *  	unsigned char *bStringFromGeneration(AutomataGeneration &)
 *  	bStringFromCurrentGeneration()
 *  		Identical to stringFromGeneration except that the ascii characters
 *  		for '0' and '1' are used instead of the default characters.  Useful
 *  		for slow byt guaranteed binary output from the Automata.  Could be
 *  		used to output to stdout, for example.
 * 		The reason that *FromCurrentGeneration() exists is that it has no prototype
 * 		dependencies on the AutomataGeneration bitset.  This can be easily and
 * 		seamilessly used by the cython compiler for direct access to the buffer,
 * 		returned as a std::string or unsigned char * (easy datatypes to work with
 * 		on the python side).  At this point in time I'm not sure how to directly
 * 		access the AutomataGeneration bitset, however it appears that there is no
 * 		immediate need for direct access.  These supporting functions are doing just fine.
 *
 * 	Possible Bugs:
 *   	Interestingly enough, it appears that the following happens:
 *   	1) _generations = new AutomataGenerations is initialized to <kAutomataGenerations> rows
 *   	2) I think each row in AutomataGenerations is created with default constructor (kAutomataGenerationLength)
 *   	3) Once the true ca width comes through, either the size() is updated to a brand new
 *   		vector (leaving the first vectors hanging in memory) or the print-off
 *   		is restricted to the correct range. Not sure, yet.
 *   	Either way something strange is happening
 *
 *   	Seems that a new vector is created every single time, and is either copied
 *   	or replaces the existing vector. Either way a single destructor is called
 *   	with every constructor, there doesn't seem to be any dangling pointers.
 *   	All is good and sane.
 */

#include "Automata.h"

//#pragma mark -
//#pragma mark Constructors

/* I added this as a class so that we can set how large a generation is at run time
 * This just isn't possible with bitset - it has to be set at compile time.
 *
 * set() and reset() are used for compatibility
 */
AutomataGeneration::AutomataGeneration(unsigned int generationLength)
{
	this->generationLength = generationLength;
	this->_vector = new std::vector<bool>(generationLength);

	//cout << "creating AutomataGeneration with size=" << generationLength << "\n";
	/*for (unsigned int i=0; i < generationLength; i++)
	{
		//(*this->_vector).push_back(0);
		cout << " " << (*this->_vector).size();
	}*/
	reset();

}

/* It appears that this is being called the appropriate number of times */
/*
AutomataGeneration::~AutomataGeneration() {
	cout << "deleting vector" << endl;
}
*/

void AutomataGeneration::reset()
{
	//cout << "AutomataGeneration::reset();size=" << generationLength << "|" << (*this->_vector).size() << "\n";
	for (unsigned int i=0; i < generationLength; i++)
	{
		(*this->_vector)[i] = 0; // shorthand for clearing a bitset
	}
}

void AutomataGeneration::clear(unsigned int index){ (*this->_vector)[index] = 0; }
void AutomataGeneration::flip(unsigned int index){ (*this->_vector)[index].flip(); }
void AutomataGeneration::set(unsigned int index){ (*this->_vector)[index] = 1; }
unsigned int AutomataGeneration::size(){ return (*this->_vector).size(); }

/*
AutomataGeneration& AutomataGeneration::operator = (const AutomataGeneration &ref) {
	*_vector = *(ref._vector);
	generationLength = ref.generationLength;
	return *this;
}*/

Automata::Automata(
		unsigned int rule,
		unsigned int caWidth
/*		unsigned int seedIsRandom,
		unsigned int seedStartingPosition,
		unsigned int generationLength) {
*/	){
//	cout << "Initializing automaton... (rule: " << rule
/*			", generationLength: " << generationLength <<
//			", seedIsRandom: " << seedIsRandom <<
//			", seedStartingPosition: " << seedStartingPosition << ")"
			<< ")" << endl; */
	this->p_rule=rule;
//	this->p_seedIsRandom=seedIsRandom;
//	this->p_seedStartingPosition=seedStartingPosition;
	this->p_generationLength=caWidth;

	try {
		_currentIndex = -1;
		_overallIndex = -1;
		_generations = new AutomataGenerations(kAutomataGenerations); // 150

		//typedef std::bitset< kAutomataGenerationLength >	AutomataGeneration;
		//typedef std::vector< AutomataGeneration >			AutomataGenerations;

		//typedef std::bitset< 100 > ag;
		//void *p = new std::vector< std::bitset< kAutomataGenerationLength > >(10);
		setBinaryRuleFromInteger(rule);
	}
	catch (std::bad_alloc &exc) {
		cout << exc.what() << endl;
	}
	catch (...) {
		delete _generations;
	}

	if (TIME_BASED_SEED)
	{   // this was harder than i realized to get a seed not based on seconds
		timeval t1;
		gettimeofday(&t1, NULL);
		srand(t1.tv_usec * t1.tv_sec);
	} else {
		srand(SRAND_SEED);
	}

	//cout << "end init\n";
}

void Automata::init_seed(unsigned int mode, unsigned int value, unsigned int position)
{
	/*
	 * This function is meant to handle everything relating to the seeding of the
	 * cellular automata.  This was moved out of the constructor.  I needed the option
	 * of specifying that the seed is random, a single cell, or a pre-set value.
	 *
	 * Throws exception 100 if the mode is invalid.
	AutomataGeneration *g_seed = new AutomataGeneration;
	if (seedIsRandom)
		randomizeSeed(*g_seed);
	else
		initializeSeedAtPosition(*g_seed, seedStartingPosition);

	validateGeneration(*g_seed);
	appendGeneration(*g_seed);
	delete g_seed;

	*/
	this->p_seedIsRandom=0;
	this->p_seedStartingPosition=kDefaultSeedStartingPosition;

	// reset back to the head of the buffer!
	this->_currentIndex = -1;
	this->_overallIndex = -1;

	AutomataGeneration *g_seed=NULL;
	//try
	//{
		g_seed = new AutomataGeneration(p_generationLength);
		//(*g_seed).reset(); // or g_seed->reset()
	//} catch (std::bad_alloc &exc)
	//{
	//	cout << exc.what() << endl;
	//}

	//cout << "init_seed()1\n";

	if (mode == 0)
	{	// single centered cell
		initializeSeedAtPosition(*g_seed, p_generationLength/2);

	} else if (mode == 1)
	{   // random seed
		randomizeSeed(*g_seed);
		this->p_seedIsRandom=1;

	} else if (mode == 2)
	{	// pre-specified value, centered
		initializeSeedWithValue(*g_seed, value, position);

	} else if (mode == 3)
	{
		// 010101010101 pattern
		delete g_seed;
		throw(101);

	} else {
		delete g_seed;
		throw(101);
	}


	//cout << "init_seed()2\n";
	validateGeneration(*g_seed);
	//cout << "init_seed()2b\n";
	appendGeneration(*g_seed); // increments _generationIndex
	delete g_seed;

	//cout << "init_seed()3\n";
}

void Automata::init_seed(unsigned int values[], unsigned int n)
{
	/*
	 * This function is meant to handle everything relating to the seeding of the
	 * cellular automata.  This was moved out of the constructor.  I needed the option
	 * of specifying that the seed is random, a single cell, or a pre-set value.
	 *
	 * This function takes a list of 8-bit unsigned integers and places them
	 * into the a generation. The seeding is kind of strange but it works, so I'm
	 * not sure if I'm going to leave it as is or not...
	 *
	 * Basically the value in values[0] is mapped onto the generation from the
	 * left side, but it is "reversed" the bit value. So if the number that is
	 * seeded is 135, in binary that is 10000111. The seeded value is:
	 *     (left) | 11100001 <second value> <third value> |
	 * So you can see that the value is seeded but it is seeded in reverse.
	 * Not a huge deal, but kind of strange.
	 *
	 * Let's say the vector looks like this:
	 *  0         10        20        30        40         50
	 *  |         |         |         |         |          |
	 * |____________________________________________________|
	 *
	 * If I want to make it look like this:
	 *  0         10        20        30        40         50
	 *  |         |         |         |         |          |
	 * |__________11111100000_______________________________|
	 * What convention do I use? Do i say I want 1111100000 at 10? Do I say I
	 * want 0000011111 at 20? Do I say I want 1111100000 at 15?
	 *
	 * I'm choosing that I say I want 1111100000 at 10. So you pass in a value
	 * that has left-most-significant-bit (16-bits) and the left-most index that
	 * you want to include the number (16-bit).
	 *
	 * so init_seed(value=0b1111100000, position=10) should work.

	*/
	unsigned int i;
	this->p_seedIsRandom=0;
	this->p_seedStartingPosition=kDefaultSeedStartingPosition;

	// reset back to the head of the buffer!
	this->_currentIndex = -1;
	this->_overallIndex = -1;

	AutomataGeneration *g_seed=NULL;
	try
	{
		g_seed = new AutomataGeneration(p_generationLength);
		//(*g_seed).reset();
	} catch (std::bad_alloc &exc)
	{
		cout << exc.what() << endl;
	}

	// what does this next line do? I forget. Why is '4' hard coded?
	// looks like position is hard coded. That's weird.
	// each value is an unsigned int. sizeof(unsigned int) == 4 (bytes) = 4*8=16
	// next up: why do we set the position as the center?
//	unsigned int position = (unsigned int)((sizeof(unsigned int)*BITS_PER_SEED_VALUE)/2);
	unsigned int position = 0; // start at left-most side
	cout << "initial position: " << position << endl;
	unsigned int value;
	for (i=0; i<n; i++)
	{
		value = values[i];
		initializeSeedWithValue(*g_seed, value, position);
		position += sizeof(unsigned int)*BITS_PER_SEED_VALUE;
		cout << "next position: " << position << endl;
	}
	//cout << g_seed->count() << "------\n";
	validateGeneration(*g_seed);
	appendGeneration(*g_seed); // increments _generationIndex
	delete g_seed;
}

void Automata::initializeSeedWithValue (AutomataGeneration &seed, unsigned int value, unsigned int position)
{
	/*
	 * initializeSeedWithValue
	 * 	seed is an AutomataGeneration object to be worked with
	 *  value is what value to seed with. unsigned int, so 16-bit by nature
	 *
	 * BITS_PER_SEED_VALUE will determine how many bits we'll use of <value>,
	 * although all entries in <value> will be packed tightly into the vector
	 * See Automata.h to set BITS_PER_SEED_VALUE
	 * 1: Use only the first 4 bits (0-15)
	 * 2: Use only the first 8 bits (0-255)
	 * 4: use only the first 16 bits (0-65535)
	 * 8: use entire range of the unsigned int: (0-4294967295)
	 *
	 * in other words, if BITS_PER_SEED_VALUE is 2:
	 * 		init_seed((255, 255, 255, 255))
	 * is the equivalent of the following if BITS_PER_SEED is 4:
	 * 		init_seed((65535, 65535))
	 */

	unsigned int n = (int)ceil(log2(value)); // number of bits required
	n = sizeof(unsigned int)*BITS_PER_SEED_VALUE; // how many bits per chunk (16-bit)
	bool bit = 0;

//	cout << "position: " << position << endl;

	// n is the width of the chunk to write, typically hard coded to be 16-bits
	if (n > seed.size())
	{
		cout << "Warning: truncating seed because it is too large. ";
		cout << n << "-bits truncated to " << seed.size() << "-bits\n";
		n = seed.size();
	}

	// if we're trying to center too close to the end of the vector, no good
	if (position > (seed.size() - n))
		position = seed.size() - n;

//	cout << "left: " << position << " w:" << w << endl;
//	for (unsigned int i=0; i < n; ++i)
	for (unsigned int i=(n); i > 0; i--) // weird, if we do >= 0 it wraps around into an infinite loop
	{
		// for each bit, set it appropriately
		bit = value % 2;
		if (bit == 1)
		{
			seed.set((i+position-1));
		}
//		cout << (i-1) << ":" << bit << " ";
		value = value / 2;
	}
}


Automata::Automata(const Automata& ref, unsigned int seedFromEndOfRef) {
	_currentIndex = ref._currentIndex;
	_overallIndex = ref._overallIndex;
	for (unsigned int i = 0; i < kNumberOfRules; ++i)
		_rule[i] = ref._rule[i];
	
	if (seedFromEndOfRef)
		_generations->at(0) = ref._generations->at(kAutomataGenerations-1);
	else
		for (unsigned int i = 0; i < kAutomataGenerations; ++i)
			_generations->push_back(ref._generations->at(i));
}

Automata::~Automata()
{
    //cout << "Deleting automaton..." << endl;
    delete _generations;
}

/*
 * C++ Operators
 * 	Definitions for operators like << and =
 */

ostream& operator << (ostream &os, const Automata &ref) {
	AutomataGeneration *test = new AutomataGeneration(ref.p_generationLength);
	for (unsigned int i = 0; i < kAutomataGenerations; ++i) {
		ref.generationAtIndex(*test, i);
		os << i << " " << ref.stringFromGeneration(*test) << endl;
	}
	delete test;
	return os;
}

Automata& Automata::operator = (const Automata &ref) {
	_currentIndex = ref._currentIndex;
	_overallIndex = ref._overallIndex;
	_rule = ref._rule;
	*_generations = *(ref._generations);
	return *this;
}


/*
 * Public Functions
 *   All publicly accessible functions
 */
void Automata::iterateAutomata () {
	/* iterateAutomata
	 * 		This is the core function responsible for applying the rule to each
	 * 		generation in the automata.  A single execution of this function:
	 * 			0) If there is no seed, call init_seed() with default parameters
	 * 			1) Create 2 bit generations, <current> and <next>
	 * 			2) current = generationAtIndex(_currentIndex)
	 * 			3) next = getNextGeneration(current)
	 * 			4) Make sure it is a valid generation
	 * 			5) Append the generation to the buffer and increment index
	 * 			6) Delete both <current> and <next>.  This might be optimizable
	 */
	try {
		// Q: Why are these created each time?
		// A:  Because they are copied into the _generations vector and
		//		subsequently deleted.
		AutomataGeneration *current = new AutomataGeneration(p_generationLength);
		AutomataGeneration *next = new AutomataGeneration(p_generationLength);

		if (_currentIndex == -1)
		{
			init_seed(0);
		}

		generationAtIndex(*current, _currentIndex);
		getNextGeneration(*current, *next);
		validateGeneration(*next);
		appendGeneration(*next);

		delete next;
		delete current;
	}
	catch (std::bad_alloc &exc) {
		cout << exc.what() << endl;
	}
}

void Automata::printBuffer () const {
	/*
	 * printBuffer
	 * 		prints out the automata 2D Buffer.  It does this by creating a bit
	 * 		vector (AutomataGenerations) and setting it as each consecutive
	 * 		generation present in the buffer.  Then that generation is printed
	 * 		out as a single line (via stringFromGeneration()).
	 */
	unsigned int const max_digits = 7;
	if (_currentIndex == -1)
	{
		cout << "*** Warning: Cellular Automata not seeded.  Please use init_seeds()\n";
		return;
	}
	unsigned int digits=0;
	AutomataGeneration *test = new AutomataGeneration(p_generationLength);
//	cout << "test size to start out with is: " << (*test).size() << endl;
	for (unsigned int i = 0; i < kAutomataGenerations; ++i) {
		generationAtIndex(*test, i);
		if (i == 0)
		{
			digits = 0;
		} else {
			digits = log10(i); // see how many positions we need
		}
		if (digits > max_digits)
		{
			digits = max_digits;
		}
		if (digits < 0)
		{
			digits = 0;
		}

		for (unsigned int j=0; j<(max_digits-digits); j++)
		{
			cout << " ";
		}
//		cout << "length: >>" << (*test).size() << " " << p_generationLength << " ";
		cout << i << "|" << stringFromGeneration(*test) << "|" << endl;
	}
	delete test;		
}

void Automata::fillBuffer ()
{
	/* fillBuffer
	 * 	This function calls iterateAutomata() kAutomataGenerations-1 times.
	 * 	It appears this function is meant to really only be called one time.
	 * 	After it is called once, each consecutive time would continue to generate
	 * 	the buffer... but not overwrite it.
	 */
	for (unsigned int counter = 0; counter < kAutomataGenerations-1; ++counter)
		iterateAutomata();
}

int Automata::currentGenerationIndex () const
{
	return _currentIndex;
}

std::string Automata::stringFromGeneration (AutomataGeneration &g) const
{
    std::string str(g.size(), kOffPixelChar);
    for (unsigned int i = 0; i < g.size(); ++i)
    {
        str[i] = (g[i] == kOffPixel ? kOffPixelChar : kOnPixelChar);
    }
    return str;
}

std::string Automata::stringFromCurrentGeneration () const
{
	std::string s;
    AutomataGeneration *g = new AutomataGeneration(p_generationLength);
	if (_currentIndex == -1)
	{
		cout << "*** Warning: Cellular Automata not seeded.  Please use init_seeds()\n";
		return s;
	}
//		init_seed();

    currentGeneration(*g);
    s = stringFromGeneration(*g);
    delete g;
    return s;
}

std::string Automata::bStringFromGeneration (AutomataGeneration &g) const
{
//	const char on = '1';
//	const char off = '0';
    std::string str(g.size(), kOffPixelChar);
    for (unsigned int i = 0; i < g.size(); ++i)
    {
        str[i] = (g[i] == kOffPixel ? '0' : '1');
    }
    return str;
}

std::string Automata::bStringFromCurrentGeneration() const
{
	std::string s;
	AutomataGeneration *g = new AutomataGeneration(p_generationLength);
	if (_currentIndex == -1)
	{
		cout << "*** Warning: Cellular Automata not seeded.  Please use init_seeds()\n";
		return s;
	}
	currentGeneration(*g);
	s = bStringFromGeneration(*g);
	delete g;
	return s;
}

// *** Could return void *, cast to appropriate type based on size of <n> ***
unsigned long *Automata::chunks_FromGeneration(AutomataGeneration &g, unsigned int &numchunks_out, unsigned int n) const
/*
 * unsigned int *bytes_FromGeneration(AutomataGeneration &g, unsigned int &chunks)
 *
 * Returns a byte array (on the heap) (unsigned character array). chunks is set
 * 	to the length of the returned byte array.  Don't forget to free the princess.
 * 	// for example.
 * 	unsigned int n=0;
 * 	AutomataGeneration *g = new AutomataGeneration()
 * 	currentGeneration(g)
 * 	unsigned char *princess = bytes_FromGeneration(*g, n);
 * 	do_something_like_have_tea_with(princess, n);
 * 	free(princess); // important, otherwise she will run rampant through the
 * 					// memory swamp like a banshee in the night. You don't want that.
 *
 * @TODO: I think there is a bug here, must check chunks for accurate shit
 */
{
	//unsigned char c;
//	unsigned int n=6; // Bits per Byte
	unsigned int k=0,num=0,i=0,j=0;

	 /**********'-0__0-'***************************************-0__o-**********\
	 |                                                                         |
	 |  		MALLOC WARNING - don't forget to free the princess!!!!		   |
	 |                                                                         |
	 \******************************+~.._/''\_..~+*****************************/
	numchunks_out = g.size()/n; // Number of Bytes per Generation
	if (n <= 0)
	{
		cout << "** ERROR **: n <= 0 invalid\n";
		return NULL;
	}
	/*
	if (n <= 8)
	{
		unsigned char *princess = (unsigned char *)malloc(chunks_out*sizeof(unsigned char)); // dependent on arbitrary n=8
	} else if (8 < n <= 16)
	{
		unsigned int *princess = (unsigned int *)malloc(chunks_out*sizeof(unsigned int));
	} else if (16 < n <= 32)
	{
		unsigned long *princess = (unsigned long *)malloc(chunks_out*sizeof(unsigned long));
	}

	if (princess == NULL)
	{
		cout << "** ERROR **: Unable to allocate memory for byte array.  The princess has been lost.\n";
		exit(EXIT_FAILURE);
	}
	*/
	unsigned long *princess = (unsigned long *)malloc(numchunks_out*sizeof(unsigned long));

	for (i=0; i<(g.size());)
	{
		// for some stupid reason, cout << g[i] throws all kinds of garbage out
		//    it's because i am using pointer math, gar. (*g)[0] not g[0]
		num=0;
		for (j=0; j<n; j++)
		{
			if (i >= g.size()) {
				break;
			}
//			cout << i << " < " << g.size() << " < " << numchunks_out << endl;;
			num += g[i]*pow(2, (n-1)-j);
			//cout << g[i];
			i++;
		}

		//cout << " " << num << " ";
		// *** Protect the Princess! ***
		if (k >= numchunks_out)
		{
			//cout << "The princess cannot eat cake; too full. k>=numchunks_out\n";
		} else
		{
			princess[k] = num;
		}
		k++;
	}
	return princess;

}

unsigned long *Automata::chunks_FromCurrentGeneration(unsigned int &numchunks_out, unsigned int n) const
/*
 * bytes_FromCurrentGeneration
 * 	Creates a byte array representation (binary) of the current generation state.
 * 	Returns a pointer to the byte array of length <n>
 * 	numchunks_out is set (passed by reference) to the length of the returned character array
 *	n is the number of bits to break the automata into.
 *
 *  The current generation is broken up into groups of 8 bits.  Each bit is converted
 *  into an unsigned number and added to the array.  Pretty standard.
 *
 *  Next up - mapping of arbitrary lengths.
 *
 *  *g means "the object pointed to by the pointer g"
 */
{
	AutomataGeneration *g = new AutomataGeneration(p_generationLength);
	if (_currentIndex == -1)
	{
		cout << "*** Warning: Cellular Automata not seeded.  Please use init_seeds()\n";
		return 0;
	}
	currentGeneration(*g);
	//unsigned char *c = bytes_FromGeneration(*g, chunks_out, n);
	unsigned long *c = chunks_FromGeneration(*g, numchunks_out, n);
	return c;
}

// note: this creates *very large* integers.  Duh! Use string instead
//unsigned int Automata::uintFromCurrentGeneration()
/*
 * uintFromCurrentGeneration
 * 		a helper class for cython.  Gets us an unsigned integer representation
 * 		of the current generation without having to export bitset, AutomataGeneration,
 * 		AutomataGenerations, string, etc.
 */
/*
{
	unsigned int i;
	unsigned long long int ret=0;
	AutomataGeneration *g = new AutomataGeneration();
	currentGeneration(*g);
	for (i=0; i<(g->size()); i++)
	{
		ret += ((*g)[i] == kOffPixel ? 0 : 1);
		ret = ret << 1;
		cout << (*g)[i] << "";
	}
	cout << " " << ret << "\n";
	return ret;
}
*/

/*
 * Private / Utility Functions
 */
int	Automata::currentIndex () const
{
	return _currentIndex;
}

int	Automata::overallIndex () const
{
	return _overallIndex;
}

void Automata::getGeneration (AutomataGeneration &output, unsigned int index) const
{
	//output is a bitmask vector.
	//cout << "index: " << index << "\n";
	// this has to be a copy operation.  we're copying the data from the
	// AutomataGeneration in the <vector> _generations into the fresh
	// AutomataGeneration &output.  A pointer to _generations is not desired
	// to protect data integrity (read-only).
	// @TODO if output already points at a vector, does it create a dangling pointer?
	output = _generations->at(index);
}

void Automata::currentGeneration (AutomataGeneration &output) const
{
	// Get the generation we are at based on the index
	if (_currentIndex == -1)
	{
		cout << "*** Warning: Cellular Automata not seeded.  Please use init_seeds()\n";
		return;
	}
	getGeneration(output, _currentIndex);
}
void Automata::generationAtIndex (AutomataGeneration &output, unsigned int index) const
{
	// Get the generation of an arbitrary index.
	getGeneration(output, index);
}

void Automata::appendGeneration (AutomataGeneration &g)
{
	//increment indices and append generation to the buffer
    _overallIndex++;
    if (_currentIndex < 0) {
    	_currentIndex = -1; // protection
    }
    _currentIndex = (_currentIndex + 1) % kAutomataGenerations; //wrap-around
    _generations->at(_currentIndex) = g; // copy? has to be, g gets deleted()
    //_currentGeneration = g;
}
void Automata::randomizeSeed (AutomataGeneration &seed)
{
	// generate a random generation bitfield
	for (unsigned int i = 0; i < p_generationLength; ++i)
		if (random() % 2 == 0)
		{
			seed.clear(i);
		} else
		{
			seed.set(i);
		}
}
void Automata::initializeSeedAtPosition (AutomataGeneration &seed, unsigned int position)
{
	// seed is a generation bitfield.
	//cout << "initializeSeedAtPosition1\n";
    seed.reset(); // reset bitfield to all binary 0's
    seed.set(position); // Turn the <position> to binary 1
	//cout << "initializeSeedAtPosition2\n";
}

/*
void Automata::initializeCurrentGeneration(unsigned int value)
{
    AutomataGeneration *g = new AutomataGeneration();
    currentGeneration(*g);
	initializeSeedWithValue(*g, value);
    delete g;
}*/



/*
 * Private Members & Functions
 *
 */
 void Automata::setBinaryRuleFromInteger (unsigned int numericalCode)
 /*
  * setBinaryRuleFromInteger
  * 	Takes the given rule, like 110, and computes the computation (0 or 1) that
  * 	each neighborhood permutation would produce.  Initializes _rule[]
  */
 {
 	int x, y;
 	for (y = kNumberOfRules-1; y >= 0; y--) {
 		x = numericalCode / (1 << y);
 		numericalCode = numericalCode - x * (1 << y);
 		_rule[y] = (x == 0 ? kOffPixel : kOnPixel);
 	}
 }

 void Automata::getNextGeneration (AutomataGeneration &cg, AutomataGeneration &ng)
 /*
  * getNextGeneration
  * 	For the given generation, execute the output of each neighborhood and
  * 	assign that value to the corresponding cell in the next generation &ng.
  */
 {
 	unsigned int output, neighborhood;
 	for (unsigned int position = 0; position < cg.size(); ++position) {
 		neighborhood = getNeighborhoodForGenerationAtPosition(cg, position);
 		validateNeighborhood(neighborhood);
 		output = getOutputForNeighborhood(neighborhood);
 		validateOutput(output);
 		//ng[position] = (output == 0 ? kOffPixel : kOnPixel);
 		if (output == 0)
 			ng.clear(position);
 		else
 			ng.set(position);
 	}
 }

 unsigned int Automata::getOutputForNeighborhood (unsigned int neighborhood)
 /*
  * 	This function basically looks up the next state of the cell given
  * 	the unsigned integer representation (3-bit) of the neighborhood.
  * 	Returns a lookup into the array _rule[unsigned int[8]]
  */
 {
 	validateNeighborhood(neighborhood);
 	unsigned int output = _rule[neighborhood];
 	validateOutput(output);
 	return output;
 }

 unsigned int Automata::getNeighborhoodForGenerationAtPosition (AutomataGeneration &cg, unsigned int position)
 /*
  * getNeighborhoodForGenerationAtPosition
  * 	Creates the neighborhood of generation cg at <position>.
  * 	Also takes care of wrapping around the neighborhood to include neighbors
  * 	on either side of the vector (wrap-around).
  *
  * 	This would be the function to modify if wrap-around is not desired. For
  * 	example, each cell to the left of g[0] and to the right of g[-1] can be
  * 	assumed as static value of 0 or 1.
  *
  */
 {
 	unsigned int center = (cg[position] == kOffPixel ? kOffPixelInteger : kOnPixelInteger);
 	unsigned int left;
 	if (position == 0) {
 		left = (cg[cg.size()-1] == kOffPixel ? kOffPixelInteger : kOnPixelInteger);
 	} else {
 		left = (cg[position-1] == kOffPixel ? kOffPixelInteger : kOnPixelInteger);
 	}
 	unsigned int right;
 	if (position == cg.size()-1) {
 		right = (cg[0] == kOffPixel ? kOffPixelInteger : kOnPixelInteger);
 	} else {
 		right = (cg[position+1] == kOffPixel ? kOffPixelInteger : kOnPixelInteger);
 	}
 	unsigned int neighborhood = (1 * right) + (2 * center) + (4 * left);
 	validateNeighborhood(neighborhood);
 	return neighborhood;
 }

/* Exceptions */
void Automata::validateNeighborhood (unsigned int neighborhood)
{
	if ((neighborhood < kLowBoundOnNeighborhood) || (neighborhood > kHighBoundOnNeighborhood))
		throw kAutomataBadNeighborhoodBoundsException;
}
void Automata::validateOutput (unsigned int output)
{
	if ((output < kLowBoundOnOutput) || (output > kHighBoundOnOutput))
		throw kAutomataBadOutputBoundsException;
}
void Automata::validateGeneration (AutomataGeneration &g)
{
	//cout << g.size() << " " << p_generationLength << "\n";
	if (g.size() > p_generationLength)
		throw kAutomataBadStringLengthException;
}
