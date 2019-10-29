/**
 * @author Socaci Radu Andrei
 * @group 30421
 *
 * Compare the following sorting methods: bubble sort, insertion sort and selection sort.
 *
 * The time complexity is O(1) for all sorting algorithms presented below, since they do not require additional memory.
 *
 * Analysing the running time, we have 3 cases: Average, Best and Worst.
 *
 * In the average case, bubble sort is the slowest algorithm both assignments and comparisons wise. Although insertion sort's comparisons and assignments are quadratic
 * their growth rate is much smaller than the one of bubble sort. Selection sort outperforms both in terms of assignments (linear), but since it performs more comparisons
 * than insertion sort, the total performance is the same with ins sort.
 *
 * In the worst case, in terms of assignments, selection sort is again the only linear performing algorithm, but insertion sort is still outperforming bubble sort.
 * In terms of comparisons, all curves are quadratic, bubble sort still having the worst performance overall.
 * 
 * In the best case, selection sort is the worst of the 3, since the number of comparisons it performs is always the same (meaning that the algorithm is not adaptive).
 * Bubble sort manages to outperform insertion sort by a small margin, thanks to it performing no assignments when the input array is already sorted in ascending order.
 *
 * In terms of stability, my implementation of selection sort is not stable, while the other 2 are stable.
 *
 */

#include <iostream>
#include <conio.h>
#include "Profiler.h"

#define MAX_SIZE 10000

Profiler profiler("Average Case Evaluation");

/** 
 * Helper functions 
 */

void swap(int *x, int *y) {
	int temp = *x;
	*x = *y;
	*y = temp;
}

void printArray(int *intArray, int size) {
	for (int i = 0; i < size; i++) {
		std::cout << intArray[i] << ' ';
	}

	std::cout << std::endl;
}

/**
 * Sorting algorithms
 */

void selectionSort(int *intArray, int size) {
	// used by the profiler
	Operation assignments = profiler.createOperation("selectionSortAss", size);
	Operation comparisons = profiler.createOperation("selectionSortComp", size);

	int i, j, minPos;
	for (i = 0; i < size - 1; i++) {
		minPos = i;

		for (j = i + 1; j < size; j++) {
			// if there is an element smaller than the element at index minPos, we update minPos with the new value
			if (intArray[j] < intArray[minPos])
				minPos = j;
			// it doesn't matter whether the if statement evaluates to true or false, we are still comparing the elements
			comparisons.count();
		}

		// if the value of minPos changed, we need to swap it in place
		if (minPos != i) {
			swap(&intArray[i], &intArray[minPos]);
			// swapping 2 elements requires 3 assignments
			assignments.count(3);
		}
	}
}

void bubbleSort(int *intArray, int size) {
	// used by the profiler
	Operation assignments = profiler.createOperation("bubbleSortAss", size);
	Operation comparisons = profiler.createOperation("bubbleSortComp", size);

	bool done;
	do {
		// if the array is already sorted, we don't need to iterate through it anymore
		done = true;
		for (int i = 0; i < size - 1; i++) {
			// if 2 consecutive elements are not ordered appropriately, we swap them.
			if (intArray[i] > intArray[i + 1]) {
				done = false;
				swap(&intArray[i], &intArray[i + 1]);
				// swapping 2 elements requires 3 assignments
				assignments.count(3);
			}
			// it doesn't matter whether the if statement evaluates to true or false, we are still comparing the elements
			comparisons.count();
		}
	} while (done == false);
}

void insertionSort(int *intArray, int size) {
	// used by the profiler
	Operation assignments = profiler.createOperation("insertionSortAss", size);
	Operation comparisons = profiler.createOperation("insertionSortComp", size);

	int temp = 0, j, i;
	for (i = 1; i < size; i++) {
		temp = intArray[i];
		j = i - 1;
		
		// as long as the element to insert is < the elements to it's left, we shift those elements to the right by 1 position
		while (j >= 0 && temp < intArray[j]) {
			intArray[j + 1] = intArray[j];
			j--;
			assignments.count();
			comparisons.count();
		}

		// if the while failed because of the second condition, then we need to count one more comparison
		if (j >= 0) {
			comparisons.count();
		}

		// now, the element to insert is in its place.
		intArray[j + 1] = temp;
		// both the beginning and ending assignments are counted here
		assignments.count(2);
	}
}

void averageCase(void) {
	int base[MAX_SIZE], toOrder[MAX_SIZE], size, samples;

	for (size = 100; size < MAX_SIZE; size += 100) {
		// since we are in the average case, the number of experiments is > 1 and we take the mean value.
		for (samples = 0; samples < 5; samples++) {
			FillRandomArray(base, size);
			
			// we need to make a copy of the randomly generated array, since the sorting fuctions alterate the array given as parameter 
			CopyArray(toOrder, base, size);
			selectionSort(toOrder, size);

			CopyArray(toOrder, base, size);
			bubbleSort(toOrder, size);

			CopyArray(toOrder, base, size);
			insertionSort(toOrder, size);
		}
	}

	profiler.addSeries("insertionSortTotal", "insertionSortAss", "insertionSortComp");
	profiler.addSeries("selectionSortTotal", "selectionSortAss", "selectionSortComp");
	profiler.addSeries("bubbleSortTotal", "bubbleSortAss", "bubbleSortComp");
	
	// take the mean value
	profiler.divideValues("insertionSortTotal", 5);
	profiler.divideValues("insertionSortAss", 5);
	profiler.divideValues("insertionSortComp", 5);
	profiler.divideValues("selectionSortTotal", 5);
	profiler.divideValues("selectionSortAss", 5);
	profiler.divideValues("selectionSortComp", 5);
	profiler.divideValues("bubbleSortTotal", 5);
	profiler.divideValues("bubbleSortAss", 5);
	profiler.divideValues("bubbleSortComp", 5);

	profiler.createGroup("comparisonsAverage", "insertionSortComp", "selectionSortComp", "bubbleSortComp");
	profiler.createGroup("assignmentsAverage", "insertionSortAss", "selectionSortAss", "bubbleSortAss");
	profiler.createGroup("selectionSortAs", "selectionSortAss");
	profiler.createGroup("totalAverage", "insertionSortTotal", "selectionSortTotal", "bubbleSortTotal");

	profiler.reset("Worst Case Evaluation");
}

void worstCase(void) {
	int base[MAX_SIZE], toOrder[MAX_SIZE], size, index;
	// used for worst case of selection sort (alternating array of decreasing/increasing subsequences)
	const int start = 5000;

	for (size = 100; size < MAX_SIZE; size += 100) {
		FillRandomArray(base, size, 0, 50000, false, 2);
		index = 0;

		CopyArray(toOrder, base, size);
		bubbleSort(toOrder, size);

		CopyArray(toOrder, base, size);
		insertionSort(toOrder, size);

		// generate a worst case array for selection sort
		toOrder[index] = size / 2;
		index++;
		for (int i = 1; i <= size / 2; i++) {
			toOrder[index++] = toOrder[0] - i;
			toOrder[index++] = toOrder[0] + i;
		}
		selectionSort(toOrder, size);
	}

	profiler.addSeries("insertionSortTotal", "insertionSortAss", "insertionSortComp");
	profiler.addSeries("selectionSortTotal", "selectionSortAss", "selectionSortComp");
	profiler.addSeries("bubbleSortTotal", "bubbleSortAss", "bubbleSortComp");

	profiler.createGroup("comparisonsWorst", "insertionSortComp", "selectionSortComp", "bubbleSortComp");
	profiler.createGroup("assignmentsWorst", "insertionSortAss", "selectionSortAss", "bubbleSortAss");
	profiler.createGroup("totalWorst", "insertionSortTotal", "selectionSortTotal", "bubbleSortTotal");

	profiler.reset("Best Case Evaluation");
}

void bestCase(void) {
	int base[MAX_SIZE], toOrder[MAX_SIZE], size;

	for (size = 100; size < MAX_SIZE; size += 100) {
		FillRandomArray(base, size, 0, 50000, false, 1);

		// we need to make a copy of the randomly generated array, since the sorting fuctions alterate the array given as parameter 
		CopyArray(toOrder, base, size);
		selectionSort(toOrder, size);

		CopyArray(toOrder, base, size);
		bubbleSort(toOrder, size);

		CopyArray(toOrder, base, size);
		insertionSort(toOrder, size);
	}

	profiler.addSeries("insertionSortTotal", "insertionSortAss", "insertionSortComp");
	profiler.addSeries("selectionSortTotal", "selectionSortAss", "selectionSortComp");
	profiler.addSeries("bubbleSortTotal", "bubbleSortAss", "bubbleSortComp");

	profiler.createGroup("comparisonsBest", "insertionSortComp", "selectionSortComp", "bubbleSortComp");
	profiler.createGroup("insertionAndBubbleComp", "insertionSortComp", "bubbleSortComp");
	profiler.createGroup("assignmentsBest", "insertionSortAss", "selectionSortAss", "bubbleSortAss");
	profiler.createGroup("totalBest", "insertionSortTotal", "selectionSortTotal", "bubbleSortTotal");
	profiler.createGroup("insertionAndBubbleTotal", "insertionSortTotal", "bubbleSortTotal");

	profiler.reset("Done");
}

int main(void) {
	/* int base[MAX_SIZE], toOrder[MAX_SIZE];

	// Example of execution for every sorting algorithm on a randomized array of length 10
	FillRandomArray(base, 10, 1, 100);
	std::cout << "The starting array is: ";
	printArray(base, 10);
		
	CopyArray(toOrder, base, 10);
	selectionSort(toOrder, 10);
	std::cout << std::endl << "Sorted using selectionSort: ";
	printArray(toOrder, 10);

	CopyArray(toOrder, base, 10);
	insertionSort(toOrder, 10);
	std::cout << "Sorted using insertionSort: ";
	printArray(toOrder, 10);

	CopyArray(toOrder, base, 10);
	bubbleSort(toOrder, 10);
	std::cout << "Sorted using bubbleSort: ";
	printArray(toOrder, 10);
	std::cout << "Press any key to continue";

	profiler.reset("Average Case Evaluation"); */

	averageCase();
	worstCase();
	bestCase();

	_getch();
	return 0;
}