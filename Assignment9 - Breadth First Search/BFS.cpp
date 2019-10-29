/**
 * @author Socaci Radu Andrei
 * @group 30421
 *
 * Implement correctly and efficiently the Breadth-First Search graph algorithm.
 *
 * The complexity of BFS is O(E + V), since we enqueue every vertex and only check one adj. list at a time,
 * for the white children of a grey vertex.
 * 
 * The BFS traversal creates a BFS tree, with the dest from the source node to all the other nodes being minimal.
 */

#include <iostream>
#include <list>
#include <queue>
#include <ctime>
#include <algorithm>
#include "Profiler.h"

using namespace std;

#define NULLP -1
#define SPACING 4

Profiler profiler("Fixed number of vertices analysis");

enum Color
{
    WHITE,
    GREY,
    BLACK
};

typedef struct node
{
    int key, dist, parent;
    Color color;
    list<int> adjNodes;
} NodeT;

typedef struct graph
{
    int nrVertices, nrEdges;

    NodeT **nodeLists;
} GraphT;

void fatalError(const char *msg)
{
    fprintf(stderr, msg);
    exit(EXIT_FAILURE);
}

GraphT *createGraph(int nrVertices, int nrEdges)
{
    GraphT *newGraph = new GraphT;

    if (newGraph)
    {
        newGraph->nrVertices = nrVertices;
        newGraph->nrEdges = nrEdges;
        newGraph->nodeLists = new NodeT *[nrVertices];

        for (int i = 0; i < nrVertices; i++)
        {
            newGraph->nodeLists[i] = new NodeT;
        }
    }
    else
    {
        fatalError("Could not allocate graph!");
    }

    return newGraph;
}

void initializeGraph(GraphT *graph)
{
    for (int i = 0; i < graph->nrVertices; i++)
    {
        graph->nodeLists[i]->key = i;
        graph->nodeLists[i]->color = WHITE;
        graph->nodeLists[i]->dist = __INT_MAX__;
        graph->nodeLists[i]->parent = NULLP;
    }
}

void addEdge(GraphT *graph, int source, int destination)
{
    graph->nodeLists[source]->adjNodes.push_back(destination);
    graph->nodeLists[destination]->adjNodes.push_back(source);
}

GraphT *generateRandomGraph(int nrVertices, int nrEdges)
{
    GraphT *newGraph = createGraph(nrVertices, nrEdges);

    for (int i = 0; i < nrEdges; i++)
    {
        int randomSource = rand() % nrVertices, randomDestination = rand() % nrVertices;

    Test_Duplicate:
        if (newGraph->nodeLists[randomSource]->adjNodes.size() < newGraph->nodeLists[randomDestination]->adjNodes.size())
        {
            if (newGraph->nodeLists[randomSource]->adjNodes.empty() == false && find(newGraph->nodeLists[randomSource]->adjNodes.begin(),
                newGraph->nodeLists[randomSource]->adjNodes.end(), randomDestination) != newGraph->nodeLists[randomSource]->adjNodes.end())
            {
                randomSource = rand() % nrVertices;
                randomDestination = rand() % nrVertices;
                goto Test_Duplicate;
            }
        }
        else
        {
            if (newGraph->nodeLists[randomDestination]->adjNodes.empty() == false && find(newGraph->nodeLists[randomDestination]->adjNodes.begin(),
                newGraph->nodeLists[randomDestination]->adjNodes.end(), randomSource) != newGraph->nodeLists[randomDestination]->adjNodes.end())
            {
                randomSource = rand() % nrVertices;
                randomDestination = rand() % nrVertices;
                goto Test_Duplicate;
            }
        }

        addEdge(newGraph, randomSource, randomDestination);
    }

    return newGraph;
}

void BFS(GraphT *graph, int sourceNode, Operation *o)
{
    graph->nodeLists[sourceNode]->color = GREY;
    graph->nodeLists[sourceNode]->dist = 0;
    graph->nodeLists[sourceNode]->parent = NULLP;

    o->count(4);

    queue<int> queue;
    queue.push(sourceNode);

    while (!queue.empty())
    {
        int temp = queue.front();
        queue.pop();

        o->count(3);

        for (list<int>::iterator i = graph->nodeLists[temp]->adjNodes.begin(); i != graph->nodeLists[temp]->adjNodes.end(); i++)
        {
            if (graph->nodeLists[*i]->color == WHITE)
            {
                graph->nodeLists[*i]->color = GREY;
                graph->nodeLists[*i]->dist = graph->nodeLists[temp]->dist + 1;
                graph->nodeLists[*i]->parent = temp;

                queue.push(*i);

                o->count(4);
            }

            o->count(2);
        }

        graph->nodeLists[temp]->color = BLACK;
    }
}

// in case the graph is not connected
void unconnectedBFS(GraphT *graph, Operation *o)
{
    initializeGraph(graph);

    for (int i = 0; i < graph->nrVertices; i++)
    {
        if (graph->nodeLists[i]->color == WHITE)
            BFS(graph, i, o);
    }
}

void printGraph(GraphT *graph, bool *printed, int index)
{
    if (index == NULLP || printed[index])
        return;

    printGraph(graph, printed, graph->nodeLists[index]->parent);

    printed[index] = true;
    printf("%*s%d\n", graph->nodeLists[index]->dist * SPACING, "", index);
}

void prettyPrint(GraphT *graph)
{
    bool *printed = new bool[graph->nrVertices];
    memset(printed, false, sizeof(bool) * graph->nrVertices);

    for (int i = 0; i < graph->nrVertices; i++)
    {
        printGraph(graph, printed, i);
    }
}

void freeGraph(GraphT *graph) {
    for (int i = 0; i < graph->nrVertices; i++) {
        free(graph->nodeLists[i]);
    }

    free(graph->nodeLists);
    free(graph);
}

void averageCase()
{
    for (int nrEdges = 1000; nrEdges <= 5000; nrEdges += 100)
    {
        const int nrVertices = 100;
        Operation o = profiler.createOperation("nrOpsFixedVertices", nrEdges);

        GraphT *graph = generateRandomGraph(nrVertices, nrEdges);
        unconnectedBFS(graph, &o);
        freeGraph(graph);
    }

    profiler.createGroup("Fixed number of vertices analysis", "nrOpsFixedVertices");
    profiler.reset("Fixed number of edges analysis");

    for (int nrVertices = 100; nrVertices <= 200; nrVertices += 10)
    {
        const int nrEdges = 4500;
        Operation o = profiler.createOperation("nrOpsFixedEdges", nrVertices);

        GraphT *graph = generateRandomGraph(nrVertices, nrEdges);
        unconnectedBFS(graph, &o);
        freeGraph(graph);
    }

    profiler.createGroup("Fixed number of edges analysis", "nrOpsFixedEdges");
    profiler.showReport();
}

main(int argc, char const *argv[])
{
    srand(time(NULL));

    const int nrVertices = 10, nrEdges = 25;
    Operation o = profiler.createOperation("demo", nrVertices + nrEdges);

    GraphT *newGraph = generateRandomGraph(nrVertices, nrEdges);
    unconnectedBFS(newGraph, &o);
    prettyPrint(newGraph);
    freeGraph(newGraph);

    /* averageCase(); */

    return 0;
}
