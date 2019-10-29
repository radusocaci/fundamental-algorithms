/**
 * @author Socaci Radu Andrei
 * @group 30421
 *
 * Merge k Ordered Lists Efficiently and analyse the time complexity in the average case.
 *
 * The space is O(n) since the algorithm requires a list holding the result, of size n
 *
 * In the average case, the algorithm runs in O(nlogk) time.
 */

#include <iostream>
#include "Profiler.h"
#include <string.h>

#define MAX_NR_OF_LISTS 1000
#define MAX_NR_ELEMENTS 10000

Profiler profiler("Second Part Average");

/**
* value = the value stored in the node as an int
* next = reference to the next node
*/
typedef struct node {
	int value;
	node *next;
} NodeT;

/**
* first = reference to the first node in the list
* last = reference to the last node in the list
* nrElements = number of nodes in the list
*/
typedef struct list {
	NodeT *first;
	NodeT *last;
	int nrElements;
} ListT;

void fatal_error(const char *msg)
{
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

// swaps 2 variables 
void swap(NodeT **x, NodeT **y) {
	NodeT *temp = *x;
	*x = *y;
	*y = temp;
}

// allocates and instantiates a new list
ListT *createListHead(void) {
	ListT *newList = (ListT*)malloc(sizeof(ListT));

	if (newList) {
		newList->first = newList->last = NULL;
		newList->nrElements = 0;
	}
	else {
		fatal_error("Could not allocate memory for a new list!");
	}

	// returns the newly created list
	return newList;
}

// allocates and instantiates a new node
NodeT *createNode(int value) {
	NodeT *newNode = (NodeT*)malloc(sizeof(NodeT));
	
	if (newNode) {
		newNode->value = value;
		newNode->next = NULL;
	}
	else {
		fatal_error("Could not allocate memory for a new node!");
	}

	// returns the newly created node
	return newNode;
}

// inserts a node with the value "value" in the list
void insertNode(ListT *listRef, int value) {
	NodeT *newNode = createNode(value);

	// if the list is empty, just make the first/last elements of the list point to the element to insert
	if (!listRef->first) {
		listRef->first = listRef->last = newNode;
	}
	else {
		listRef->last->next = newNode;
		listRef->last = listRef->last->next;
	}

	listRef->nrElements++;
}

// prints the contents of a list
void printList(ListT *listRef) {
	NodeT *current = listRef->first;

	if (!current) {
		std::cout << "The list is empty!";
		return;
	}

	while (current) {
		std::cout << current->value << " ";
		current = current->next;
	}

	std::cout << "\n";
}

// deallocates a list
void deallocateList(ListT *listRef) {
	NodeT *current = NULL;

	while (listRef->first) {
		current = listRef->first;
		listRef->first = listRef->first->next;
		free(current);
	}

	listRef->first = listRef->last = NULL;
	listRef->nrElements = 0;
}

// return the index of the parent of the node situated at position "index" in the array
int parent(int index) {
	if (index % 2 == 0) {
		return (index - 1) / 2;
	}
	else {
		return index / 2;
	}
}

// inserts a new element in the given heap, updating the heaps' size
void insertHeap(NodeT *nodeArray[], int *heapSize, int size, NodeT *keyToInsert, Operation* o) {
	// increase heap size
	(*heapSize)++;

	// the size of the heap needs to be smaller than the size of the array
	if ((*heapSize) - 1 >= size) {
		std::cout << "\nThe size of the requested heap is greater than the size of the array!\n";
		return;
	}

	// insert the new element as the last leaf in the heap
	nodeArray[(*heapSize) - 1] = keyToInsert; // 0 based array
	o->count();

	int i = (*heapSize) - 1;

	// as long as the value in the new item is greater thant the value of its parent, swap them
	while (i > 0 && nodeArray[parent(i)]->value > nodeArray[i]->value) {
		swap(&nodeArray[parent(i)], &nodeArray[i]);
		i = parent(i);

		o->count(4);
	}

	if (i > 0) {
		o->count();
	}
}

// given 2 heaps and a new element, creates one heap
void heapify(NodeT *nodeArray[], int indexOfRoot, int size, Operation* o) {
	int largest = indexOfRoot, leftChild = indexOfRoot * 2 + 1, rightChild = indexOfRoot * 2 + 2; // 0 based array

	// update index of the element with the largest value, if necessary
	if (leftChild < size && nodeArray[leftChild]->value < nodeArray[largest]->value) {
		largest = leftChild;
	}
	if (leftChild < size) {
		o->count();
	}

	// update index of the element with the largest value, if necessary
	if (rightChild < size && nodeArray[rightChild]->value < nodeArray[largest]->value) {
		largest = rightChild;
	}
	if (rightChild < size) {
		o->count();
	}

	// if the element with the largest value != root, swap them and call heapify again on the index of the largest element
	if (largest != indexOfRoot) {
		swap(&nodeArray[largest], &nodeArray[indexOfRoot]);
		o->count(3);

		heapify(nodeArray, largest, size, o);
	}
}

// extracts the root (smallest element) from the min heap
NodeT *extractHeap(NodeT *nodeArray[], int *heapSize, Operation *o) {
	if (*heapSize < 1) {
		fatal_error("heap underflow!");
	}

	NodeT *min = nodeArray[0];
	nodeArray[0] = nodeArray[*heapSize - 1];
	(*heapSize)--;

	heapify(nodeArray, 0, *heapSize, o);
	return min;
}

// given k ascending lists, merges them in O(nlogk)
ListT *mergeLists(int k, ListT *listArray[], int size) {
	Operation o = profiler.createOperation("mergeOperations", size);

	// list that will hold the result
	ListT *newList = createListHead();
	int heapSize = 0;
	NodeT *heapArray[MAX_NR_OF_LISTS];

	// insert first elements from each and every list into the heap
	for (int i = 0; i < k; i++) {
		insertHeap(heapArray, &heapSize, k, listArray[i]->first, &o);
	}

	// as long as there are still elements in the heap
	while (heapSize != 0) {
		// extract the min element
		NodeT *top = extractHeap(heapArray, &heapSize, &o);

		// if the list that gave the element still has elements, insert the next into the heap
		if (top->next) {
			insertHeap(heapArray, &heapSize, k, top->next, &o);
		}

		// add the extracted node to the resulting list
		insertNode(newList, top->value);
	}

	// return the list resulted from merging all the other k lists
	return newList;
}

// given a random array and an empty list, copies the array into the list
void arrayToList(int *intArray, ListT *listRef, int length) {
	if (listRef->nrElements != 0) {
		deallocateList(listRef);
	}
	
	for (int i = 0; i < length; i++) {
		insertNode(listRef, intArray[i]);
	}
}

// generates an array holding the size of each list s.t. the total number of elements is n
// the lists do not necessarily have the same number of elements
void generateListSizeArray(int k, int n, int *listSizeArray) {
	// if the number of lists is equal with the number of elements, have k 1 element lists
	if (n / k == 1) {
		for (int i = 0; i < k; i++) {
			listSizeArray[i] = 1;
		}

		return;
	}

	// otherwise, for each list take a size between 1 and n/k
	FillRandomArray(listSizeArray, k, 1, n / k);
	int diff = n;

	// compute the remaining elements needed to reach n elements
	for (int i = 0; i < k; i++) {
		diff -= listSizeArray[i];
	}

	// equally distribute the remaining elements throughout the k lists
	int incrementStep = diff / k;
	for (int i = 0; i < k-1; i++) {
		listSizeArray[i] += incrementStep;
		diff -= incrementStep;
	}

	// put the remaining elements (if any) in the last list
	listSizeArray[k - 1] += diff;
}

// generates k sorted lists of arbitrary length
void populateLists(int k, ListT *listArray[], int *listSizeArray) {
	int auxArray[MAX_NR_ELEMENTS];

	for (int i = 0; i < k; i++) {
		listArray[i] = createListHead();
		FillRandomArray(auxArray, listSizeArray[i], 0, 50000, false, 1);
		arrayToList(auxArray, listArray[i], listSizeArray[i]);
	}
}

void averageCase(void) {
	ListT *listArray[MAX_NR_OF_LISTS];
	int listSizeArray[MAX_NR_OF_LISTS];
	const int kValue[3] = { 5, 10, 100 };

	for (int i = 0; i < 3; i++) {
		for (int j = 100; j <= MAX_NR_ELEMENTS; j += 100) {
			generateListSizeArray(kValue[i], j, listSizeArray);
			populateLists(kValue[i], listArray, listSizeArray);

			ListT *sortedList = mergeLists(kValue[i], listArray, j);

			for (int i = 0; i < kValue[i]; i++) {
				deallocateList(listArray[i]);
			}

			deallocateList(sortedList);
		}

		switch (i) {
			case 0: profiler.createGroup("k=5 Average Case", "mergeOperations");
					break;
			case 1: profiler.createGroup("k=10 Average Case", "mergeOperations");
					break;
			case 2: profiler.createGroup("k=100 Average Case", "mergeOperations");
		}

		profiler.reset("First Part Average");
	}

	for (int i = 10; i <= 500; i += 10) {
		generateListSizeArray(i, MAX_NR_ELEMENTS, listSizeArray);
		populateLists(i, listArray, listSizeArray);

		ListT *sortedList = mergeLists(i, listArray, i);

		for (int i = 0; i < i; i++) {
			deallocateList(listArray[i]);
		}

		deallocateList(sortedList);
	}

	profiler.createGroup("n=10000 Average Case", "mergeOperations");
	
	profiler.showReport();
}

int main(void)
{
	ListT *listArray[MAX_NR_OF_LISTS];
	int listSizeArray[MAX_NR_OF_LISTS];

	generateListSizeArray(8, 80, listSizeArray);
	populateLists(8, listArray, listSizeArray);

	std::cout << "The lists to sort:\n";

	for (int i = 0; i < 8; i++) {
		printList(listArray[i]);
	}

	std::cout << "\n\nThe sorted list:\n";

	ListT *sortedList = mergeLists(8, listArray, 8);
	printList(sortedList);
	deallocateList(sortedList);

	for (int i = 0; i < 8; i++) {
		deallocateList(listArray[i]);
	}

	/*averageCase();*/

	return 0;
}