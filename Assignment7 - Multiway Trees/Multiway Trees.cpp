/**
 * @author Socaci Radu Andrei
 * @group 30421
 *
 * Implement correctly and efficiently liniar time transformations between 3 different representations of a multi-way tree
 *
 * The parent -> multiway transformation runs in liniar time, since every node is processed only once.
 * If a node was already processed, we just return. In order to allocate the children vectors with exactly the same
 * size as the number of children, we need to make a pass and keep a frequency array from the very beginning.
 * Also, we need to keep the created nodes into an array, since we want to return if the node has already been processed.
 * 
 * The multiway -> binary transformation also runs in liniar time (every node is processed only once). 
 * No additional memory is employed.
 *
 */

#include <iostream>
#include <queue>

#define spacingH 7

using namespace std;

typedef struct node
{
	int key, nrChildren;
	node **children;
} MWTreeNodeT;

typedef struct bnode
{
	int key;
	bnode *left, *right;
} BTreeNodeT;

MWTreeNodeT *createNode(int key, int numberOfChildren)
{
	MWTreeNodeT *newNode = (MWTreeNodeT *)malloc(sizeof(MWTreeNodeT));

	newNode->key = key;
	newNode->nrChildren = numberOfChildren;
	newNode->children = (numberOfChildren > 0) ? (MWTreeNodeT **)malloc(sizeof(MWTreeNodeT *) * numberOfChildren) : NULL;
	for (int i = 0; i < numberOfChildren; i++)
	{
		newNode->children[i] = NULL;
	}

	return newNode;
}

void createNodeInMultiway(int *parent, int *numberOfChildren, int key, MWTreeNodeT **createdNodes, MWTreeNodeT **root)
{
	// the node was already created in the multiway tree
	if (createdNodes[key])
	{
		return;
	}

	// if the node was not created, create it
	createdNodes[key] = createNode(key, numberOfChildren[key]);

	// if the node does not have a parent (-1), that means it is the root
	if (parent[key - 1] == -1)
	{
		*root = createdNodes[key];
		return;
	}

	// if the parent of the node is not created, then we need to create it first
	if (!createdNodes[parent[key - 1]])
	{
		createNodeInMultiway(parent, numberOfChildren, parent[key - 1], createdNodes, root);
	}

	// otherwise, just retrieve the pointer to its parent and add the current node as a child
	MWTreeNodeT *parentNode = createdNodes[parent[key - 1]];
	int insertAt = 0;

	while (parentNode->children[insertAt])
	{
		insertAt++;
	}

	parentNode->children[insertAt] = createdNodes[key];
}

MWTreeNodeT *createMultiwayTree(int *parent, int parentSize)
{
	MWTreeNodeT **createdNodes = (MWTreeNodeT **)malloc(sizeof(MWTreeNodeT *) * (parentSize + 1));
	MWTreeNodeT *root = NULL;

	// array needed for the exact allocation of children vectors
	int *numberOfChildren = (int *)malloc(sizeof(int) * (parentSize + 1));

	for (int i = 0; i <= parentSize; i++)
	{
		numberOfChildren[i] = 0;
		createdNodes[i] = NULL;
	}

	for (int i = 1; i <= parentSize; i++)
	{
		numberOfChildren[parent[i - 1]]++;
	}

	for (int i = 1; i <= parentSize; i++)
	{
		createNodeInMultiway(parent, numberOfChildren, i, createdNodes, &root);
	}

	free(createdNodes);

	return root;
}

BTreeNodeT *createBNode(int key)
{
	BTreeNodeT *newNode = (BTreeNodeT *)malloc(sizeof(BTreeNodeT));

	newNode->key = key;
	newNode->left = newNode->right = NULL;

	return newNode;
}

void addChildrenToBTree(BTreeNodeT *pNode, MWTreeNodeT *sourceTree)
{
	// if the current node has no children, nothing to do
	if (!sourceTree->children || !pNode)
	{
		return;
	}

	// go as deep on the left branch as possible
	pNode->left = createBNode(sourceTree->children[0]->key);
	addChildrenToBTree(pNode->left, sourceTree->children[0]);

	BTreeNodeT *current = pNode->left;

	// for every node in the binary tree, put all its siblings to its right
	for (int i = 1; i < sourceTree->nrChildren; i++)
	{
		current->right = createBNode(sourceTree->children[i]->key);
		// check for any left links
		addChildrenToBTree(current->right, sourceTree->children[i]);
		current = current->right;
	}
}

BTreeNodeT *transformMultiwayToBinary(MWTreeNodeT *sourceTree)
{
	// the root of the binary tree is the root of the multiway tree
	BTreeNodeT *rootNode = createBNode(sourceTree->key);
	// call addChildren recursively
	// at the end we have the resulting tree in rootNode
	addChildrenToBTree(rootNode, sourceTree);

	return rootNode;
}

void printParentArray(int *pArray, int size)
{
	cout << "The parent array is: ";

	for (int i = 0; i < size; i++)
	{
		cout << pArray[i] << " ";
	}

	cout << "\n\n";
}

void printMultiwayTree(MWTreeNodeT *mwTree, int nrNodes)
{
	bool *markedNodes = (bool *)malloc(sizeof(bool) * (nrNodes + 1));
	int index;
	queue<MWTreeNodeT *> queue;

	for (int i = 0; i <= nrNodes; i++)
	{
		markedNodes[i] = false;
	}

	markedNodes[mwTree->key] = true;
	cout << mwTree->key << " ";
	queue.push(mwTree);
	while (!queue.empty())
	{
		MWTreeNodeT *temp = queue.front();
		queue.pop();
		index = 0;

		while (index < temp->nrChildren && !markedNodes[temp->children[index]->key])
		{
			markedNodes[temp->children[index]->key] = true;
			cout << temp->children[index]->key << " ";
			queue.push(temp->children[index]);
			index++;
		}
	}
}

// displays tree tilted 90 degrees to the left, horizontally
void printTree(BTreeNodeT *pTree, int level)
{
	if (!pTree)
		return;

	int distance = level * spacingH;
	//displays as many spaces as we need
	printf("%*s%d\n", distance, "", pTree->key); //* replaced with level at runtime
	printTree(pTree->left, level + 1);
	printTree(pTree->right, level);
}

void purgeTree(BTreeNodeT **pTree)
{
	if (*pTree)
	{
		purgeTree(&((*pTree)->left));
		purgeTree(&((*pTree)->right));
		free(*pTree);
		*pTree = NULL;
	}
}

int main()
{
	int parent[] = {2, 7, 5, 2, 7, 7, -1, 5, 2}, sizeParent = sizeof(parent) / sizeof(int);
	printParentArray(parent, sizeParent);

	MWTreeNodeT *multiWayTree = createMultiwayTree(parent, sizeParent);
	cout << "Multiway tree printed in BFS order: ";
	printMultiwayTree(multiWayTree, sizeParent);

	BTreeNodeT *binaryTree = transformMultiwayToBinary(multiWayTree);
	cout << "\n\nBinary tree resulted from the multiway tree conversion: \n\n";
	printTree(binaryTree, 0);
	purgeTree(&binaryTree);
}
