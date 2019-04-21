// Dijkstra ADT interface for Ass2 (COMP2521)
#include "Dijkstra.h"
#include "PQ.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define INFINITY 999999

typedef PredNode *PredNode_pointer;

static ItemPQ newItemPQ(int a, int b);
static ShortestPaths init_shortest_path(Graph g,Vertex v);
static void relax(Vertex alt, Vertex dest, int weight, ShortestPaths data, PQ queue);
//static bool is_existing_pred(ShortestPaths data, Vertex v, Vertex alt);
//static void insert_in_order(PredNode *new, ShortestPaths data, Vertex dest);


static ItemPQ newItemPQ(int a, int b){
  ItemPQ *p = malloc(sizeof(struct ItemPQ)) ;
  p->key = a;
  p->value = b;
  
  //free(p);
  return *p;
}


//Preliminary Functions to create Shortest Path Data Structure
static ShortestPaths init_shortest_path(Graph g,Vertex v) {
	assert(g != NULL);
	int num_vertices = numVerticies(g);
	
	int *dist = malloc(sizeof(int) * num_vertices);
	for (int counter = 0;counter < num_vertices;counter++) {
		dist[counter] = INFINITY;
	}
	dist[v] = 0;

	PredNode **pred = malloc(sizeof(PredNode_pointer) * num_vertices);
	for (int counter = 0; counter < num_vertices;counter++) {
		pred[counter] = NULL;
	}
	
	ShortestPaths data;
	data.noNodes = num_vertices;
	data.src = v;
	data.dist = dist;
	data.pred = pred;

	return data;
}

ShortestPaths dijkstra(Graph g, Vertex v) {
	int num_vertices = numVerticies(g);
	
	ShortestPaths data = init_shortest_path(g,v);
	data.dist[v] = 0;
	
	//Creating new priority queue and appending inital values onto it 
	PQ queue = newPQ();
	for (int counter = 0; counter < num_vertices;counter++) {
		addPQ(queue,newItemPQ(counter,INFINITY));
	}
	updatePQ(queue,newItemPQ(v,0));
	
	//Main loop for dijkstra's algorithm
	while (!PQEmpty(queue)) {
		//Find vertex with min distance to source which has not been seen already
		ItemPQ next_node = dequeuePQ(queue);

		//For each adjacent vertex, perform relaxation
		AdjList adj_nodes = outIncident(g,next_node.key);
		AdjList dest = adj_nodes;
		while (dest != NULL) {
			relax(next_node.key,dest->w,dest->weight,data,queue);	
			dest = dest->next;
		}
		//free(adj_nodes);
	}


	for (int counter = 0; counter < num_vertices; counter++) {
		if (data.dist[counter] == INFINITY) {
			data.dist[counter] = 0;
		}
	}
	
	freePQ(queue);
	return data;
}

static void relax(Vertex alt, Vertex dest, int weight, ShortestPaths data, PQ queue) {
	//printf("Distance to dest via alt is %d + %d\n",data.dist[alt],weight);
	//printf("Direct distance to dest is %d\n",data.dist[dest]);
	PredNode *curr = data.pred[dest];
	if(data.dist[alt] + weight < data.dist[dest]) {
		PredNode *new_node = malloc(sizeof(*new_node));
		new_node->v= alt;
		new_node->next = NULL; 

		//The pred array can have mulitple values and thus every existing value
		//must be overwritten
		while (curr != NULL) {
			PredNode *temp = curr;
			curr = curr->next;
			free(temp);
		}
		data.pred[dest] = new_node;
		data.dist[dest] = data.dist[alt] + weight;
		updatePQ(queue,newItemPQ(dest,data.dist[alt] + weight));
	}
	
	//Case where there is an alternative path of the same minimal length to a node 
	//Note that there is an additional condition which stops multiple instances
	//of the same predecessor node
	else if (data.dist[dest] == data.dist[alt]) {
		
		PredNode *new_node = malloc(sizeof(*new_node));
		new_node->v= alt;
		new_node->next = NULL; 

		//printf("Appending pred onto list\n");
		//insert_in_order(new_node,data,dest);
		while(curr->next != NULL) {
			curr = curr->next;
		}
		curr->next = new_node;
	}
	return;
}


/*This function inserts nodes in the pred list based on descending? order
static void insert_in_order(PredNode *new, ShortestPaths data, Vertex dest) {
	PredNode *curr = data.pred[dest];

	if (new->v > curr->v) {
		PredNode *temp = curr;
		data.pred[dest] = new;
		new->next = temp;
		return;
	} 
	
	PredNode *prev = curr;
	curr =curr->next;
	while (curr!= NULL) {
		if (new->v < curr->v) {
			prev->next = new;
			new->next = curr;
			return;
		}
		prev = curr;
		curr = curr->next;
	}
	prev->next = new;
	return;
}
*/

//Function to check if the pred node already exists
/*
static bool is_existing_pred(ShortestPaths data, Vertex v, Vertex alt){
	PredNode *curr = data.pred[v];
	while(curr != NULL) {
		if (curr->v == alt) {
			return true;
		}
		curr = curr->next;
	}
	return false;
}
*/


//Copied from the Test Example(Does it well enough)
void showShortestPaths(ShortestPaths paths){
	int i = 0;
	printf("Node %d\n",paths.src);
	printf("  Distance\n");
	for (i = 0; i < paths.noNodes; i++) {
			if(i == paths.src)
	    	printf("    %d : X\n",i);
			else
				printf("    %d : %d\n",i,paths.dist[i]);
	}
	printf("  Preds\n");
	for (i = 0; i < paths.noNodes; i++) {
		printf("    %d : ",i);
		PredNode* curr = paths.pred[i];
		while(curr!=NULL) {
			printf("[%d]->",curr->v);
			curr = curr->next;
		}
		printf("NULL\n");
	}
	
	return;
}


void freeShortestPaths(ShortestPaths paths) {
	//printf("Freeing distance array\n");
	free(paths.dist);
	//printf("Freed distance array\n");

	for (int counter = 0; counter < paths.noNodes; counter++) {
		PredNode *curr = paths.pred[counter];
		while(curr != NULL) {
			PredNode *temp = curr;
			curr = curr->next;
			free(temp);
		}
	} 
	//printf("Freed pred blocks\n");
	
	free(paths.pred);
	return;
}
