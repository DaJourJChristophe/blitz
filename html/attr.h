#ifndef ATTR_H
#define ATTR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct dom_tree_node_attr
{
  char name[32];
  char value[32];
};

typedef struct dom_tree_node_attr dom_tree_node_attr_t;

dom_tree_node_attr_t *dom_tree_node_attr_new(const char *name, const char *value);

void dom_tree_node_attr_destroy(dom_tree_node_attr_t *self);

struct dom_tree_node_attr_stack
{
  size_t cap;
  uint64_t top;
  dom_tree_node_attr_t *attrs[];
};

typedef struct dom_tree_node_attr_stack dom_tree_node_attr_stack_t;

dom_tree_node_attr_stack_t *dom_tree_node_attr_stack_new(const size_t cap);

void dom_tree_node_attr_stack_destroy(dom_tree_node_attr_stack_t *self);

bool dom_tree_node_attr_stack_push(dom_tree_node_attr_stack_t *self, dom_tree_node_attr_t *attr);

dom_tree_node_attr_t *dom_tree_node_attr_stack_peek(dom_tree_node_attr_stack_t *self);

dom_tree_node_attr_t *dom_tree_node_attr_stack_pop(dom_tree_node_attr_stack_t *self);

#endif/*ATTR_H*/
