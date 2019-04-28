// Assignment 2 Task 3 - Centrality measures for social network analysis
// Final Version

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "CentralityMeasures.h"
#include "Dijkstra.h"

static double AdjListDegree (AdjList l) {
    double counter = 0;
    for (AdjList curr = l; curr != NULL; curr = curr->next) {
        counter++;
    }
    return counter;
} 

static void freeAdjList (AdjList l) {
    AdjList next = NULL;
    while (l) {
        next = l->next;
        free (l);
        l = next;
    }
    return;
}

NodeValues outDegreeCentrality(Graph g) {
    NodeValues outD;
    outD.noNodes = numVerticies (g);
    outD.values = malloc (outD.noNodes * sizeof (double));
    AdjList outConns = NULL;
    for (int i = 0; i < outD.noNodes; i++) {
        outConns = outIncident (g, i);
        outD.values[i] = AdjListDegree (outConns);
    }
    return outD;
}

NodeValues inDegreeCentrality(Graph g) {
    NodeValues inD;
    inD.noNodes = numVerticies (g);
    inD.values = malloc (inD.noNodes * sizeof (double));
    AdjList inConns = NULL;
    for (int i = 0; i < inD.noNodes; i++) {
        inConns = inIncident (g, i);
        inD.values[i] = AdjListDegree (inConns);
        freeAdjList (inConns);
    }
    return inD;
}

NodeValues degreeCentrality(Graph g) { // for undirected graph
    NodeValues degrees;
    degrees.noNodes = numVerticies (g);
    degrees.values = malloc (degrees.noNodes * sizeof (double));
    AdjList outConns, inConns;
    outConns = inConns = NULL;
    for (int i = 0; i < degrees.noNodes; i++) {
        outConns = outIncident (g, i);
        inConns = inIncident (g, i);
        degrees.values[i] = AdjListDegree (outConns) + AdjListDegree (inConns);
        freeAdjList (inConns);
    }
    return degrees;
}

NodeValues closenessCentrality(Graph g) {
    NodeValues out;
    out.noNodes = numVerticies (g); // number of nodes in Graph
    double N = (double) out.noNodes; 
    out.values = malloc (out.noNodes * sizeof (double));
    
    double n = 0;  // number of reachable nodes from u (including u)
    double pathSum = 0;
    int dist = 0;
    for (Vertex u = 0; u < N; u++) {
        n = 0;
        pathSum = 0;
        ShortestPaths uPaths = dijkstra (g, u);
        for (Vertex v = 0; v < N; v++) {
            dist = uPaths.dist[v];
            if (dist > 0 || v == u) {
                n++;
                pathSum += dist;
            }
        }
        // Apply formula from spec -> out.values[u]
        if (n == 1) out.values[u] = 0;
        else out.values[u] = (n-1) / (N-1) * (n-1) / pathSum;
        freeShortestPaths (uPaths);
    }
    return out;
}

static int nShortestPaths (Graph g, Vertex src, Vertex dest) {
    if (src == dest) return 1;
    int nsp = 0; //number of shortest paths from s to t
    ShortestPaths sPaths = dijkstra (g, src);
    for (PredNode *currN = sPaths.pred[dest]; currN; currN = currN->next) {
        nsp += nShortestPaths (g, src, currN->v);
    }
    freeShortestPaths (sPaths);
    return nsp;
}

// Returns # of shortest paths from src to dest that pass through v
static int nPathsThrough (Graph g, Vertex src, Vertex dest, Vertex v) {
    if (dest == v) return nShortestPaths (g, src, dest);
    if (dest == src) return 0;
    ShortestPaths sPaths = dijkstra (g, src);
    int nPaths = 0;
    for (PredNode *curr = sPaths.pred[dest]; curr; curr = curr->next) {
        nPaths += nPathsThrough (g, src, curr->v, v);
    }
    freeShortestPaths (sPaths);
    return nPaths;
}

NodeValues betweennessCentrality (Graph g) {
    NodeValues out;
    out.noNodes = numVerticies (g);
    out.values = malloc (out.noNodes * sizeof (double));
    double numPaths = 0;
    double numPathsThrough = 0;
    for (Vertex s = 0; s < out.noNodes; s++) {
        for (Vertex t = 0; t < out.noNodes; t++) {
            numPaths = (double) nShortestPaths (g, s, t);
            for (Vertex v = 0; v < out.noNodes; v++) {
                if (v == s || v == t || numPaths == 0) continue;
                numPathsThrough = (double) nPathsThrough (g, s, t, v);
                out.values[v] += numPathsThrough / numPaths;
            }
        }
    }
    return out;
}

NodeValues betweennessCentralityNormalised (Graph g) {
    int n = numVerticies (g);
    NodeValues out;
    out.noNodes = n;
    out.values = malloc (out.noNodes * sizeof (double));
    NodeValues betweenness = betweennessCentrality (g); // un-normalised
    for (Vertex v = 0; v < n; v++) {
        if (n < 3)
            out.values[v] = 0;
        else
            out.values[v] = betweenness.values[v] / (double) ((n-1) * (n-2));
    }
    freeNodeValues (betweenness);
    return out;
}


void  showNodeValues(NodeValues n) {
    for (Vertex v = 0; v < n.noNodes; v++) {
        printf ("%d: %f\n", v, n.values[v]);
    }
    return;
}
void  freeNodeValues(NodeValues n) {
    free (n.values);
    return;
}
