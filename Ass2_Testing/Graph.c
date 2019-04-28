// Assignment 2 Task 1 - Graph ADT
// Last modified 11/04/19
// Final Version

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "Graph.h"

typedef struct GraphRep {
    int nV;
    int nE;
    AdjList *edges;
} GraphRep;

static bool validV (Graph g, Vertex v) {
    return (g != NULL && v >= 0 && v < g->nV);
}

static AdjList newAdjNode (Vertex v, int weight) {
    assert (v >= 0);
    AdjList new = malloc (sizeof *new);
    new->w = v;
    new->weight = weight;
    new->next = NULL;
    return new;
}

Graph newGraph (int noNodes) {
    assert (noNodes >= 0);
    Graph new = malloc (sizeof *new);
    new->nV = noNodes;
    new->nE = 0;
    new->edges = malloc (new->nV * sizeof (AdjList));
    for (int i = 0; i < new->nV; i++)
        new->edges[i] = NULL;
    return new;
}

// Inserts an edge into the relevant adjacency list
void  insertEdge (Graph g, Vertex src, Vertex dest, int weight) {
    assert (validV (g, src) && validV (g, dest));
    
    AdjList new = newAdjNode (dest, weight);
    if (g->edges[src] == NULL) // Currently no edges from src
        g->edges[src] = new;
    else {
        AdjList curr = g->edges[src];
        while (curr->next != NULL) {
            if (curr->w == dest) { //Node already in graph
                free (new);
                return;
            }
            curr = curr->next;
        }
        curr->next = new;
    }
    g->nE++;
    return;
}

void  removeEdge (Graph g, Vertex src, Vertex dest) {
    assert (validV (g, src) && validV (g, dest));
    AdjList curr = g->edges[src];
    AdjList prev = NULL;
    while (curr != NULL) {
        if (curr->w == dest) {
            if (prev) prev->next = curr->next; // Not first node
            else g->edges[src] = curr->next; // First node
            free (curr);
            g->nE--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    return;
}

bool  adjacent (Graph g, Vertex src, Vertex dest) {
    assert (validV (g, src) && validV (g, dest));
    AdjList curr = g->edges[src];
    while (curr) {
        if (curr->w == dest)
            return true;
        curr = curr->next;
    }
    return false;
}

int numVerticies (Graph g) {
    assert (g != NULL);
    return g->nV;
}

AdjList outIncident (Graph g, Vertex v) {
    assert (validV (g, v));
    return g->edges[v];
}

// Note - the returned INcident needs to be freed when inIncident() is used
AdjList inIncident (Graph g, Vertex v) {
    assert (validV (g, v));
    AdjList INcident, currG, currI;
    INcident = currG = currI = NULL;
    for (int i = 0; i < g->nV; i++) {
        if (i == v) continue;
        currG = g->edges[i]; // Current linked list in graph
        while (currG) {
            if (currG->w == v) { // Then i is connected to v by currG
                AdjList newNode = newAdjNode (i, currG->weight);
                if (INcident == NULL) // INcident empty
                    INcident = newNode;
                else { // Add newNode to end of INcident, using currI;
                    currI = INcident;
                    while (currI->next != NULL)
                        currI = currI->next;
                    currI->next = newNode;
                }
            }
            currG = currG->next;
        }
    }
    return INcident;
}

void  showGraph (Graph g) {
    assert (g != NULL);
    printf ("Showing graph...\n\n");
    AdjList curr = NULL;
    for (int v = 0; v < g->nV; v++) {
        printf ("Vertex %d connected to vertices:\n\t", v);
        curr = g->edges[v];
        while ((curr = curr->next))
            printf (" %d", curr->w);
        printf (".\n\n");
    }
    return;
}

void  freeGraph (Graph g) {
    assert (g != NULL);

    AdjList curr = NULL;
    AdjList temp;
    for (int v = 0; v < g->nV; v++) {
        curr = g->edges[v];
        while (curr != NULL) {
            temp = curr;
            curr = curr->next;
            free (temp);
        }
    }
    free (g->edges);
    free (g);
    return;
}
            
