#include "Automata.h"
#include <iostream>
/*
Automata::Automata(unsigned int rule,
		bool seedIsRandom,
		unsigned int seedStartingPosition,
		unsigned int generationLength) {
*/

int main()
{
	Automata automata = Automata(110);
	cout << "p_rule: " << automata.p_rule << "\n";
        /*
	int i;
	for (i=0;i<10;i++)
	{
		//automata.uintFromCurrentGeneration();
                // uncomment the following if you don't want to use
                // fillBuffer()
		automata.iterateAutomata();
	}
        */

	//unsigned int *c = (unsigned int *)malloc(sizeof(unsigned int)*chunks);
	//cout << automata;
        //automata.printBuffer();
        automata.fillBuffer(); // just to be safe, fill up the buffer
        automata.printBuffer();

}
