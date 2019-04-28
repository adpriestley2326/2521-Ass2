/*Lance-Williams Algorithm for Agglomerative Clustering
  Written by 
  COMP2521 2019 T1
*/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "LanceWilliamsHAC.h"
#include "Graph.h"
#include "Dijkstra.h"

#define numVertices numVerticies
#define INFINITY 999999


typedef struct index_rep *Index;

struct index_rep {
	int vertex;
	Index next;
};


static Dendrogram *create_clusters(Graph g);
static void merge_cluster (Dendrogram *cluster, int left_node, int right_node);

static double **create_dist_array(Graph g);
static void find_min_dist(double **dist, int *src, int *dest, int size);
static float update_dist(double **dist, int left_cluster, int right_cluster,Graph g);
static void free_dist_array(double **dist, int size);

static void showDendrogram(Dendrogram d);


/* 
 * Finds Dendrogram using Lance-Williams algorithm (as discussed in the specs) 
   for the given graph g and the specified method for agglomerative clustering.
   Method vertex 1 represents 'Single linkage', and 2 represents 'Complete linkage'.
   For this assignment, you only need to implement the above two methods. 

   The function returns 'Dendrogram' structure (binary tree) with the required information.
 * 
 */


Dendrogram LanceWilliamsHAC(Graph g, int method) {
	Dendrogram *cluster = create_clusters(g);
	double **dist = create_dist_array(g);
	
	for (int counter = 0; counter < numVerticies(g) - 1;counter++)  {
		int left_node;
		int right_node;

		find_min_dist(dist, &left_node, &right_node, numVerticies(g));
		merge_cluster(cluster,left_node,right_node);		
		update_dist(dist,left_node,right_node,g);
	}
/*
	for (int i = 0; i < numVerticies(g); i++) {
		showDendrogram(cluster[i]);
		printf("NULL\n");

	printf("Left node is %d\n",left_node);
	printf("Right node is %d\n",right_node);
	for (int i = 0; i < numVerticies(g); i++) {
		for (int j = 0; j < numVerticies(g); j++) {
			printf("%f  ",dist[i][j]);
		}
		printf("\n");
	}
	printf("\n\n");
	}	
*/
    Dendrogram tree = cluster[0];

	free_dist_array(dist, numVerticies(g));
	free(cluster);
    return tree;
}


double **create_dist_array(Graph g) {
	int num_vertices = numVerticies(g);

	double **dist = malloc(sizeof(double) * num_vertices);

	for (int row = 0; row < num_vertices; row++) {
		dist[row] = malloc(sizeof(double) * num_vertices);
	}
	

	for (int i = 0; i < num_vertices; i++) {
		AdjList out = outIncident(g,i);
		for (int j = 0; j < num_vertices; j++) {
			if (i == j) {
				dist[i][j] = -1;
			}
			else {
				AdjList in = outIncident(g,j);
				AdjList curr_i = out;
				AdjList curr_j = in;
				
				while (curr_i != NULL) {
					if (curr_i->w == j) {
						break;
					}
					curr_i = curr_i->next;
				}
				while (curr_j != NULL) {
					if (curr_j->w == i) {
						break;
					}
					curr_j = curr_j->next;
				}

				if (curr_i == NULL && curr_j == NULL) {
					dist[i][j] = INFINITY;
				}
				else if (curr_j == NULL) {
					dist[i][j] = 1/(double)curr_i->weight;
				}
				else if (curr_i == NULL) {
					dist[i][j] = 1/(double)curr_j->weight;
				}
				else if(curr_i->weight > curr_j->weight) {
					dist[i][j] = 1/(double)curr_i->weight;
				}
				else {
					dist[i][j] = 1/(double)curr_j->weight;
				}
			}
		}
	}
	return dist;
}

static float update_dist(double **dist, int left_cluster, int right_cluster,Graph g) {
	int counter = 0;
	while(counter <  numVerticies(g)) {
		if (dist[left_cluster][counter] != -1 && counter != right_cluster) {
			float path_left = dist[left_cluster][counter];
			float path_right = dist[right_cluster][counter];
			if (path_left < path_right) {
				dist[left_cluster][counter] = path_left;
				dist[counter][left_cluster] = path_left;
			}
			else {
				dist[left_cluster][counter] = path_right;
				dist[counter][left_cluster] = path_right;
			}
			dist[right_cluster][counter] = -1;
			dist[counter][right_cluster] = -1;
		}
		dist[left_cluster][right_cluster] = -1;
		dist[right_cluster][left_cluster] = -1;
		counter++;
	}


}

static Dendrogram *create_clusters(Graph g) {
	Dendrogram *cluster = malloc(sizeof(Dendrogram) * numVerticies(g));
	if (cluster == NULL) {
		fprintf(stderr, "Not enough memory\n");
		exit(0);
	}

	for (int counter = 0; counter < numVerticies(g); counter++) {
		Dendrogram new = malloc(sizeof(*new));
		if (new == NULL) {
			fprintf(stderr, "Not enough memory\n");
			exit(0);
		}

		new->vertex = counter;
		new->left = NULL;
		new->right = NULL;

		cluster[counter] = new;
	} 
	return cluster;
}



static void find_min_dist(double **dist, int *src, int *dest, int size) {
	float curr_min = INFINITY;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (dist[i][j] <= curr_min && dist[i][j] != -1) {
				//printf("Min detected at %d,%d\n",i,j);
				curr_min = dist[i][j];
				if (i < j) {
					*src = i;
					*dest = j;
				}
				else {
					*src = j;
					*dest = i;
				}
			} 
		}
	}
}


static void merge_cluster (Dendrogram *cluster, int left_node, int right_node) {
	Dendrogram new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Not enough memory\n");
		exit(0);
	}
	//printf("Merging %d and %d together\n",left_node,right_node);

	new->left = cluster[left_node];
	new->right = cluster[right_node];
	cluster[left_node] = new;
	//freeDendrogram(cluster[right_node]);
	cluster[right_node] = NULL;
}




void freeDendrogram(Dendrogram d) {
	if (d == NULL) {
		return;
	}

	freeDendrogram(d->left);
	freeDendrogram(d->right);
	free(d);
	return;
}

static void showDendrogram(Dendrogram d) {
	if (d == NULL) {
		return;
	}

	showDendrogram(d->left);
	showDendrogram(d->right);
	if (d->left == NULL && d->right == NULL) {
		printf("%d-> ",d->vertex);
	}
	return;
}

static void free_dist_array(double **dist, int size) {
	for (int i = 0; i < size; i++) {
		free(dist[i]);
	}
	free(dist);
}
