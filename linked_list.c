#include "linked_list.h"

void
add_node(Node **node, DIRECTION direction)
{
	Node *node = malloc(sizeof(Node));
	node->content = NULL;
	node->right = NULL;
	node->left = NULL;
	node->parent = NULL;

	if (direction == RIGHT)
		*node->right = node;
	else
		*node->left = node;
}

Node *
create_list(void)
{
	Node *node = malloc(sizeof(Node));
	node->content = NULL;
	node->right = NULL;
	node->left = NULL;
	node->parent = NULL;
}
