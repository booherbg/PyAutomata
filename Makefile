# cython makefile - Blaine Booher December 2010
# 
# Template file for using cython to create a python extention module.  As an example
# I will use the Automata C++ object.
#
# Automata.cpp and Automata.h are the original source files.  These define a pure 
# C++ object meant to be used with C++.
# 
# In pyAutomata.pyx I have created a cython definition for the interfacing into 
# the C++ object found in Automata.cpp.  This special cython syntax basically 
# describes how cython will treat the C++ interfaces in order to create a separate
# interface using the Python/C++ Extention API.  The extension that is created 
# will be pyAutomata.so, and contains both the C++ object from Automata.cpp and 
# the python extension interface known as pyAutomata.  
#
# This makefile does the following for NAME=pyAutomata and SOURCE=Automata
# 1) It makes the ${BUILD} directory, by default build/temp.linux-x86_64
# 2) Translate ${NAME}.pyx into ${NAME}.cpp using cython --cplus
# 3) Compile ${NAME}.cpp into a .o file
# 4) Compile ${SOURCE}.cpp into a .o file
# 5) Link the .o files together into a shared library that can be imported into
#		the python environment, ${NAME}.so
# Note: no optimizations take like 20s for 100x200,000 generations.
# -O1 takes 2sec, -O2 takes 1.8sec

# ${NAME}.pyx will be turned into ${NAME}.cpp and finally ${NAME}.o and the python library ${NAME}.so
NAME = pyAutomata

# ${AUTOMATA} is the folder where ${SOURCE}.cpp and ${SOURCE}.h exist
AUTOMATA = automata-1.0.0/

# ${SOURCE}.cpp and ${SOURCE}.h need to exist.
SOURCE = ${AUTOMATA}/Automata

# Edit these if you have anything other than the setup specified above
FILES = ${NAME}.pyx ${SOURCE}.cpp ${SOURCE}.h
all: 	${NAME}.so

# Don't have 
BUILD = build/temp.linux-x86_64-2.6/

${NAME}.so:	${FILES} 
	mkdir -p ${BUILD}
#	python setup.py build_ext --inplace
	cython --cplus ${NAME}.pyx
	# The following was generated based on setup.py; $python setup.py build_ext --inplace
	gcc -O2 -pthread -fno-strict-aliasing -fwrapv -Wall -fPIC -I. -I${AUTOMATA} -I/usr/include/python2.6 -c ${NAME}.cpp -o ${BUILD}/${NAME}.o
	gcc -O2 -pthread -fno-strict-aliasing -fwrapv -Wall -fPIC -I. -I${AUTOMATA} -I/usr/include/python2.6 -c ${SOURCE}.cpp -o ${BUILD}/${SOURCE}.o
	g++ -lm -pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions ${BUILD}/${NAME}.o ${BUILD}/${SOURCE}.o -lstdc++ -o ${NAME}.so

clean:	
	rm ${NAME}.so ${NAME}.cpp
