/**
 * @author Socaci Radu Andrei
 * @group 30421
 *
 * Implement correctly and efficiently the base operations for disjoint sets and Kruskal's algorithm using disjoint sets.
 *
 * The complexity of makeSet is O(1) since it has only atomic operations. We need to build n such sets, so, to build the entire setsArray, we need O(n).
 * 
 * The complexity of union and find is O(h) where h = height of the tree. h is kept under control using the union by rank and path compression.
 * 
 * Kruskal's algorithm takes O(ElogE) to sort the edges and then applyes union-find E times => O(ElogV). Therefor, total time complexity is O(ElogE + ElogV)
 */

#include <iostream>
#include <ctime>
#include <algorithm>
#include "Profiler.h"

#define MAX_NR_NODES 10000
#define NR_EDGES_PER_V 4

using namespace std;

Profiler profiler("Time Complexity Analysis");

typedef struct node
{
    int key, rank;
    node *parent;
} NodeT;

typedef struct edge
{
    int source, destination, weight;
} EdgeT;

typedef struct graph
{
    int nrVertices, nrEdges;
    EdgeT **edges;
} GraphT;

void fatalError(const char *msg)
{
    fprintf(stderr, msg);
    exit(EXIT_FAILURE);
}

NodeT *makeSet(int key, Operation *o)
{
    NodeT *newNode = (NodeT *)malloc(sizeof(NodeT));

    if (newNode)
    {
        o->count(3);

        newNode->key = key;
        newNode->rank = 0;
        newNode->parent = newNode;
    }
    else
        fatalError("Could not allocate space for a new set!");

    return newNode;
}

NodeT *findSet(NodeT *setElement, Operation *o)
{
    // uses path compression
    if (setElement->parent != setElement)
    {
        setElement->parent = findSet(setElement->parent, o);
        o->count(2);
    }

    return setElement->parent;
}

void unionSet(NodeT *firstSet, NodeT *secondSet, Operation *o)
{
    if (firstSet == secondSet)
    {
        return;
    }

    NodeT *firstRoot = findSet(firstSet, o);
    NodeT *secondRoot = findSet(secondSet, o);

    o->count();
    // already connected
    if (firstRoot == secondRoot)
    {
        return;
    }

    o->count(2);
    // weighted union (by rank)
    if (firstRoot->rank > secondRoot->rank)
    {
        secondRoot->parent = firstRoot;
    }
    else
    {
        firstRoot->parent = secondRoot;

        o->count();
        // rank only changes when the ranks of the items to join were equal
        if (firstRoot->rank == secondRoot->rank)
        {
            o->count();
            secondRoot->rank++;
        }
    }
}

void buildAllSets(int upperBound, NodeT **setsArray, Operation *o)
{
    for (int i = 0; i < upperBound; i++)
    {
        setsArray[i] = makeSet(i, o);
    }
}

GraphT *newGraph(int nrVertices, int nrEdges)
{
    GraphT *newGraph = (GraphT *)malloc(sizeof(GraphT));

    newGraph->nrVertices = nrVertices;
    newGraph->nrEdges = nrEdges;
    newGraph->edges = (EdgeT **)malloc(sizeof(EdgeT *) * nrEdges);

    return newGraph;
}

EdgeT *createNewEdge(int source, int destination, int weight)
{
    EdgeT *newEdge = (EdgeT *)malloc(sizeof(EdgeT));

    newEdge->source = source;
    newEdge->destination = destination;
    newEdge->weight = weight;

    return newEdge;
}

GraphT *createGraph(int nrVertices, int nrEdges)
{
    GraphT *graph = newGraph(nrVertices, nrEdges);
    int i = 0;

    while(i < nrEdges) {
        // ensure connectivity
        if(i < nrVertices - 1) {
            graph->edges[i] = createNewEdge(i, i + 1, rand() % 1000 + 1);
        }
        // add the other 3*nrVertices edges
        else {
            int randomSource = rand() % nrVertices, randomDestination = rand() % nrVertices;

            while(randomDestination == randomSource) {
                randomDestination = rand() % nrVertices;
            }

            graph->edges[i] = createNewEdge(randomSource, randomDestination, rand() % 1000 + 1);
        }

        i++;
    }

    return graph;
}

int compare(const void *a, const void *b) {
    return ((EdgeT*)a)->weight < ((EdgeT*)b)->weight;
}

// given a graph, returns MST
GraphT *kruskal(GraphT *graph, NodeT **setsArray, Operation *o) {
    GraphT *MST = newGraph(graph->nrVertices, graph->nrVertices - 1);
    int indexGraph = 0, indexMST = 0;

    sort(graph->edges, graph->edges + graph->nrEdges, compare);
    while(indexMST < MST->nrEdges) {
        EdgeT *currentEdge = graph->edges[indexGraph++];

        if(findSet(setsArray[currentEdge->source], o)->key != findSet(setsArray[currentEdge->destination], o)->key) {
            MST->edges[indexMST++] = currentEdge;
            unionSet(setsArray[currentEdge->source], setsArray[currentEdge->destination], o);
        }
    }   

    return MST;
}

void freeSets(NodeT **setsArray, int size) {
    for (int i = 0; i < size; i++) {
        free(setsArray[i]);
    }
}

void freeGraph(GraphT *graph) {
    for (int i = 0; i < graph->nrEdges; i++) {
        free(graph->edges[i]);
    }

    free(graph);
}

void averageCase()
{
    NodeT *setsArray[MAX_NR_NODES];

    for (int n = 100; n < MAX_NR_NODES; n += 100)
    {
        Operation o = profiler.createOperation("Average Case", n);

        buildAllSets(n, setsArray, &o);
        GraphT *graph = createGraph(n, 4 * n);
        GraphT *MST = kruskal(graph, setsArray, &o);
        // deallocate graph
        freeSets(setsArray, n);
        freeGraph(graph);
        // edges already deallocated
        free(MST);
        MST = NULL;
    }

    profiler.createGroup("Average Case Analysis", "Average Case");
    profiler.showReport();
}

void printAllEdges(GraphT *graph) {
    for (int i = 0; i < graph->nrEdges; i++) {
        cout << "Edge " << i + 1 << ": " << graph->edges[i]->source << " " << graph->edges[i]->destination << " " << graph->edges[i]->weight << "\n";
    }
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));

    /* int nrSets = 10;
    NodeT *setsArray[nrSets];
    Operation o = profiler.createOperation("Test Drive", nrSets);

    buildAllSets(nrSets, setsArray, &o);
    for (int i = 1; i <= 5; i++) {
        int j = rand() % nrSets;

        unionSet(setsArray[i], setsArray[j], &o);

        cout << "After union of " << i << " with " << j << ": " << i << "->key = " << setsArray[i]->key
             << " " << i << "->parent->key = " << setsArray[i]->parent->key << " find(" << j << ") = "
             << findSet(setsArray[j], &o)->key << "\n";
    }

    nrSets /= 2;

    freeSets(setsArray, nrSets);
    buildAllSets(nrSets, setsArray, &o);

    GraphT *graph = createGraph(nrSets, nrSets * 4);
    cout << "\n\nThe Graph is: \n\n";
    printAllEdges(graph);

    GraphT *MST = kruskal(graph, setsArray, &o);
    cout << "\n\nThe MST is: \n\n";
    printAllEdges(MST);
    
    freeGraph(graph);
    free(MST); */

    averageCase();

    return 0;
}
