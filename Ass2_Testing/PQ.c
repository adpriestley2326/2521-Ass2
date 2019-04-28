#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "PQ.h"

//Data Structs are defined here
struct PQNodeRep {
	ItemPQ data;
	struct PQNodeRep *next;
}; 


typedef struct PQNodeRep *PQNode;

struct PQRep {
	PQNode head; 
	int nitems;
};

//Static helper functions are defined here
static PQNode create_new_PQnode(ItemPQ node);
static int deleteNode (PQ queue, int key);

//Create Empty PQ List Structure 
PQ newPQ() {
	PQ new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Not enough memory\n");
		exit(0);
	}

	new->head = NULL;
	new->nitems = 0;
	return new;
}

//Create a new PQ Node with value and key 
static PQNode create_new_PQnode(ItemPQ node) {
	PQNode new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Not enough memory\n");
		exit(0);
	}
	new->data = node;
	new->next = NULL;

	//printf("Created node of with key of %d\n",new->data.key);
	//printf("Create node with value of %d\n",new->data.value);
	return new;
}

//Insertion into PQ Node 
void addPQ(PQ queue, ItemPQ node) {
	assert(queue != NULL);
	
	PQNode new = create_new_PQnode(node);

	//Empty List
	if (queue->nitems == 0) {
		queue->head = new;
		queue->nitems++;
		return;
	}

	//If the item to be inserted has the highest priority
	if (new->data.value < queue->head->data.value) {
		new->next = queue->head;
		queue->head = new;
		queue->nitems++;
		return;
	}
	
	PQNode curr = queue->head->next;
	PQNode prev = queue->head;
	
	//Standard Iteration through list until value becomes less than current value
	while (curr != NULL) {
		if (new->data.value < curr->data.value) {
			prev->next = new;
			new->next = curr;
			queue->nitems++;
			return;
		}
		prev = curr;
		curr = curr->next;
	}
	//If value has the lowest priority 
	prev->next = new;
	queue->nitems++;
	return;
}

ItemPQ dequeuePQ(PQ queue) {
	assert(queue != NULL);
	
	//Cannot actually return NULL for a function that does not return a pointer 
	if (queue->nitems == 0) {
		fprintf(stderr, "Trying to dequeue an empty queue\n");
		exit(0);
	}

	PQNode temp = queue->head;
	ItemPQ result = queue->head->data;

	queue->head = queue->head->next;
	queue->nitems--;
	free(temp);

	return result;
}


void showPQ(PQ queue) {
	if (queue->nitems != 0) {
		int counter = 0;
		PQNode curr = queue->head;
		while (counter < queue->nitems) {
			printf("{%d,%d} -> ",curr->data.key,curr->data.value);
			counter++;
			curr = curr->next;
		}
		printf("\n");
	}
}

//Delete old node via key and inesrt new node in
void updatePQ(PQ queue, ItemPQ node) {
	int found = deleteNode(queue,node.key);
	if (found == 1) {
		addPQ(queue,node);
	} 

	return;
}

static int deleteNode (PQ queue, int key) {
	if (queue->nitems == 0) {
		return 0;
	}

	if (queue->head->data.key == key) {
		PQNode temp = queue->head;
		queue->head = queue->head->next;
		free(temp);
		queue->nitems--;
		return 1;
	}

	PQNode curr = queue->head->next;
	PQNode prev = queue->head;

	while (curr != NULL) {
		if (curr->data.key == key) {
			prev->next = curr->next;
			free(curr);
			queue->nitems--;
			return 1;
		}
		prev = curr;
		curr = curr->next;
	}
	return 0;
}

int PQEmpty(PQ queue){
	return (queue->nitems == 0);
}

void freePQ(PQ queue){
	int counter = 0;
	while (counter < queue->nitems) {
		PQNode temp = queue->head;
		queue->head = temp->next;
		free(temp);
		counter++;
	}
	free(queue);
}
