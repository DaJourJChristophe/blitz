#ifndef TREE_H
#define TREE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define CONTENT_TREE_NODE_CAPACITY (1ul << 12)

struct content_tree_node
{
  uint8_t *data;
  size_t size;
  size_t cap;
  uint64_t count;
  struct content_tree_node *parent;
  struct content_tree_node **children;
};

typedef struct content_tree_node content_tree_node_t;

content_tree_node_t *content_tree_node_new(const void *data, const size_t size, const size_t cap);

void content_tree_node_destroy(content_tree_node_t *self);

bool content_tree_node_append(content_tree_node_t *self, content_tree_node_t *node);

void content_tree_node_print(const content_tree_node_t *self);

struct content_tree
{
  content_tree_node_t *root;
};

typedef struct content_tree content_tree_t;

content_tree_t *content_tree_new(void);

void content_tree_destroy(content_tree_t *self);

void content_tree_print(const content_tree_t *self);

#endif/*TREE_H*/
