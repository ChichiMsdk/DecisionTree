#include "linked_list.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void
add_node(Node **node, DIRECTION direction)
{
	assert(direction == RIGHT || direction == LEFT);
	Node *new_node = malloc(sizeof(Node));
	new_node->content = NULL;
	new_node->right = NULL;
	new_node->left = NULL;
	new_node->parent = NULL;

	if (direction == RIGHT)
	{
		printf("%d: %pu\n", RIGHT, (*node)->right = new_node);
	}
	else
	{
		printf("%d: %pu\n", LEFT, (*node)->left = new_node);
	}
}

void
create_list(Node **node)
{
	*node = malloc(sizeof(Node));
	(*node)->content = NULL;
	(*node)->right = NULL;
	(*node)->left = NULL;
	(*node)->parent = NULL;
}
