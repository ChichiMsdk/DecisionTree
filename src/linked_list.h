#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef enum
{
	RIGHT = 0,
	LEFT = 1
}DIRECTION;

typedef struct Node Node;
typedef struct data_set data_set;

typedef struct List
{
	Node			*head;
	data_set		*ds;
}List;

typedef struct Node
{
	void			*content;
	size_t			content_size;
	float			max_val;
	float			min_val;
	Node			*right;
	Node			*left;
	List			*parent;
}Node;

void				create_list(Node **node);
void				add_node(Node **node, DIRECTION direction);
void				remove_node(Node **node);

#endif //LINKED_LIST_H
