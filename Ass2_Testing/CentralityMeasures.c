// Assignment 2 Task 3 - Centrality measures for social network analysis

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "CentralityMeasures.h"

static double AdjListDegree (AdjList l) {
    double counter = 0;
    for (AdjList curr = l; curr != NULL; curr = curr->next) {
        counter++;
    }
    return counter;
} 

NodeValues outDegreeCentrality(Graph g) {
    NodeValues outD;
    outD.noNodes = numVertices (g);
    outD.values = malloc (outD.noNodes * sizeof double);
    AdjList outConns = NULL;
    for (int i = 0; i < outD.noNodes; i++) {
        outConns = outIncident (g, i);
        outD.values[i] = AdjListDegree (outConns);
    }
    return outD;
}

NodeValues inDegreeCentrality(Graph) {
    NodeValues inD;
    inD.noNodes = numVertices (g);
    inD.values = malloc (inD.noNodes * sizeof double);
    AdjList inConns = NULL;
    for (int i = 0; i < inD.noNodes; i++) {
        inConns = inIncident (g, i);
        inD.values[i] = AdjListDegree (inConns);
    }
    return inD;
}

NodeValues degreeCentrality(Graph) { // for undirected graph
    NodeValues degrees;
    degrees.noNodes = numVertices (g);
    degrees.values = malloc (degrees.noNodes * sizeof double);
    AdjList outConns, inConns;
    outConns = inConns = NULL;
    for (int i = 0; i < degrees.noNodes; i++) {
        outConns = outIncident (g, i);
        inConns = inIncident (g, i);
        degrees.values[i] = AdjListDegree (outConns) + AdjListDegree (inConns);
    }
    return degrees;
}

NodeValues closenessCentrality(Graph) {}

NodeValues betweennessCentrality(Graph) {}
NodeValues betweennessCentralityNormalised(Graph) {}


void  showNodeValues(NodeValues) {}
void  freeNodeValues(NodeValues) {}
