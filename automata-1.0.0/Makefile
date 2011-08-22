GCC=g++
SHARED=-fPIC -O3 -Wall -shared
BUILD=-O3 -Wall

all: 			automata.so automata

automata.so:	Automata.h Automata.cpp
	${GCC} ${SHARED} Automata.cpp -o automata.so

automata:	automata.so main.cpp
	#${GCC} ${BUILD} Automata.cpp main.cpp -o automata
	${GCC} ${BUILD} ./automata.so main.cpp -o automata
	#${GCC} ${BUILD} -lautomata main.cpp -o automata
	
clean: 
	rm automata.so automata
