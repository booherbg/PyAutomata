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
 *  Automata.h
 *  Automata
 *  
 *  November 07 2010 - modified by Blaine Booher
 *      removed #pragma constructs
 *      changed for loops to use unsigned ints where appropriate
 *  Nov. 16 2010
 *  	- Took out all functions from .h file.  Semantics.  It bothered me a little
 *  		bit too much that i couldn't glance at .h file for definition headers.
 *  	- Removed bool types in constructors; cython compatibility
 *  		see http://www.mail-archive.com/cython-dev@codespeak.net/msg09176.html
 *  	- Added printAutomaton as a cython accessor so I don't have to figure out
 *  		how to access the operator<<!
 *  Nov. 18 2010
 *  	- added _p* internal parameters for rule&seed info..  Bound to cython class read-only.
 *  Jan 11 2011
 *  	- Revamped the seeding mechanism, in particular added init_seed() with 3 modes,
 *  		plus another mode that allows for an arbitrary list to be used a the seed
 *  	- changed AutomataGeneration from bitset to vector<bool> to allow for run-time
 *  		allocation.  bitset can only be set at compile time.
 */

#pragma once

#ifndef AUTOMATA_H
#define AUTOMATA_H

#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include <bitset>
#include <cstring>
#include <math.h>
#include <sys/time.h>

using namespace std;

// this shouldn't be a problem, right?
// used with init_seed, now that i see it i think we should use short.
#define BITS_PER_BYTE 4

static const unsigned int kNumberOfRules =				8;
static const unsigned int kAutomataGenerationLength =	64;//160 //divisible by 8 please (for byte chunking)
																// or divisible by <organism_geneLength>
static const unsigned int kAutomataGenerations =		50;//240

// Defaults for Class Constructor
static const unsigned int kDefaultRule =				30;
static const unsigned int kDefaultSeedStartingPosition =kAutomataGenerationLength/2 - 1; // center
static const unsigned int kDefaultSeedInitializerIsRandom =		0;
static const unsigned int kDefaultShouldSeedFromEndOfRef =		0;
static const unsigned int kDefaultSeedMode = 			0;
static const unsigned int kDefaultSeed = 				1360;

// Universe Parameters
static const unsigned int kLowBoundOnNeighborhood =		0;
static const unsigned int kHighBoundOnNeighborhood =	7;
static const unsigned int kLowBoundOnOutput =			0;
static const unsigned int kHighBoundOnOutput =			1;
static const bool kOffPixel =							0;
static const bool kOnPixel =							1;
static const char kOffPixelChar =						' ';
static const char kOnPixelChar =						'#';
static const unsigned int kOffPixelInteger =			0;
static const unsigned int kOnPixelInteger =				1; 
static const std::string kAutomataBadNeighborhoodBoundsException =	"Bad neighborhood bounds!";
static const std::string kAutomataBadOutputBoundsException =		"Bad output bounds!";
static const std::string kAutomataBadStringLengthException =		"Automata generation is too long!";

static const bool TIME_BASED_SEED = 					1;
static const unsigned long long SRAND_SEED = 			1123581321;

//typedef std::bitset< kNumberOfRules >				AutomataRule;
//typedef std::bitset< kAutomataGenerationLength >	AutomataGeneration;
//typedef std::vector< AutomataGeneration >			AutomataGenerations;

class AutomataGeneration {
public:
	unsigned int generationLength;
	std::vector< bool > *_vector;
	AutomataGeneration(unsigned int generationLength=kAutomataGenerationLength);
	void clear(unsigned int);
	void flip(unsigned int index);
	void reset();
	void set(unsigned int);
	unsigned int size();
	int operator[](int i) { return (*this->_vector)[i]; }
	//AutomataGeneration& operator = (const AutomataGeneration &ref) {
	// how to overload g[x]=i;?
	//int &operator[](int i);
};

typedef std::bitset< kNumberOfRules >				AutomataRule;
//typedef std::bitset< kAutomataGenerationLength >	AutomataGeneration;
typedef std::vector< AutomataGeneration >			AutomataGenerations;

class Automata {		
public:

	// constructors
	Automata(unsigned int rule = kDefaultRule,
			 unsigned int caWidth = kAutomataGenerationLength
//			 unsigned int seedIsRandom = kDefaultSeedInitializerIsRandom,
//			 unsigned int seedStartingPosition = kDefaultSeedStartingPosition,
//			 unsigned int generationLength = kAutomataGenerationLength);
			);
	Automata(const Automata &ref, unsigned int seedFromEndOfRef = kDefaultShouldSeedFromEndOfRef); //?
	void init_seed(unsigned int mode=kDefaultSeedMode, unsigned int value=kDefaultSeed, unsigned int position=kDefaultSeedStartingPosition);
	void init_seed(unsigned int values[], unsigned int n);
	// destructor
	~Automata();
	
	typedef std::bitset< 10 > aaa;

	// public variables
	unsigned int p_rule;
	unsigned int p_seedIsRandom;
	unsigned int p_seedStartingPosition;
	unsigned int p_generationLength;

	// operators
	friend ostream& operator<< (ostream &os, const Automata &ref);
	Automata& operator= (const Automata &ref);
	
	// public functions
	// const in a function means that it is illegal for the function to modify a class variable
	/* const in a function declaration is interesting.
	 *   imagine function currentGeneration(AutomataGeneration &output).  Is
	 *   	&output being used because it is being modified by the function, or
	 *   	is it being used because it is expensive to copy the datatype (but
	 *   	no modification is necessary)? No way to know without looking at source.
	 *   	The const in a parameter list lets the user know that the variable is
	 *   	not being modified (but still being passed as reference to save resources).
	 *   http://duramecho.com/ComputerInformation/WhyHowCppConst.html
	 */
	void iterateAutomata ();
	void printBuffer () const;
	void fillBuffer ();
	int currentGenerationIndex () const;
	void currentGeneration (AutomataGeneration &output) const;
	void generationAtIndex (AutomataGeneration &output, unsigned int index) const;
	std::string stringFromGeneration (AutomataGeneration &g) const;
	std::string stringFromCurrentGeneration() const;

	// output as 0/1 characters
	std::string bStringFromGeneration (AutomataGeneration &g) const;
	std::string bStringFromCurrentGeneration() const;

	// output as byte arrays
	// increased from <char> to <int> so we can go up to 16-bit
	unsigned long *chunks_FromGeneration(AutomataGeneration &g, unsigned int &, unsigned int) const;
	unsigned long *chunks_FromCurrentGeneration(unsigned int &, unsigned int) const;

	// used to be private but it is useful to know how far into a computation we are.
	 int _overallIndex;
	 //void initializeCurrentGeneration(unsigned int);

private:
	// local vars
	int _currentIndex;
	AutomataRule _rule;
	AutomataGenerations* _generations;
	
	// private functions
	void setBinaryRuleFromInteger (unsigned int numericalCode);
	void getNextGeneration (AutomataGeneration &cg, AutomataGeneration &ng);
	unsigned int getOutputForNeighborhood (const unsigned int neighborhood);
	unsigned int getNeighborhoodForGenerationAtPosition (AutomataGeneration &cg, unsigned int position);
	
	// utility functions
	int	currentIndex () const;
	int	overallIndex () const;
	void getGeneration (AutomataGeneration &output, unsigned int index) const;
	void appendGeneration (AutomataGeneration &g);
	void randomizeSeed (AutomataGeneration &seed);
	void initializeSeedAtPosition (AutomataGeneration &seed, unsigned int position);
	void initializeSeedWithValue (AutomataGeneration &, unsigned int, unsigned int position=kDefaultSeedStartingPosition);
	//void initializeSeedWithValue (AutomataGeneration &, unsigned int[], unsigned int);

	// Exceptions
	void validateNeighborhood (unsigned int neighborhood);
	void validateOutput (unsigned int output);
	void validateGeneration (AutomataGeneration &g);
};

#endif
