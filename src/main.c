#include "utils.h"
#include "csv.h"

#include <assert.h>
#include <time.h>
#include <string.h> // strcmp()
#include <math.h> // fmod()

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

// todo: stop when one type left -> if feature[5] is unique
// todo: print(depth)
// todo: find a reliable way to track allocations
// todo: get median, average, etc..
// done: separate nodes
// done: random number

extern data_set g_ds;
data_set g_ds;

// stupid idea -> RAW(i) would automatically expand using global
// dangerous, stupid, impossible to debug but fkn cooooool B)

#define RAW(random_feature, indexarray, i) \
	(g_ds.fields[(random_feature)].entries[(indexarray)[(i)]])

void
print_tree(Node *node, int a)
{
	if (!node)
		return ;
	printf("%d: size: %zu\n", a, node->array_size);
	print_tree(node->left, 0);
	print_tree(node->right, 1);
}

const char *__asan_default_options() { return "detect_leaks=0"; }

int 
max_depth(Node* node)
{
	if (node == NULL)
		return 0;
	else 
	{
		/* compute the depth of each subtree */
		int lDepth = max_depth(node->left);
		int rDepth = max_depth(node->right);

		/* use the larger one */
		if (lDepth > rDepth)
			return (lDepth + 1);
		else
			return (rDepth + 1);
	}
}

void
append_node_array(Node **node_array, size_t *nb_elem, int max_elem, Node *node)
{
	assert(*nb_elem < max_elem && *nb_elem >= 0);
	node_array[(*nb_elem)] = node;
	(*nb_elem)++;
}

void
postorder_append(Node *node, NodeArray *narray, int depth)
{
	if (!node)
		return;
	postorder_append(node->right, narray, depth + 1);
	postorder_append(node->left, narray, depth + 1);
	append_node_array(narray[depth].array, &narray[depth].nb_elem, narray[depth].max_elem, node);
}

void
fill_level_array(NodeArray *narray, Node *node)
{
	int depth = max_depth(node);
	postorder_append(node, narray, 0);
}

NodeArray *
create_level_array(Node *node)
{
	int depth = max_depth(node);
	NodeArray *tree_array = malloc(sizeof(NodeArray) * depth);
	/* Node ***tree_array = malloc(sizeof(Node **) * depth); */
	int i = 0;
	while (i < depth)
	{
		tree_array[i].array = malloc(sizeof(Node*) * pow(2, i));
		tree_array[i].depth = depth;
		tree_array[i].max_elem = pow(2, i);
		tree_array[i].nb_elem = 0;
		i++;
	}
	return tree_array;
}

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
	printf("depth: %d\n", max_depth(node));
	NodeArray *narray = create_level_array(node);
	fill_level_array(narray, node);
	for (int i = 0; i < narray[0].depth; i++)
	{
		for(int j = 0; j < narray[i].nb_elem; j++)
		{
			Node *tmp = narray[i].array[j];
			printf("%zu\t", tmp->array_size);
		}
		printf("\n");
	}
    /*
	 * printf("postorder\n");
	 * postorder_traversal(node, "root");
	 * printf("---------------\n");
	 * printf("inorder\n");
	 * inorder_traversal(node, "root");
	 * printf("---------------\n");
	 * printf("preorder\n");
	 * preorder_traversal(node, "root");
     */

	// needs to free nodes
	destroy_dataset(g_ds);
	return 0;
}

void
separate_nodes(Node **node)
{
	static int depth = 0;
	depth++;
	/* printf("depth: %d\n", depth); */
	// condition -> more than one "type"
	/* if (depth >= 10 || (*node)->array_size <= 1) */
	if (depth >= 40 || (*node)->array_size <= 1 || (is_unique(g_ds, (*node)->indexarray, (*node)->array_size)) == true)
		return ;
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
	(*node)->right->prev = (*node);
	(*node)->left->prev = (*node);

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
	float random_condition = get_random_condition(g_ds, rfeat, c_node);

	size_t i = 0;
	size_t j = 0;

	int *r_indexarray = right_node->indexarray;
	const int *c_indexarray = c_node->indexarray;

    /*
	 * printf("random_conditon %f\n", random_condition);
	 * printf("random_feat %d\n", rfeat);
     */
	/* printf("cnode->array_size: %llu\n", c_node->array_size); */

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
	depth++;
	separate_nodes(&right_node);

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
	/* printf("lnode: %zu\t rnode: %llu\n", j, right_node->array_size); */
	depth++;
	separate_nodes(&left_node);
}
