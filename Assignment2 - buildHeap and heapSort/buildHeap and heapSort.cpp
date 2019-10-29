/**
 * @author Socaci Radu Andrei
 * @group 30421
 *
 * Compare the 2 ways of building a heap: top down using insert and bottom up using heapify
 *
 * Both algorithms do not require additional memory.
 *
 * In the average case, but the one based on heapify and constructing
 * the tree from the bottom has a smaller coefficient and is therefore outperforming its counterpart in practice.
 *
 * In the worst case, the difference between the 2 methods of building a heap is even more dramatic. Although they are still both O(n), 
 * the buttom up technique is way faster.
 */

#include <iostream>
#include <conio.h>
#include "Profiler.h"

#define MAX_SIZE 10000
#define INCREMENT 100

Profiler profiler("Starting Values");

int parent(int);

/**
* Helper functions for program flow
*/

// swaps 2 variables 
void swap(int *x, int *y) {
	int temp = *x;
	*x = *y;
	*y = temp;
}

// prints the contents of an int array of a given size
void printArray(int *intArray, int size) {
	for (int i = 0; i < size; i++) {
		std::cout << intArray[i] << " ";
	}

	std::cout << "\n";
}

// checks whether an array has the heap property
bool isHeap(int *intArray, int size) {
	for (int i = 1; i < size; i++) {
		// if there is a child that holds a bigger value than its parent, then the heap property is violated
		if (intArray[i] > intArray[parent(i)]) {
			return false;
		}
	}

	// if no such child could be found in the array, the array has the heap property
	return true;
}

// prints an error if the given array does not have the heap property
bool printError(int *intArray, int size) {
	if (!isHeap(intArray, size)) {
		std::cout << "\nThe heap property is violated!\n";
		return true;
	}

	return false;
}

/**
* Helper functions for heap construction
*/

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
void insertHeap(int *intArray, int *heapSize, int size, int keyToInsert, Operation* o) {
	// increase heap size
	(*heapSize)++;
	
	// the size of the heap needs to be smaller than the size of the array
	if ((*heapSize) - 1 >= size) {
		std::cout << "\nThe size of the requested heap is greater than the size of the array!\n";
		return;
	}

	// insert the new element as the last leaf in the heap
	intArray[(*heapSize) - 1] = keyToInsert; // 0 based array
	o->count();

	int i = (*heapSize) - 1;

	// as long as the value in the new item is greater thant the value of its parent, swap them
	while (i > 0 && intArray[parent(i)] < intArray[i]) {
		swap(&intArray[parent(i)], &intArray[i]);
		i = parent(i);

		o->count(4);
	}

	if (i > 0) {
		o->count();
	}
}

// given 2 heaps and a new element, creates one heap
void heapify(int *intArray, int indexOfRoot, int size, Operation* o) {
	int largest = indexOfRoot, leftChild = indexOfRoot * 2 + 1, rightChild = indexOfRoot * 2 + 2; // 0 based array

	// update index of the element with the largest value, if necessary
	if (leftChild < size && intArray[leftChild] > intArray[largest]) {
		largest = leftChild;
	}
	if (leftChild < size) {
		o->count();
	}

	// update index of the element with the largest value, if necessary
	if (rightChild < size && intArray[rightChild] > intArray[largest]) {
		largest = rightChild;
	}
	if (rightChild < size) {
		o->count();
	}

	// if the element with the largest value != root, swap them and call heapify again on the index of the largest element
	if (largest != indexOfRoot) {
		swap(&intArray[largest], &intArray[indexOfRoot]);
		o->count(3);

		heapify(intArray, largest, size, o);
	}
}

/**
* Functions to build the heap
*/

void buildHeapBottomUp(int *intArray, int size) {
	Operation o = profiler.createOperation("operationsBottomUp", size);

	// starting from the first parent, take it's children (leafs are heaps) and its index and use heapify to build a bigger heap
	for (int indexOfRoot = size / 2 - 1; indexOfRoot >= 0; indexOfRoot--) {
		heapify(intArray, indexOfRoot, size, &o);
	}
}

int buildHeapTopDown(int *intArray, int size) {
	Operation o = profiler.createOperation("operationsTopDown", size);

	// originally the size of the heap is 1, since the first element of the array is already a heap by itself
	int heapSize = 1;

	// starting from the second element, insert it in the previously existing heap
	for (int i = 1; i < size; i++) {
		// automatically updates the size of the heap
		insertHeap(intArray, &heapSize, size, intArray[i], &o);
	}

	return heapSize;
}

/**
* heap sort function using top down heap construction
*/

void heapSort(int *intArray, int size) {
	Operation o = profiler.createOperation("operationsHeapSort1", size);

	// build the heap. As a consequence, the element with the greatest value is placed at the root of the heap
	buildHeapBottomUp(intArray, size);
	const int indexOfRoot = 0;

	for (int i = size - 1; i >= 0; i--) {
		// put the maximum element in its final position (as last element in the array) by swapping it with the last element in the array
		swap(&intArray[indexOfRoot], &intArray[i]);
		o.count(3);

		// to make sure the heap property holds, we call heapify on the new array, disregarding the previously swapped elements
		heapify(intArray, indexOfRoot, i, &o);
	}

	profiler.addSeries("operationsHeapSort", "operationsHeapSort1", "operationsBottomUp");
}

void averageCase(void) {
	int baseArray[MAX_SIZE], intArray[MAX_SIZE], size, samples;

	for (size = 100; size <= MAX_SIZE; size += INCREMENT) {
		// since we are in the average case, the number of experiments is > 1 and we take the mean value.
		for (samples = 0; samples < 5; samples++) {
			FillRandomArray(baseArray, size);

			// we need to make a copy of the randomly generated array, since the construct heap fuctions alterate the array given as parameter 
			CopyArray(intArray, baseArray, size);
			buildHeapTopDown(intArray, size);
			printError(intArray, size);

			CopyArray(intArray, baseArray, size);
			buildHeapBottomUp(intArray, size);
			printError(intArray, size);
		}
	}

	// take the mean value
	profiler.divideValues("operationsTopDown", 5);
	profiler.divideValues("operationsBottomUp", 5);

	profiler.createGroup("heapConstructionComparisonAverage", "operationsTopDown", "operationsBottomUp");

	profiler.reset("Worst Case Evaluation");
}

void worstCase(void) {
	int baseArray[MAX_SIZE], intArray[MAX_SIZE], size;

	for (size = 100; size <= MAX_SIZE; size += INCREMENT) {
		FillRandomArray(baseArray, size, 0, 50000, true, 1);

		// we need to make a copy of the randomly generated array, since the construct heap fuctions alterate the array given as parameter 
		CopyArray(intArray, baseArray, size);
		buildHeapTopDown(intArray, size);
		printError(intArray, size);

		CopyArray(intArray, baseArray, size);
		buildHeapBottomUp(intArray, size);
		printError(intArray, size);
	}

	profiler.createGroup("heapConstructionComparisonWorst", "operationsTopDown", "operationsBottomUp");

	profiler.showReport();
}

int main(void) {
	int intArray[MAX_SIZE], baseArray[MAX_SIZE];
	
	FillRandomArray(baseArray, 10, 0, 100);
	std::cout << "The initial array: ";
	printArray(baseArray, 10);
	std::cout << "\n";

	CopyArray(intArray, baseArray, 10);
	heapSort(intArray, 10);
	std::cout << "The sorted array: ";
	printArray(intArray, 10);

	CopyArray(intArray, baseArray, 10);
	int heapSize = buildHeapTopDown(intArray, 10);
	std::cout << "The heap constructed top down: ";
	if (!printError(intArray, 10)) {
		printArray(intArray, 10);
	}

	CopyArray(intArray, baseArray, 10);
	buildHeapBottomUp(intArray, 10);
	std::cout << "The heap constructed bottom up: ";
	if (!printError(intArray, 10)) {
		printArray(intArray, 10);
	}

	/*profiler.reset("average case evaluation");

	averageCase();
	worstCase();*/

	std::cout << "\n\nPress any key to continue...";
	_getch();
	return 0;
}
