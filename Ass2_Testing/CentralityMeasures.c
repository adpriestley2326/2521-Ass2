// Assignment 2 Task 3 - Centrality measures for social network analysis

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

NodeValues outDegreeCentrality(Graph g) {
    printf ("Finding outDegree Centrality\n");
    NodeValues outD;
    outD.noNodes = numVerticies (g);
    //printf ("n vertices in graph: %d\n", outD.noNodes);
    outD.values = malloc (outD.noNodes * sizeof (double));
    AdjList outConns = NULL;
    for (int i = 0; i < outD.noNodes; i++) {
        outConns = outIncident (g, i);
        outD.values[i] = AdjListDegree (outConns);
    }
    return outD;
}

NodeValues inDegreeCentrality(Graph g) {
    printf ("Finding inDegree Centrality\n");
    NodeValues inD;
    inD.noNodes = numVerticies (g);
    printf ("n vertices in graph: %d\n", inD.noNodes);
    inD.values = malloc (inD.noNodes * sizeof (double));
    AdjList inConns = NULL;
    //printf ("Centrality found for nodes: ");
    for (int i = 0; i < inD.noNodes; i++) {
        inConns = inIncident (g, i);
        inD.values[i] = AdjListDegree (inConns);
        //printf ("%d ", i);
    }
    //printf ("InDegree Centrality found\n");
    return inD;
}

NodeValues degreeCentrality(Graph g) { // for undirected graph
    printf ("Finding degree Centrality\n");
    NodeValues degrees;
    degrees.noNodes = numVerticies (g);
    //printf ("n vertices in graph: %d\n", degrees.noNodes);
    degrees.values = malloc (degrees.noNodes * sizeof (double));
    AdjList outConns, inConns;
    outConns = inConns = NULL;
    for (int i = 0; i < degrees.noNodes; i++) {
        outConns = outIncident (g, i);
        inConns = inIncident (g, i);
        degrees.values[i] = AdjListDegree (outConns) + AdjListDegree (inConns);
    }
    return degrees;
}

NodeValues closenessCentrality(Graph g) {
    //printf ("Finding closeness Centrality\n");
    NodeValues out;
    out.noNodes = numVerticies (g); // number of nodes in Graph
    double N = (double) out.noNodes; 
    //printf ("n vertices in graph: %d\n", out.noNodes);
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
        out.values[u] = (n-1) / (N-1) * (n-1) / pathSum;
    }
    return out;
}

static int nShortestPaths (Graph g, Vertex s, Vertex t) {
    if (s == t) return 1;
    int nsp = 0; //number of shortest paths from s to t
    ShortestPaths sPaths = dijkstra (g, s);
    for (PredNode *currN = sPaths.pred[t]; currN; currN = currN->next) {
        nsp += nShortestPaths (g, s, currN->v);
    }
    return nsp;
}

static int nPathsThrough (Graph g, Vertex s, Vertex t, Vertex v) {
    if (s == v || v == t) return 0;
    // # of paths through v = #paths to v * #paths from v
    return nShortestPaths (g, s, v) * nShortestPaths (g, v, t);
}

NodeValues betweennessCentrality (Graph g) {
    //printf ("Finding betweenness centrality\n");
    NodeValues out;
    out.noNodes = numVerticies (g);
    out.values = malloc (out.noNodes * sizeof (double));
    double numPaths = 0;
    double numPathsThrough = 0;
    for (Vertex s = 0; s < out.noNodes; s++) {
        for (Vertex t = 0; t < out.noNodes; t++) {
            numPaths = (double) nShortestPaths (g, s, t);
            for (Vertex v = 0; v < out.noNodes; v++) {
                if (v == s || v == t) continue;
                numPathsThrough = (double) nPathsThrough (g, s, t, v);
                out.values[v] += numPathsThrough / numPaths;
            }
        }
    }
    return out;
}

NodeValues betweennessCentralityNormalised (Graph g) {
    //printf ("Finding normalised betweenness Centrality\n");
    int n = numVerticies (g);
    NodeValues out;
    out.noNodes = n;
    //printf ("n vertices in graph: %d\n", out.noNodes);
    out.values = malloc (out.noNodes * sizeof (double));
    NodeValues betweenness = betweennessCentrality (g);
    for (Vertex v = 0; v < n; v++) {
        out.values[v] = betweenness.values[v] / (double) ((n-1) * (n-2));
    }
    freeNodeValues (betweenness);
    return out;
}


void  showNodeValues(NodeValues n) {
    //printf ("Displaying NodeValues for %d nodes...\n", n.noNodes);
    /*for (Vertex v = 0; v < n.noNodes; v++) {
        printf ("%d: %f\n", v, n.values[v]);
    }
    printf ("NodeValues displayed successfully\n");*/
    return;
}
void  freeNodeValues(NodeValues n) {
    printf ("Freeing node values...\n");
    for (Vertex v = 0; v < n.noNodes; v++) {
        printf ("%d: %f\n", v, n.values[v]);
    }
    free (n.values);
    return;
}
