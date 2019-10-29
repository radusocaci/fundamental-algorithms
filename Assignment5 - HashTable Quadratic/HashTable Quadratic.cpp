/**
 * @author Socaci Radu Andrei
 * @group 30421
 *
 * Implement a hashTable using quadratic probing and evaluate the search operation
 *
 * The search operation is O(1) in all cases, thus the data structure is very efficient
 */

#include <iostream>
#include <fstream>
#include "Profiler.h"
#include "HashTable.h"

std :: ofstream fout("result.txt");

#define HASHT_SIZE 10007
#define ELEMENTS_SEARCH 1500

int hashingFunction(int id, int arraySize) {
	return id % arraySize;
}

void insertElements(HashTableT *hashTable, double fillFactor, char *name) {
	int random[2];

	do {
		FillRandomArray(random, 2, 0, 50000, true, 0);
		insertHashTable(hashTable, random[0], name);
	} while ((double)hashTable->nrElements / hashTable->arraySize < fillFactor);
}

void performSearch(HashTableT *hashTable, char *name, int *tef, int *mef, int *tenf, int *menf) {
	int searchIndex[ELEMENTS_SEARCH + 1], operations;

	FillRandomArray(searchIndex, ELEMENTS_SEARCH, 0, hashTable->arraySize - 1, true, 0);

	for (int i = 0; i < ELEMENTS_SEARCH; i++) {
		while (hashTable->storageArray[searchIndex[i]].data == NULL) {
			if (searchIndex[i] == hashTable->arraySize) {
				searchIndex[i] = 0;
			}
			else {
				searchIndex[i]++;
			}
		}

		findHashTable(hashTable, hashTable->storageArray[searchIndex[i]].data->id, name, &operations);
		*tef += operations;
		if (operations > *mef) {
			*mef = operations;
		}
	}

	FillRandomArray(searchIndex, ELEMENTS_SEARCH, 50001, 100000, true, 0); // since we insert [0, 50000]
	
	for (int i = 0; i < ELEMENTS_SEARCH; i++) {
		findHashTable(hashTable, searchIndex[i], name, &operations);
		*tenf += operations;
		if (operations > *menf) {
			*menf = operations;
		}
	}
}

void printResult(int aef, int mef, int aenf, int menf, double ff) {
	if (fout.is_open()) {
		fout << ff << "\t\t\t" << aef << "\t\t\t" << mef << "\t\t\t" << aenf << "\t\t\t" << menf << "\n";
	}
}

void averageCase() {
	HashTableT *hashTable;
	char name[30] = "Test Name";
	const double fillFactor[] = { 0.8, 0.85, 0.9, 0.95, 0.99 };
	int sizeFill = sizeof(fillFactor) / sizeof(double), samples, totalEffortFound = 0, maxEffortFound = 0,
		totalEffortNotFound = 0, maxEffortNotFound = 0;
	
	if (fout.is_open()) {
		fout << "Filling factor \t Avg. Effort found \t Max. Effort found \t Avg. Effort not found \t Max. Effort not found\n";
	}

	for (int i = 0; i < sizeFill; i++) {
		for (samples = 0; samples < 5; samples++) {
			hashTable = createHashTable(hashingFunction, HASHT_SIZE);

			insertElements(hashTable, fillFactor[i], name);
			performSearch(hashTable, name, &totalEffortFound, &maxEffortFound, &totalEffortNotFound, &maxEffortNotFound);

			purgeHashTable(hashTable);
		}

		int averageEffortFound = totalEffortFound / (ELEMENTS_SEARCH * 5),
			averageEffortNotFound = totalEffortNotFound / (ELEMENTS_SEARCH * 5);
		printResult(averageEffortFound, maxEffortFound, averageEffortNotFound, maxEffortNotFound, fillFactor[i]);

		totalEffortFound = maxEffortFound = totalEffortNotFound = maxEffortNotFound = 0;
	}
}

int main()
{
	/*HashTableT *hashTable = createHashTable(hashingFunction, 11);
	char test[5][30] = { "Radu", "Maria", "Andreea", "George", "Ioana" };
	int operations;

	insertHashTable(hashTable, 1, test[0]);
	insertHashTable(hashTable, 11, test[1]);
	insertHashTable(hashTable, 21, test[2]);
	insertHashTable(hashTable, 31, test[3]);
	insertHashTable(hashTable, 41, test[4]);

	printHashTable(hashTable);

	std::cout << "\nSearch for entry Maria with id 11: ";
	if (findHashTable(hashTable, 11, test[1], &operations) != -1) { 
		std::cout << hashTable->storageArray[findHashTable(hashTable, 11, test[1], &operations)].data->name 
		<< " at position " << findHashTable(hashTable, 11, test[1], &operations) << "\n";
	}

	std::cout << "\nDeleting entry Maria\n";
	deleteHashTable(hashTable, 11, test[1]);

	std::cout << "\nSearch for entry Maria with id 11: ";
	if (findHashTable(hashTable, 11, test[1], &operations) != -1) {
		std::cout << hashTable->storageArray[findHashTable(hashTable, 11, test[1], &operations)].data->name
		<< " at position " << findHashTable(hashTable, 11, test[1], &operations) << "\n";
	}
	else
		std::cout << "Entry was not found!\n";

	printHashTable(hashTable);

	purgeHashTable(hashTable);
*/
	averageCase();
}

