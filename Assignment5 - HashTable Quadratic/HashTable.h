#ifndef  HASHTABLE_H_
#define HASHTABLE_H_

#define NOT_FOUND -1
#define EMPTY 0
#define PLACED 1
#define DELETED 2
#define C1 1
#define C2 3

typedef struct entry {
	int id;
	char name[30];
} EntryT;

typedef struct HashEntry {
	int flag;
	EntryT *data;
} HashTEntry;

typedef struct HashT {
	int (*hashFunction)(int, int);
	HashTEntry *storageArray;
	int arraySize;
	int nrElements;
} HashTableT;

extern HashTableT *createHashTable(int (*f)(int, int), int);
extern void insertHashTable(HashTableT*, int, char*);
extern void printHashTable(HashTableT*);
extern void deleteHashTable(HashTableT*, int, char*);
extern void purgeHashTable(HashTableT*);
extern int findHashTable(HashTableT*, int, char*, int*);

#endif // ! HASHTABLE_H_

