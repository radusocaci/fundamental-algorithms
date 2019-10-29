/**
 * @author Socaci Radu Andrei
 * @group 30421
 *
 * Analysis & Comparison of Advanced Sorting Methods – Heapsort and Quicksort
 *
 * Both algorithms do not require additional memory and have a running time in the worst case of O(nlogn). Neither algorithm is stable.
 * (actually quickSort takes time proportional to O(n^2) in the worst case, but the randomized version never enters the worst case).
 *
 * In the average case, quickSort is faster than heapSort, but the curve is not as stable since the running time of quickSort greatly depends 
 * on the selection of the pivot. Time: O(nlogn)	
 *
 * In the worst case, quickSort takes time proportional to O(n^2), and in theory is not optimal. This worst case occurs when the depth of the
 * execution tree is n - 1. Therefore, this case occurs when the pivot is either the smallest or the largest element in the array
 *
 * In the best case, quickSort is very close to the average case (which is desirable). The pivot needs to halve the array at each step
 * for this case to occur. 
 */

#include <iostream>
#include <ctime>
#include <conio.h>
#include "Profiler.h"

#define MAX_SIZE 10000
#define INCREMENT 100

Profiler profiler("Starting Values");

/**
* Helper functions for program flow
*/

// swaps 2 variables
void swap(int *x, int *y)
{
	int temp = *x;
	*x = *y;
	*y = temp;
}

// prints the contents of an int array of a given size
void printArray(int *intArray, int size)
{
	for (int i = 0; i < size; i++)
	{
		std::cout << intArray[i] << " ";
	}

	std::cout << "\n";
}

/**
* Helper functions for quick sort
*/

// used to eliminate the overhead needed for small arrays
void insertionSort(int *intArray, int left, int right, Operation *o)
{
	int temp = 0, j, i;
	for (i = left + 1; i <= right; i++)
	{
		temp = intArray[i];
		j = i - 1;

		// as long as the element to insert is < the elements to it's left, we shift those elements to the right by 1 position
		while (j >= left && temp < intArray[j])
		{
			intArray[j + 1] = intArray[j];
			j--;
			o->count(2);
		}

		// if the while failed because of the second condition, then we need to count one more comparison
		if (j >= left)
		{
			o->count();
		}

		// now, the element to insert is in its place.
		intArray[j + 1] = temp;
		// both the beginning and ending assignments are counted here
		o->count(2);
	}
}

// main partitioning function. Patitions around the last element of the array
int partition(int *intArray, int left, int right, Operation *o)
{
	int pivot = intArray[right], pivotPos = left - 1;
	o->count();

	for (int i = left; i < right; i++)
	{
		// if the element is < pivot, it needs to go to its right
		if (intArray[i] <= pivot)
		{
			pivotPos++;
			swap(&intArray[pivotPos], &intArray[i]);
			o->count(3);
		}

		o->count();
	}

	// after all < elements are in their final position, swap the pivot with the first > element
	pivotPos++;
	swap(&intArray[pivotPos], &intArray[right]);
	o->count(3);

	return pivotPos;
}

// better partitioning because the worst case is avoided.
int randomizedPartition(int *intArray, int left, int right, Operation *o)
{
	// choose a random index
	int pivot = rand() % (right - left) + left;

	// swap the last element with the element situated at the chosen index
	swap(&intArray[pivot], &intArray[right]);
	o->count(3);

	// now, the randomly selected pivot is the last element in the array, so we can call partitioni on it
	return partition(intArray, left, right, o);
}

/**
* Helper functions for heap construction
*/

// given 2 heaps and a new element, creates one heap
void heapify(int *intArray, int indexOfRoot, int size, Operation *o)
{
	int largest = indexOfRoot, leftChild = indexOfRoot * 2 + 1, rightChild = indexOfRoot * 2 + 2; // 0 based array

	// update index of the element with the largest value, if necessary
	if (leftChild < size && intArray[leftChild] > intArray[largest])
	{
		largest = leftChild;
	}
	if (leftChild < size)
	{
		o->count();
	}

	// update index of the element with the largest value, if necessary
	if (rightChild < size && intArray[rightChild] > intArray[largest])
	{
		largest = rightChild;
	}
	if (rightChild < size)
	{
		o->count();
	}

	// if the element with the largest value != root, swap them and call heapify again on the index of the largest element
	if (largest != indexOfRoot)
	{
		swap(&intArray[largest], &intArray[indexOfRoot]);
		o->count(3);

		heapify(intArray, largest, size, o);
	}
}

/**
* Functions to build the heap
*/

void buildHeapBottomUp(int *intArray, int size, Operation *o)
{
	// starting from the first parent, take it's children (leafs are heaps) and its index and use heapify to build a bigger heap
	for (int indexOfRoot = size / 2 - 1; indexOfRoot >= 0; indexOfRoot--)
	{
		heapify(intArray, indexOfRoot, size, o);
	}
}

/**
* Functions for the 2 main sorting algorithms
*/

void heapSort(int *intArray, int size)
{
	Operation o = profiler.createOperation("operationsHeapSort", size);

	// build the heap. As a consequence, the element with the greatest value is placed at the root of the heap
	buildHeapBottomUp(intArray, size, &o);
	const int indexOfRoot = 0;

	for (int i = size - 1; i >= 0; i--)
	{
		// put the maximum element in its final position (as last element in the array) by swapping it with the last element in the array
		swap(&intArray[indexOfRoot], &intArray[i]);
		o.count(3);

		// to make sure the heap property holds, we call heapify on the new array, disregarding the previously swapped elements
		heapify(intArray, indexOfRoot, i, &o);
	}
}

// quicksort with partitioning around the last element in the array (can enter the O(n^2) worst case)
void quickSort(int *intArray, int left, int right, Operation *o)
{
	// if the length of the array is less than 1, there is nothing to sort (we avoid useless overhead)
	if (right - left + 1 <= 1)
	{
		return;
	}
	// if the length of the array is smaller than 5, it's faster to just call a direct sorting method on that array
	else if (right - left + 1 <= 5)
	{
		insertionSort(intArray, left, right, o);
	}
	else
	{
		// put the pivot in its right place in the sorted array and return its position
		int pivot = partition(intArray, left, right, o);

		// recursively sort the elements to the left and right of the pivot
		quickSort(intArray, left, pivot - 1, o);
		quickSort(intArray, pivot + 1, right, o);
	}
}

// quicksort with partitioning around a random element of the array (avoids the O(n^2) worst case)
void quickSortRandomized(int *intArray, int left, int right, Operation *o)
{
	// if the length of the array is less than 1, there is nothing to sort (we avoid useless overhead)
	if (right - left + 1 <= 1)
	{
		return;
	}
	// if the length of the array is smaller than 5, it's faster to just call a direct sorting method on that array
	else if (right - left + 1 <= 5)
	{
		insertionSort(intArray, left, right, o);
	}
	else
	{
		// put the pivot in its right place in the sorted array and return its position
		int pivot = randomizedPartition(intArray, left, right, o);

		// recursively sort the elements to the left and right of the pivot
		quickSortRandomized(intArray, left, pivot - 1, o);
		quickSortRandomized(intArray, pivot + 1, right, o);
	}
}

// selects the ith greatest element of the array in O(n) time
int quickSelect(int *intArray, int left, int right, int pos)
{
	Operation o = profiler.createOperation("quickSelectOperations", right - left + 1);

	// if there is only one element in the array, return it
	if (left == right)
	{
		return intArray[left];
	}

	// take a random pivot and return its position in the sorted array
	int pivotRelativePos = randomizedPartition(intArray, left, right, &o);
	// number of elements to the left of the pivot, including the pivot
	int nrElementsLeft = pivotRelativePos - left + 1;

	// if the pivot is at the requested position, return the pivot
	if (pos == nrElementsLeft)
	{
		return intArray[pivotRelativePos];
	}
	// if the position is less than the position of the pivot, we need to search in the left side (smaller elements)
	else if (pos < nrElementsLeft)
	{
		return quickSelect(intArray, left, pivotRelativePos - 1, pos);
	}
	// otherwise we need to search on the right side, but taking all the elements to the left out of the equation. Hence, we are searching for pos - nrElemLeft
	else
	{
		return quickSelect(intArray, pivotRelativePos + 1, right, pos - nrElementsLeft);
	}
}

void averageCase(void)
{
	int baseArray[MAX_SIZE], intArray[MAX_SIZE], size, samples;

	for (size = 100; size <= MAX_SIZE; size += INCREMENT)
	{
		Operation o = profiler.createOperation("operationsQuickSort", size);
		// since we are in the average case, the number of experiments is > 1 and we take the mean value.
		for (samples = 0; samples < 5; samples++)
		{
			FillRandomArray(baseArray, size);

			// we need to make a copy of the randomly generated array, since the construct heap fuctions alterate the array given as parameter
			CopyArray(intArray, baseArray, size);
			heapSort(intArray, size);

			CopyArray(intArray, baseArray, size);
			quickSortRandomized(intArray, 0, size - 1, &o);
		}
	}

	// take the mean value
	profiler.divideValues("operationsHeapSort", 5);
	profiler.divideValues("operationsQuickSort", 5);

	profiler.createGroup("Average Case", "operationsHeapSort", "operationsQuickSort");

	profiler.reset("Worst Case Evaluation");
}

void worstCase(void)
{
	int baseArray[MAX_SIZE], size;

	for (size = 100; size <= MAX_SIZE; size += INCREMENT)
	{
		Operation o = profiler.createOperation("operationsQuickSort", size);

		FillRandomArray(baseArray, size, 0, 50000, true, 1);
		quickSort(baseArray, 0, size - 1, &o);
	}

	profiler.createGroup("Worst Case quickSort", "operationsQuickSort");

	profiler.reset("Best Case Evaluation");
}

// IMPORTANT!! THIS FUNCTION ASSUMES AN ASCENDINGLY ORDERED ARRAY AS INPUT. helper function used for generating a best care array for quickSort with pivot as last element
void generateBestCaseArray(int *intArray, int left, int right)
{
	if (right - left < 2)
	{
		return;
	}

	// take the middle of the array
	int middle = (right + left) / 2;

	// swap the last element and the middle element
	swap(&intArray[right], &intArray[middle]);
	// recursively optimize the left and right halfs
	generateBestCaseArray(intArray, left, middle - 1);
	generateBestCaseArray(intArray, middle + 1, right);
}

void bestCase(void)
{
	int baseArray[MAX_SIZE], size;

	for (size = 100; size <= MAX_SIZE; size += INCREMENT)
	{
		Operation o = profiler.createOperation("operationsQuickSort", size);

		FillRandomArray(baseArray, size, 0, 50000, true, 1);
		generateBestCaseArray(baseArray, 0, size - 1);
		quickSort(baseArray, 0, size - 1, &o);
	}

	profiler.createGroup("Best Case quickSort", "operationsQuickSort");

	profiler.showReport();
}

int main(void)
{
    srand(time(NULL));

    int intArray[MAX_SIZE], baseArray[MAX_SIZE];
	Operation o = profiler.createOperation("operationsQuickSort", 100);

	FillRandomArray(baseArray, 10, 0, 100);
	std::cout << "The initial array: ";
	printArray(baseArray, 10);
	std::cout << "\n";

	CopyArray(intArray, baseArray, 10);
	heapSort(intArray, 10);
	std::cout << "Sorted using heapSort: ";
	printArray(intArray, 10);

	CopyArray(intArray, baseArray, 10);
	quickSort(intArray, 0, 9, &o);
	std::cout << "Sorted using quickSort: ";
	printArray(intArray, 10);

	CopyArray(intArray, baseArray, 10);
	quickSortRandomized(intArray, 0, 9, &o);
	std::cout << "Sorted using quickSort (Randomized): ";
	printArray(intArray, 10);

	CopyArray(intArray, baseArray, 10);
	std::cout << "4th element in the array (quickSelectRandomized): " << quickSelect(intArray, 0, 9, 4);

	/* std::cout << "\n\n";
	FillRandomArray(intArray, 10, 0, 100, false, 1);
	generateBestCaseArray(intArray, 0, 9);
	printArray(intArray, 10); */

	/*profiler.reset("Average Case Evaluation");

	averageCase();
	worstCase();
	bestCase();*/

	std::cout << "\n\nPress any key to continue...";
	_getch();
	return 0;
}
