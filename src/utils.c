#include "utils.h"
#include "csv.h"

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <string.h> // strcmp()
#include <math.h> // fmod()

extern data_set g_ds;

/*
 * Sets the whole dataset indexes to the array
 * along with the indexes of max and min value
 * and its array total size
 */
void
fill_head(Node **node, size_t size)
{
	int i = 0;
	(*node)->indexarray = malloc(sizeof(int) * size);
	(*node)->index_maxvalue = 0;
	(*node)->index_minvalue = 0;
	while (i < size)
	{
		if ((*node)->indexarray[i] >= (*node)->indexarray[(*node)->index_maxvalue])
			(*node)->index_maxvalue = i;
		if ((*node)->indexarray[i] <= (*node)->indexarray[(*node)->index_minvalue])
			(*node)->index_minvalue = i;
		(*node)->indexarray[i] = i;
		i++;
	}
	(*node)->array_size = i;
}

bool
is_unique(data_set ds, int *array, size_t size)
{
	size_t i = 0;
	int j = 0;
	while (i < size)
	{
		// type of the flower
		if (g_ds.fields[4].entries[array[0]] != g_ds.fields[4].entries[array[i]])
			return false;
		i++;
	}
	return true;
}

void
preorder_traversal(Node* node, const char *dir) 
{
	if (!node)
		return ;
    /* printf("%s: size: %zu\n", dir, node->array_size); */
	printf("\n---%s----\n", dir);
	for (int i = 0; i < node->array_size; i++)
	{
		printf("|%f|\t", g_ds.fields[4].entries[(node->indexarray[i])]);
	}
    preorder_traversal(node->left, "left");
    preorder_traversal(node->right, "right");
}

void
inorder_traversal(Node* node, const char *dir) 
{
	if (!node)
		return ;
    inorder_traversal(node->left, "left");
    printf("%s: size: %zu\n", dir, node->array_size);
    inorder_traversal(node->right, "right");
}

void
postorder_traversal(Node* node, const char *dir) 
{
	if (!node)
		return ;
    postorder_traversal(node->left, "left");
    postorder_traversal(node->right, "right");
    printf("%s: size: %zu\n", dir, node->array_size);
}

/*
 * Requires node->max_val and min to be set
 */
inline float
get_random_condition(data_set ds, int feature, Node *node)
{
	float f1 = ds.fields[feature].entries[node->index_minvalue];
	// %
	float f2 = (ds.fields[feature].entries[node->index_maxvalue] 
			- ds.fields[feature].entries[node->index_minvalue] + 1);

	int random_condition = fmodf(rand(), f2) + f1;

	return random_condition;
}

void
add_target_to_node(Node **node, data_set *ds, const char *field_name)
{
	int i = 0;
	while (i < 5)
	{
		if (strcmp(field_name, ds->fields[i].header) == 0)
		{
			printf("Found \"%s\" in the data_set\n", field_name);
			(*node)->content = ds->fields[i].entries;
			(*node)->content_size = ds->fields[i].nb_entries;
			return;
		}
		i++;
	}
	fprintf(stderr, "Could not find the target: \"%s\"\n", field_name);
}

void
array_min_max_index(int *array, size_t size, int *max, int *min)
{
	size_t i = 0;
	*max = array[0];
	*min = *max;
	while (i < size)
	{
		if (array[i] >= array[*max])
			*max = i;
		else if (array[i] <= array[*min])
			*min = i;
		i++;
	}
}
