#ifndef NODE_H
#define NODE_H

#include "attr.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct dom_tree_node
{
  char name[32];
  char body[512];
  size_t cap;
  uint64_t count;
  uint64_t attrs_count;
  dom_tree_node_attr_t **attrs;
  struct dom_tree_node *parent;
  struct dom_tree_node **children;
};

typedef struct dom_tree_node dom_tree_node_t;

dom_tree_node_t *dom_tree_node_new(const char *name, const char *body, const size_t cap);

void dom_tree_node_destroy(dom_tree_node_t *self);

bool dom_tree_node_append(dom_tree_node_t *self, dom_tree_node_t *node);

bool dom_tree_node_append_attribute(dom_tree_node_t *self, dom_tree_node_attr_t *attr);

void __dom_tree_node_print(const dom_tree_node_t *self);
void dom_tree_node_print(const dom_tree_node_t *self);

struct dom_tree_node_stack
{
  size_t cap;
  uint64_t top;
  dom_tree_node_t *nodes[];
};

typedef struct dom_tree_node_stack dom_tree_node_stack_t;

dom_tree_node_stack_t *dom_tree_node_stack_new(const size_t cap);

void dom_tree_node_stack_destroy(dom_tree_node_stack_t *self);

bool dom_tree_node_stack_push(dom_tree_node_stack_t *self, dom_tree_node_t *node);

dom_tree_node_t *dom_tree_node_stack_peek(dom_tree_node_stack_t *self);

dom_tree_node_t *dom_tree_node_stack_pop(dom_tree_node_stack_t *self);

struct dom_tree_node_queue
{
  size_t cap;
  uint64_t r;
  uint64_t w;
  dom_tree_node_t *nodes[];
};

typedef struct dom_tree_node_queue dom_tree_node_queue_t;

dom_tree_node_queue_t *dom_tree_node_queue_new(const size_t cap);

void dom_tree_node_queue_destroy(dom_tree_node_queue_t *self);

bool dom_tree_node_queue_enqueue(dom_tree_node_queue_t *self, dom_tree_node_t *node);

dom_tree_node_t *dom_tree_node_queue_dequeue(dom_tree_node_queue_t *self);

#endif/*NODE_H*/
