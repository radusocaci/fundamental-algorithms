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
#include <stack>
#include <ctime>
#include <algorithm>
#include "Profiler.h"

using namespace std;

#define NULLP -1
#define NOT_DISCOVERED_FINALIZED -2
#define SPACING 4

int globalTimer = 0;

Profiler profiler("Fixed number of vertices analysis");

enum Color
{
    WHITE,
    GREY,
    BLACK
};

typedef struct node
{
    int key, discoveryTime, finalizedTime, parent;
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
        graph->nodeLists[i]->discoveryTime = NOT_DISCOVERED_FINALIZED;
        graph->nodeLists[i]->finalizedTime = NOT_DISCOVERED_FINALIZED;
        graph->nodeLists[i]->parent = NULLP;
    }
}

void addEdge(GraphT *graph, int source, int destination)
{
    graph->nodeLists[source]->adjNodes.push_back(destination);
}

GraphT *generateRandomGraph(int nrVertices, int nrEdges)
{
    GraphT *newGraph = createGraph(nrVertices, nrEdges);

    for (int i = 0; i < nrEdges; i++)
    {
        int randomSource = rand() % nrVertices, randomDestination = rand() % nrVertices;
        while (randomDestination == randomSource)
            randomDestination = rand() % nrVertices;

        while (newGraph->nodeLists[randomSource]->adjNodes.empty() == false && find(newGraph->nodeLists[randomSource]->adjNodes.begin(),
               newGraph->nodeLists[randomSource]->adjNodes.end(), randomDestination) != newGraph->nodeLists[randomSource]->adjNodes.end())
        {
            randomSource = rand() % nrVertices;
            randomDestination = rand() % nrVertices;

            while (randomDestination == randomSource)
                randomDestination = rand() % nrVertices;
        }

        addEdge(newGraph, randomSource, randomDestination);
    }

    return newGraph;
}

void DFS(GraphT *graph, int sourceNode, Operation *o)
{
    graph->nodeLists[sourceNode]->color = GREY;
    graph->nodeLists[sourceNode]->discoveryTime = ++globalTimer;

    o->count(3);

    stack<int> stack;
    stack.push(sourceNode);
    while (!stack.empty())
    {
        int temp = stack.top();
        list<int>::iterator i;

        o->count(2);

        for (i = graph->nodeLists[temp]->adjNodes.begin(); i != graph->nodeLists[temp]->adjNodes.end(); i++)
        {
            o->count();

            if (graph->nodeLists[*i]->color == WHITE)
            {
                graph->nodeLists[*i]->color = GREY;
                graph->nodeLists[*i]->discoveryTime = ++globalTimer;
                graph->nodeLists[*i]->parent = temp;

                stack.push(*i);

                o->count(4);

                break;
            }
        }

        if (i == graph->nodeLists[temp]->adjNodes.end())
        {
            graph->nodeLists[temp]->color = BLACK;
            graph->nodeLists[temp]->finalizedTime = ++-----globalTimer;
            stack.pop();

            o->count(3);
        }

        o->count();
    }
}

// in case the graph is not connected
void unconnectedDFS(GraphT *graph, Operation *o)
{
    initializeGraph(graph);
    globalTimer = 0;

    for (int i = 0; i < graph->nrVertices; i++)
    {
        if (graph->nodeLists[i]->color == WHITE)
            DFS(graph, i, o);
    }
}

// timers are not needed
bool topologicalSortUtil(GraphT *graph, int sourceNode, stack<int> &result)
{
    graph->nodeLists[sourceNode]->color = GREY;
    graph->nodeLists[sourceNode]->discoveryTime = ++globalTimer;

    stack<int> stack;
    stack.push(sourceNode);
    while (!stack.empty())
    {
        int temp = stack.top();
        list<int>::iterator i;

        for (i = graph->nodeLists[temp]->adjNodes.begin(); i != graph->nodeLists[temp]->adjNodes.end(); i++)
        {
            if (graph->nodeLists[*i]->color == WHITE)
            {
                graph->nodeLists[*i]->color = GREY;
                graph->nodeLists[*i]->discoveryTime = ++globalTimer;
                graph->nodeLists[*i]->parent = temp;

                stack.push(*i);
                break;
            }
            // found a cycle => no topsort possible
            else if (graph->nodeLists[*i]->color == GREY)
            {
                return false;
            }
        }

        if (i == graph->nodeLists[temp]->adjNodes.end())
        {
            graph->nodeLists[temp]->color = BLACK;
            graph->nodeLists[temp]->finalizedTime = ++globalTimer;
            stack.pop();
            result.push(temp);
        }
    }

    return true;
}

// in case the graph is not connected
void topologicalSort(GraphT *graph)
{
    initializeGraph(graph);
    globalTimer = 0;
    stack<int> result;
    bool sortable = true;

    for (int i = 0; i < graph->nrVertices; i++)
    {
        if (graph->nodeLists[i]->color == WHITE)
            sortable = topologicalSortUtil(graph, i, result);

        if (!sortable)
        {
            cout << "The graph is not a DAG (contains cycles)"
                 << "\n";
            return;
        }
    }

    cout << "A topological sorting of the DAG: ";
    while (!result.empty())
    {
        cout << result.top() << " ";
        result.pop();
    }

    cout << "\n";
}

void tarjanSCCUtil(GraphT *graph, int sourceNode, stack<int> &result)
{
    // lowLink and discoveryTime are equal initially
    graph->nodeLists[sourceNode]->discoveryTime = graph->nodeLists[sourceNode]->finalizedTime = ++globalTimer;
    result.push(sourceNode);
    // the node is in the stack, marked accordingly
    graph->nodeLists[sourceNode]->color = GREY;

    for (list<int>::iterator i = graph->nodeLists[sourceNode]->adjNodes.begin(); i != graph->nodeLists[sourceNode]->adjNodes.end(); i++)
    {
        // tree edge
        if (graph->nodeLists[*i]->color == WHITE)
        {
            // do DFS
            tarjanSCCUtil(graph, *i, result);

            // check whether sourceNode has a child which can take us back,
            // with a back edge, to an ancestor of sourceNode
            graph->nodeLists[sourceNode]->finalizedTime = min(graph->nodeLists[sourceNode]->finalizedTime,
                                                              graph->nodeLists[*i]->finalizedTime);
        }
        // back edge (we do not deal with cross edges since we check for the children to always be in the stack)
        else if (graph->nodeLists[*i]->color == GREY)
        {
            // taking a minimum between lowLinks will not work,
            // because the ancestor's lowlink is not directly accesible from the descendant
            graph->nodeLists[sourceNode]->finalizedTime = min(graph->nodeLists[sourceNode]->finalizedTime,
                                                              graph->nodeLists[*i]->discoveryTime);
        }
    }

    // in the second part of the recurssion, we search for SCC roots. Roots have lowLink = discTime
    // When a root is found, pop all elements from the stack up to the root
    if (graph->nodeLists[sourceNode]->discoveryTime == graph->nodeLists[sourceNode]->finalizedTime)
    {
        int top = result.top();
        cout << "{";

        // pop all elements from the stack up to the root (they form a SCC)
        while (top != sourceNode)
        {
            cout << top << " ";
            result.pop();
            // the node has been removed from the stack => processing it now results in cross edges
            graph->nodeLists[top]->color = BLACK;
            top = result.top();
        }

        // take the root as well (sourceNode)
        cout << sourceNode << "}, ";
        result.pop();
        // the node has been removed from the stack => processing it now results in cross edges
        graph->nodeLists[sourceNode]->color = BLACK;
    }
}

/**
 * finilizingTime used as lowLink
 * recursive implementation => pass stack as argument to function
 * we use color grey for nodes in the stack and black for nodes popped from the stack
 */
void tarjanSCC(GraphT *graph)
{
    initializeGraph(graph);
    globalTimer = 0;
    stack<int> stack;

    cout << "The strongly connected components are: ";

    for (int i = 0; i < graph->nrVertices; i++)
    {
        if (graph->nodeLists[i]->color == WHITE)
            tarjanSCCUtil(graph, i, stack);
    }

    cout << "\n";
}

void freeGraph(GraphT *graph)
{
    for (int i = 0; i < graph->nrVertices; i++)
    {
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
        unconnectedDFS(graph, &o);
        freeGraph(graph);
    }

    profiler.createGroup("Fixed number of vertices analysis", "nrOpsFixedVertices");
    profiler.reset("Fixed number of edges analysis");

    for (int nrVertices = 100; nrVertices <= 200; nrVertices += 10)
    {
        const int nrEdges = 9000;
        Operation o = profiler.createOperation("nrOpsFixedEdges", nrVertices);

        GraphT *graph = generateRandomGraph(nrVertices, nrEdges);
        unconnectedDFS(graph, &o);
        freeGraph(graph);
    }

    profiler.createGroup("Fixed number of edges analysis", "nrOpsFixedEdges");
    profiler.showReport();
}

void printInitialGraph(GraphT *graph)
{
    for (int i = 0; i < graph->nrVertices; i++)
    {
        cout << graph->nodeLists[i]->key << ": ";

        for (list<int>::iterator j = graph->nodeLists[i]->adjNodes.begin(); j != graph->nodeLists[i]->adjNodes.end(); j++)
        {
            cout << *j << " ";
        }

        cout << "\n";
    }
}

main(int argc, char const *argv[])
{
    srand(time(NULL));

    const int nrVertices = 10, nrEdges = 10;
    Operation o = profiler.createOperation("demo", nrVertices + nrEdges);

    GraphT *newGraph = generateRandomGraph(nrVertices, nrEdges);
    initializeGraph(newGraph);
    printInitialGraph(newGraph);
    tarjanSCC(newGraph);
    topologicalSort(newGraph);

    freeGraph(newGraph);

    /* averageCase(); */

    return 0;
}