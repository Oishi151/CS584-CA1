//	Olivia Clark, Oishi Banerjee | CS584 CA1 - Fore & Aft | Due Thurs 1/30/25 10pm
//	
// 
//	Performance summary:
// 
//			Search Method:	Best First Search
//			Performance:

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

bool loop = true;
string fileName;
string searchName;
int puzzleSize;
int stepNum;
int maxOpenSize;
int nodeNum;

bestFirstSearch() {
	

	// add code here

	cout << "Heuristic search has found a solution with " << stepNum << " steps.\nMaximum open size is " << maxOpenSize << "\nSee " << searchName << puzzleSize << ".out for details\n\nTotal of " << nodeNum << " nodes generated.\n\nSee Best" << puzzleSize << ".out for details\n\n";
}

int main() {
	while (loop) {
		cout << "Enter puzzle size for A* (5 - 13) or 1 to exit: ";
		cin >> puzzleSize;
		if (puzzleSize == 1) {
			loop = false;
			cout << "Exiting\n";
			break;
		}
		bestFirstSearch();
		cout << "Total of " << nodeNum << " nodes generated.\n\n";
	}
	system("pause");	// Waits for user to press a key before exiting
}