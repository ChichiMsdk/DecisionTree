#ifndef UTILS_H
#define UTILS_H

#include "linked_list.h"

#include <stdbool.h>

void fill_head(Node **node, size_t size);
void array_min_max_index(int *array, size_t size, int *max, int *min);
void add_target_to_node(Node **node, data_set *ds, const char *field_name);
void separate_nodes(Node **node);
float get_random_condition(data_set ds, int feature, Node *node);
bool is_unique(data_set ds, int *array, size_t size);

void preorder_traversal(Node* node, const char *dir);
void inorder_traversal(Node* node, const char *dir);
void postorder_traversal(Node* node, const char *dir);

#endif
