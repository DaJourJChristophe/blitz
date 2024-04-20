#ifndef TREE_H
#define TREE_H

#include "node.h"

struct dom_tree
{
  char doctype[256];
  dom_tree_node_t *root;
};

typedef struct dom_tree dom_tree_t;

dom_tree_t *dom_tree_new(void);

void dom_tree_destroy(dom_tree_t *self);

void dom_tree_print(const dom_tree_t *self);

#endif/*TREE_H*/
