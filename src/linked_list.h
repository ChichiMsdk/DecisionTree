#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef enum
{
	RIGHT,
	LEFT
}DIRECTION;

typedef struct Node
{
	float	*content;
	node	*right;
	node	*left;
	node	*parent;
}Node;

Node	*create_list(Node **node);
void	add_node(Node **node);
void	remove_node(Node **node);

#endif //LINKED_LIST_H
