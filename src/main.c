#include "csv.h"
#include "linked_list.h"

#include <time.h>
#include <string.h> // strcmp()

#ifdef WIN_32
/*
 * This is to not import the whole stdlib.h on win32
 * OR NOT fckn microsoft -_-
 */
#define RAND_MAX 0x7fff

	_ACRTIMP void __cdecl srand(_In_ unsigned int _Seed);

	_Check_return_ _ACRTIMP int __cdecl rand(void);

	#if defined _CRT_RAND_S || defined _CRTBLD
		_ACRTIMP errno_t __cdecl rand_s(_Out_ unsigned int* _RandomValue);

#endif // _CRT_RAND_S || defined _CRTBLD

#endif // WIN_32


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

// todo: find a reliable way to track allocations
// todo: separate nodes
// todo: random number
// todo: get median, average, etc..

void separate_nodes(Node **node);

int
main(int ac, char *av[])
{
	if (ac != 2)
	{
		fprintf(stderr, "Usage: \"./tree <file.csv>\"\n");
		return 1;
	}

	srand(time(NULL));
	data_set ds = load_dataset_csv(av[1]);
	Node *node;
	create_list(&node);
	int index = 0;
	// remove that magic number "4"
	while (index < 4)
	{
		ds_get_min_max_valuef(&ds.fields[index].max_value,
				&ds.fields[index].min_value, ds.fields + index);

		printf("fields: %s\nmax: %f\tmin: %f\n", ds.fields[index].header, 
				ds.fields[index].max_value, ds.fields[index].min_value);
		index++;
	}

	// head node's content will point to target
	add_target_to_node(&node, &ds, "target");

	// needs to free nodes
	destroy_dataset(ds);
	return 0;
}



void
separate_nodes(Node **node)
{
	/*
	 * todo: check if we should separate them size > 1 -> needed for recursive way
	 *
	 *                       apparently rand() is harmful
	 * https://learn.microsoft.com/en-us/shows/goingnative-2013/rand-considered-harmful
	 */

	int	random_cond = 0;
	Node *right_node = (*node)->right;
	Node *left_node = (*node)->left;
	Node *current_node = (*node);
	const float *current_content = (float *)current_node->content;

	// add node to left and right
	add_node(node, RIGHT);
	add_node(node, LEFT);

	// always right_node first
	// allocate memory based on size of this node's content
	// to check the condition -> trading memory for cpu time
	// size is adjusted during loop
	right_node->content = malloc(sizeof(float) * current_node->content_size);

	random_cond = current_node->min_val +
		rand() % (int)(current_node->max_val - current_node->min_val + 1);

	int i = 0;
	int j = 0;
	float *right_content = right_node->content;
	while (i < current_node->content_size)
	{
		// right is >= && left is <
		if (current_content[i] >= random_cond)
		{
			if (j == 0)
			{
				right_node->max_val = current_content[i];
				right_node->min_val = current_content[i];
			}
			right_content[j] = current_content[i];
			if (right_content[j] > right_node->max_val)
				right_node->max_val = right_content[j];
			else if (right_content[j] < right_node->min_val)
				right_node->min_val = right_content[j];
			j++;
		}
		i++;
	}
	right_node->content_size = j;
	// should not be needed
	right_node->content = right_content;


	/*
	 * This could be optimized further by simply allocating the same amount
	 * of memory to the left_node, and then operate on both simultaneously
	 * and would prevent to loop two times the entire loop for each separation
	 * it would also reduce the size of this redundant code
	 */

	// this one's size is substraction of node's size - node_right's size
	left_node->content_size = current_node->content_size - right_node->content_size;
	left_node = malloc(sizeof(float) * left_node->content_size);

	// repeat lol
	i = 0;
	j = 0;
	float *left_content = left_node->content;
	while (i < current_node->content_size)
	{
		// left is >= && left is <
		if (current_content[i] >= random_cond)
		{
			if (j == 0)
			{
				left_node->max_val = current_content[i];
				left_node->min_val = current_content[i];
			}
			left_content[j] = current_content[i];
			if (left_content[j] > left_node->max_val)
				left_node->max_val = left_content[j];
			else if (left_content[j] < left_node->min_val)
				left_node->min_val = left_content[j];
			j++;
		}
		i++;
	}
	left_node->content_size = j;
	// should not be needed
	left_node->content = left_content;
}
