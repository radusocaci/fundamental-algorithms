#include "HashTable.h"
#include <string.h>
#include <iostream>

static void fatal_error(const char *msg)
{
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

static bool compareEntry(EntryT *a, int id, char *name) {
	if (!a)
		return false;

	if (a->id == id && !strcmp(a->name, name)) {
		return true;
	}

	return false;
}

EntryT *newEntry(int id, char *name) {
	if (id > 0 && name != NULL && strlen(name) > 0) {
		EntryT *entry = (EntryT*)malloc(sizeof(EntryT));
		if (entry) {
			entry->id = id;
			strcpy(entry->name, name);

			return entry;
		}
	}

	return NULL;
}

//EntryT *retrieveData(HashTableT *hashT, int index) {
//	return newEntry(hashT->storageArray[index].data->id, hashT->storageArray[index].data->name);
//}

HashTableT *createHashTable(int (*f)(int, int), int arraySize)
{
	if (!arraySize)
		fatal_error("The table cannot have 0 buckets!");

	HashTableT *newHashT = (HashTableT*)malloc(sizeof(HashTableT));
	if (newHashT)
	{
		newHashT->arraySize = arraySize;
		newHashT->nrElements = 0;
		newHashT->hashFunction = f;
		newHashT->storageArray = (HashTEntry*)malloc(sizeof(HashTEntry) * (arraySize + 1));
		if (newHashT->storageArray)
		{
			for (int i = 0; i < arraySize; i++)
			{
				newHashT->storageArray[i].data = NULL;
				newHashT->storageArray[i].flag = EMPTY;
			}
		}
		else
		{
			free(newHashT);
			fatal_error("Out of memory!");
		}
	}
	else
		fatal_error("Could not allocate hash table!");

	return newHashT;
}

void insertHashTable(HashTableT *hashT, int id, char *name)
{
	if (hashT->nrElements == hashT->arraySize)
	{
		fprintf(stderr, "The hash table is full!");
		return;
	}

	int initialPos = hashT->hashFunction(id, hashT->arraySize), inc = 1;
	
	if (hashT->storageArray[initialPos].flag == PLACED)
	{
		int i = initialPos;
		
		do
		{
			if (hashT->storageArray[i].flag == PLACED && hashT->storageArray[i].data != NULL && hashT->storageArray[i].data->id == id) {
				strcpy(hashT->storageArray[i].data->name, name);
				return;
			}

			i = (i + inc * C1 + inc * inc++ * C2) % hashT->arraySize;
		} while (inc != hashT->arraySize && hashT->storageArray[i].flag == PLACED);

		if (hashT->storageArray[i].flag != PLACED) {
			hashT->storageArray[i].flag = PLACED;
			hashT->storageArray[i].data = newEntry(id, name);
		}
		else
			fprintf(stderr, "The hash table is full!");
	}
	else {
		hashT->storageArray[initialPos].flag = PLACED;
		hashT->storageArray[initialPos].data = newEntry(id, name);
	}

	hashT->nrElements++;
}

void printHashTable(HashTableT *hashT)
{
	if (hashT->nrElements == 0)
	{
		std::cout << "\nThe hashTable is empty\n";
		return;
	}

	std::cout << "\n\n";
	for (int i = 0; i < hashT->arraySize; i++)
	{
		if (hashT->storageArray[i].flag == PLACED)
			std::cout << hashT->storageArray[i].data->name << "\n";
		else
			std::cout << "-- Empty bucket --\n";
	}
}

int findHashTable(HashTableT *hashT, int id, char *name, int *operations)
{
	int initialPos = hashT->hashFunction(id, hashT->arraySize), inc = 1;
	*operations = 1;

	if (hashT->storageArray[initialPos].flag == PLACED && hashT->storageArray[initialPos].data != NULL && compareEntry(hashT->storageArray[initialPos].data, id, name))
		return initialPos;
	else
	{
		int i = initialPos;

		do
		{
			i = (i + inc * C1 + inc * inc++ * C2) % hashT->arraySize;
			(*operations)++;
		} while (inc != hashT->arraySize && hashT->storageArray[i].flag != EMPTY && !compareEntry(hashT->storageArray[i].data, id, name));

		if (hashT->storageArray[i].flag == PLACED && hashT->storageArray[i].data != NULL && compareEntry(hashT->storageArray[i].data, id, name))
			return i;
		else
			return NOT_FOUND;
	}
}

void deleteHashTable(HashTableT *hashT, int id, char *name)
{
	int operations, found = findHashTable(hashT, id, name, &operations);
	
	if (found != NOT_FOUND)
	{
		hashT->storageArray[found].flag = DELETED;
		free(hashT->storageArray[found].data);
		hashT->storageArray[found].data = NULL;
		hashT->nrElements--;
	}
	else
		fprintf(stderr, "The node to delete wasn't found!\n");
}

void purgeHashTable(HashTableT *hashT)
{
	for (int i = 0; i < hashT->arraySize; i++)
	{
		free(hashT->storageArray[i].data);
		hashT->storageArray[i].data = NULL;
	}

	free(hashT->storageArray);
	free(hashT);
}
