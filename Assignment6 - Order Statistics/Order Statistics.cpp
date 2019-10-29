/**
 * @author Socaci Radu Andrei
 * @group 30421
 *
 * Implement correctly and efficiently the management operations of an order statistics tree
 *
 * The build operation takes O(n) as is only performed once for every tree.
 *
 * The select operation takes O(logn), thus being more efficient than quickSelect, with the cost of building and mastaining a balanced tree.
 *
 * Since we decrese the dimension from the start, the delete operation, in the worst case, still only has to check an entire branch. Thus,
 * the running time in the worst case of the delete operation is O(logn).
 * 
 */

#include <iostream>
#include "Profiler.h"

#define spacingH 7
#define MAX_NR_NODES 10000
#define INCREMENT 100

using namespace std;

Profiler profiler("Average Case Evaluation");

typedef struct node {
	int key, dimension;
	struct node *left, *right;
} TreeNode;

void fatal_error(const char *msg) {
	fprintf(stderr, msg);
	exit(EXIT_FAILURE);
}

int *buildArray(int n) {
	int *newArray = (int*)malloc(sizeof(int) * n);

	if (newArray) {
		for (int i = 0; i < n; i++) {
			newArray[i] = i + 1;
		}
	}
	else {
		fatal_error("Could not allocate array!");
	}

	return newArray;
}

TreeNode *allocNode(int key) {
	TreeNode *newNode = (TreeNode*)malloc(sizeof(TreeNode));

	if (newNode) {
		newNode->key = key;
		newNode->dimension = 1;
		newNode->left = newNode->right = NULL;
	}
	else
		fatal_error("Could not allocate node!");

	return newNode;
}

TreeNode *buildBalancedTree(int *array, int left, int right, Operation *o) {
	if (left > right) {
		return NULL;
	}
	
	int mid = (left + right) / 2;

	o->count(3);
	TreeNode *tree = allocNode(array[mid]);

	tree->left = buildBalancedTree(array, left, mid - 1, o);
	tree->right = buildBalancedTree(array, mid + 1, right, o);

	tree->dimension += (tree->left) ? tree->left->dimension : 0;
	tree->dimension += (tree->right) ? tree->right->dimension : 0;

	o->count(2);

	return tree;
}

TreeNode *selectNode(TreeNode *pTree, int pos, Operation *o) {
	if (!pTree) {
		return NULL;
	}
	
	int nrLeft = 1;
	if (pTree->left) {
		nrLeft = pTree->left->dimension + 1;
		o->count();
	}

	o->count();

	if (pos == nrLeft) {
		return pTree;
	}
	else if (pos < nrLeft) {
		return selectNode(pTree->left, pos, o);
	}
	else {
		return selectNode(pTree->right, pos - nrLeft, o);
	}
}

TreeNode *findMax(TreeNode *pTree, Operation *o)
{
	if (!pTree)
		return NULL;

	if (pTree->right) {
		o->count();
		return findMax(pTree->right, o);
	}
	else
		return pTree;
}

TreeNode *deleteNode(TreeNode *pTree, TreeNode *nodeToDelete, Operation *o)
{
	if (!pTree) {
		return NULL;
	}

	o->count();

	if (nodeToDelete->key < pTree->key) {
		pTree->dimension--;
		pTree->left = deleteNode(pTree->left, nodeToDelete, o);
	}
	else if (nodeToDelete->key > pTree->key) {
		pTree->dimension--;
		pTree->right = deleteNode(pTree->right, nodeToDelete, o);
	}
	else {
		if (pTree->right && pTree->left) {
			TreeNode *temp = findMax(pTree->left, o);
			o->count();
			pTree->key = temp->key;
			pTree->dimension--;
			pTree->left = deleteNode(pTree->left, temp, o);
		}
		else {
			TreeNode *toDel = pTree;
			if (pTree->right) {
				pTree = pTree->right;
			}
			else if (nodeToDelete->left) {
				pTree = pTree->left;
			}
			else {
				pTree = NULL;
			}

			free(toDel);
			o->count();
		}
	}

	return pTree;
}

// displays tree tilted 90 degrees to the left, horizontally
void printTree(TreeNode *pTree, int level)
{
	if (!pTree)
		return;

	printTree(pTree->right, level + 1);
	int distance = level * spacingH;
	//displays as many spaces as we need
	printf("%*s%d\n", distance, "", pTree->key); //* replaced with level at runtime
	printTree(pTree->left, level + 1);
}

void purgeTree(TreeNode **pTree)
{
	if (*pTree)
	{
		purgeTree(&((*pTree)->left));
		purgeTree(&((*pTree)->right));
		free(*pTree);
		*pTree = NULL;
	}
}

void averageCase() {
	for (int i = 100; i < MAX_NR_NODES; i += INCREMENT) {
		Operation o = profiler.createOperation("Avg Case Analysis", i);

		for (int samples = 0; samples < 4; samples++) {
			int *inputArray = buildArray(i), itemsLeft = i;
			TreeNode *tree = buildBalancedTree(inputArray, 0, i - 1, &o);

			for (int j = 0; j < i; j++) {
				int randomKey = rand() % itemsLeft-- + 1;
				TreeNode *selectedNode = selectNode(tree, randomKey, &o);
				tree = deleteNode(tree, selectedNode, &o);
			}

			purgeTree(&tree);
		}
	}

	profiler.divideValues("Avg Case Analysis", 5);
	profiler.createGroup("Average Case Analysis", "Avg Case Analysis");

	profiler.showReport();
}

int main()
{
	/* int arraySize = 11, *array = buildArray(arraySize);
	int randomKeys[3];
	Operation o = profiler.createOperation("demo", arraySize);
	
	TreeNode *tree = buildBalancedTree(array, 0, arraySize - 1, &o);
	printTree(tree, 0);

	cout << "\n\n";

	FillRandomArray(randomKeys, 3, 1, arraySize - 1, true, 0);

	for (int i = 0; i < 3; i++) {
		TreeNode *selectedNode = selectNode(tree, randomKeys[i], &o);
		cout << "Select position " << randomKeys[i] << ": " << selectedNode->key << "\n";
		tree = deleteNode(tree, selectedNode, &o);
	}

	cout << "\n\n";

	printTree(tree, 0);
	purgeTree(&tree); */

	averageCase();
}

