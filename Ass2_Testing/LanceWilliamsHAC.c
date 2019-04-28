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
static void update_dist_single(double **dist, int left_cluster, int right_cluster,int size);
static void update_dist_complete(double **dist, int left_cluster, int right_cluster,int size);
static void free_dist_array(double **dist, int size);

//static void showDendrogram(Dendrogram d);


/* 
 * Finds Dendrogram using Lance-Williams algorithm (as discussed in the specs) 
   for the given graph g and the specified method for agglomerative clustering.
   Method vertex 1 represents 'Single linkage', and 2 represents 'Complete linkage'.
   For this assignment, you only need to implement the above two methods. 

   The function returns 'Dendrogram' structure (binary tree) with the required information.
 * 
 */


Dendrogram LanceWilliamsHAC(Graph g, int method) {
	assert(g != NULL);
	assert(method == 1 || method == 2);

	Dendrogram *cluster = create_clusters(g);
	double **dist = create_dist_array(g);
	
	//Iterative Loop which merges closest clusters together until only 1 remains
	for (int counter = 0; counter < numVerticies(g) - 1;counter++)  {
		int left_node;
		int right_node;

		find_min_dist(dist, &left_node, &right_node, numVerticies(g));
		merge_cluster(cluster,left_node,right_node);		
		
		//Single Link or Complete Link Method 
		if (method == 1) {
			update_dist_single(dist,left_node,right_node,numVerticies(g));
		}
		else {
			update_dist_complete(dist,left_node,right_node,numVerticies(g));
		}		
	}

    //The full tree is always located at cluster[0]
    Dendrogram tree = cluster[0];

    //Freeing data structures used in function
	free_dist_array(dist, numVerticies(g));
	free(cluster);
    
    return tree;
}


//The function creates the initial distance array 
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
				
				//Finding the correct edge if there exists one
				//This part assumes that there will be a max of 2 edges between
				//each vertex. 
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

                //Taking individual cases of distances 
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


//This function updates the distance array via removing all values that 
//correspond to the right cluster and updating all values that correspond to
//left cluster. 
static void update_dist_single(double **dist, int left_cluster, int right_cluster,int size) {
	int counter = 0;
	
	//Distances between clustered groups are cleared 
	dist[left_cluster][right_cluster] = -1;
	dist[right_cluster][left_cluster] = -1;
	
	
	while(counter < size) {
		//This part updates values that are connected to the left cluster 
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
			//Clearing values associating with right cluster
			dist[right_cluster][counter] = -1;
			dist[counter][right_cluster] = -1;
		}
		counter++;
	}
}


//This function is identical to update_dist_single except for a change in 
//the manner distances are updated
static void update_dist_complete(double **dist, int left_cluster, int right_cluster,int size) {
	int counter = 0;
	
	//Distances between clustered groups are cleared 
	dist[left_cluster][right_cluster] = -1;
	dist[right_cluster][left_cluster] = -1;
	
	while(counter <  size) {
		//This part updates values that are connected to the left cluster 
		if (dist[left_cluster][counter] != -1 && counter != right_cluster) {
			float path_left = dist[left_cluster][counter];
			float path_right = dist[right_cluster][counter];
			
			if (path_left > path_right) {
				dist[left_cluster][counter] = path_left;
				dist[counter][left_cluster] = path_left;
			}
			else {
				dist[left_cluster][counter] = path_right;
				dist[counter][left_cluster] = path_right;
			}
			//Clearing values associating with right cluster
			dist[right_cluster][counter] = -1;
			dist[counter][right_cluster] = -1;
		}
		counter++;
	}
}


//This function creates an array of Dendrograms where each element
//corresponds to a single-value cluster. 
static Dendrogram *create_clusters(Graph g) {
	Dendrogram *cluster = malloc(sizeof(Dendrogram) * numVerticies(g));
	if (cluster == NULL) {
		fprintf(stderr, "Not enough memory\n");
		exit(0);
	}

    //Filling each array with points to a Dendrogram
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


//This function finds the index values of the mininmum value in the distance
//array.
static void find_min_dist(double **dist, int *src, int *dest, int size) {
	float curr_min = INFINITY;
	
	//Standard For Loop through every element in index 
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			
			//Updates min if new value is smaller and it is not invalid
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

//Function that merges two Dendrograms together in the cluster array (The merge
//always occurs on the left-side so the merging is pushed towards the 0th index)
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
	cluster[right_node] = NULL;
}


//Standard recursive function to free a BST 
void freeDendrogram(Dendrogram d) {
	if (d == NULL) {
		return;
	}

	freeDendrogram(d->left);
	freeDendrogram(d->right);
	free(d);
	return;
}

/*
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
*/

static void free_dist_array(double **dist, int size) {
	for (int i = 0; i < size; i++) {
		free(dist[i]);
	}
	free(dist);
}
