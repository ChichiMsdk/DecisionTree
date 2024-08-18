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

// todo: find a reliable way to track allocations
// todo: separate nodes
// todo: random number
// todo: get median, average, etc..

extern data_set g_ds;
data_set g_ds;

// stupid idea -> RAW(i) would automatically expand using global
// dangerous, stupid, impossible to debug but fkn cooooool B)

#define RAW(random_feature, indexarray, i) \
	(g_ds.fields[(random_feature)].entries[(indexarray)[(i)]])


void fill_head(Node **node, size_t size);
void array_min_max_index(int *array, size_t size, int *max, int *min);
void add_target_to_node(Node **node, data_set *ds, const char *field_name);
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
	g_ds = load_dataset_csv(av[1]);
	Node *node;
	// useless lol
	create_list(&node);
	int index = 0;
	// remove that magic number "4"
	while (index < 4)
	{
		ds_get_min_max_valuef(&g_ds.fields[index].max_value,
				&g_ds.fields[index].min_value, g_ds.fields + index);

		printf("fields: %s\nmax: %f\tmin: %f\n", g_ds.fields[index].header, 
				g_ds.fields[index].max_value, g_ds.fields[index].min_value);
		index++;
	}

	// head node's content will point to target
	/* add_target_to_node(&node, &g_ds, "target"); */
	fill_head(&node, g_ds.lines);
	separate_nodes(&node);

	// needs to free nodes
	destroy_dataset(g_ds);
	return 0;
}

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

inline int get_random_condition(data_set ds, int feature, Node *node);

void
separate_nodes(Node **node)
{
	/*
	 * todo: check if we should separate them size > 1 -> needed for recursive way
	 *
	 *                       apparently rand() is harmful
	 * https://learn.microsoft.com/en-us/shows/goingnative-2013/rand-considered-harmful
	 */

	/*
	 * putting add_node(node, RIGHT) BEFORE the left one is BUGGED :DDDDDDDD
	 * Seems like (*node)->right takes the value of left and leaves left with NULL
	 * Seemingly because of a previous buffer overflow done later 50 lines below :D
	 * Obviously fsanitize only reports the NULL dereference but not the REAL bug
	 * which is a buffer overflow happening L A T E R
	 * I love C :)
	 */
	add_node(node, RIGHT);
	add_node(node, LEFT);
	Node *right_node = (*node)->right;
	Node *left_node = (*node)->left;
	Node *c_node = (*node);

	// always right_node first
	// allocate memory based on size of this node's content
	// to check the condition -> trading memory for cpu time
	// size is adjusted during loop
	right_node->indexarray = malloc(sizeof(int) * c_node->array_size);

	// hard coded values berk
	int rfeat = 0 + rand() % (4 - 0 + 1);
    /*
	 * // this should only happen once (?)
	 * add_target_to_node(node, &g_ds, g_ds.fields[rfeat].header);
	 * const float *current_content = (float *)c_node->content;
	 * ds_get_min_max_valuef(&c_node->max_val, &c_node->min_val, &g_ds.fields[rfeat]);
     */

	// need to fmodulo type of function since float % float is not possible #shittyC
	// so the random_condition is an int
	int random_condition = get_random_condition(g_ds, rfeat, c_node);

	size_t i = 0;
	size_t j = 0;

	int *r_indexarray = right_node->indexarray;
	const int *c_indexarray = c_node->indexarray;

	printf("random_conditon %d\n", random_condition);
	printf("random_feat %d\n", rfeat);
	printf("cnode->array_size: %llu\n", c_node->array_size);

	// right node >= condition && left node <= condition
	while (i < c_node->array_size)
	{
		// disgusting, but working
		if (RAW(rfeat, c_indexarray, i) >= random_condition)
		{
			if (j == 0)
			{
				right_node->max_val = RAW(rfeat, c_indexarray, i);
				right_node->min_val = RAW(rfeat, c_indexarray, i);
				right_node->index_maxvalue = c_indexarray[i];
				right_node->index_minvalue = c_indexarray[i];
			}
			r_indexarray[j] = c_indexarray[i];

			if (RAW(rfeat, r_indexarray, j) >= 
					g_ds.fields[(rfeat)].entries[(right_node->index_maxvalue)])
			{
				right_node->index_maxvalue = r_indexarray[j];
			}
			else if (RAW(rfeat, r_indexarray, j) <
					g_ds.fields[(rfeat)].entries[(right_node->index_minvalue)])
			{
				right_node->index_minvalue = r_indexarray[j];
			}
			j++;
		}
		i++;
	}
	right_node->content_size = j;
	right_node->array_size = j;
	// should not be needed
	right_node->indexarray = r_indexarray;

	/*
	 * This could be optimized further by simply allocating the same amount
	 * of memory to the left_node, and then operate on both simultaneously
	 * and would prevent to loop two times the entire array for each separation
	 * it would also reduce the size of this redundant code -> leading to bugs!!
	 */

	// this one's size is substraction of node's size - node_right's size
	left_node->array_size = c_node->array_size - right_node->array_size;
	left_node->indexarray = malloc(sizeof(int) * left_node->array_size);
	int *l_indexarray = left_node->indexarray;

	// repeat lol
	i = 0;
	j = 0;
	float *left_content = left_node->content;
	while (i < c_node->array_size && j < left_node->array_size)
	{
		// left is >= && left is <
		if (RAW(rfeat, c_indexarray, i) < random_condition)
		{
			if (j == 0)
			{
				left_node->max_val = RAW(rfeat, c_indexarray, i);
				left_node->min_val = RAW(rfeat, c_indexarray, i);
				left_node->index_maxvalue = c_indexarray[i];
				left_node->index_minvalue = c_indexarray[i];
			}
			l_indexarray[j] = c_indexarray[i];

			if (RAW(rfeat, l_indexarray, j) >=
					g_ds.fields[(rfeat)].entries[(left_node->index_maxvalue)])
			{
				left_node->index_maxvalue = l_indexarray[j];
			}
			else if (RAW(rfeat, l_indexarray, j) <
					g_ds.fields[(rfeat)].entries[(left_node->index_minvalue)])
			{
				left_node->index_minvalue = l_indexarray[j];
			}
			j++;
		}
		i++;
	}
	left_node->array_size = j;
	// should not be needed
	left_node->indexarray = l_indexarray;
	printf("lnode: %zu\t rnode: %llu\n", j, right_node->array_size);
}

/*
 * Requires node->max_val and min to be set
 */
inline int
get_random_condition(data_set ds, int feature, Node *node)
{
	int random_condition = ds.fields[feature].entries[node->index_minvalue] +
		rand() % (int)
		(ds.fields[feature].entries[node->index_minvalue] 
		 - ds.fields[feature].entries[node->index_minvalue] + 1);

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
