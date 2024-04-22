/**
 * Academic Free License ("AFL") v. 3.0
 *
 * Copyright 2024 Da'Jour J. Christophe. All rights reserved.
 *
 * This Academic Free License (the "License") applies to any original
 * work of authorship (the "Original Work") whose owner (the "Licensor")
 * has placed the following licensing notice adjacent to the copyright
 * notice for the Original Work:
 *
 * Licensed under the Academic Free License version 3.0.
 */
#include "tree.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void content_tree_node_setup(content_tree_node_t *self, const void *data, const size_t size, const size_t cap)
{
  if (data != NULL)
  {
    memcpy(self->data, data, size);
  }

  self->size = size;
  self->cap  = cap;
}

content_tree_node_t *content_tree_node_new(const void *data, const size_t size, const size_t cap)
{
  content_tree_node_t *self = NULL;
  self = (content_tree_node_t *)calloc(1ul, sizeof(*self));
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }

  self->data = (uint8_t *)calloc(size, sizeof(*self->data));
  if (self->data == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }

  content_tree_node_setup(self, data, size, cap);
  return self;
}

void content_tree_node_destroy(content_tree_node_t *self)
{
  if (self != NULL)
  {
    if (self->data != NULL)
    {
      free(self->data);
      self->data = NULL;
    }

    free(self);
    self = NULL;
  }
}

bool content_tree_node_append(content_tree_node_t *self, content_tree_node_t *node)
{
  if (self->count >= self->cap)
  {
    return false;
  }

  node->parent = self;

  if (self->children == NULL)
  {
    self->children = (content_tree_node_t **)calloc(1ul, sizeof(*self->children));
  }
  else
  {
    void *__old = self->children;
    self->children = NULL;
    self->children = (content_tree_node_t **)realloc(__old, (1ul + self->count) * sizeof(*self->children));
  }

  if (self->children == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }

  self->children[self->count++] = node;
  return true;
}

void content_tree_node_print(const content_tree_node_t *self)
{
  // NOTE: Check for a ulong specifier ..
  printf("%.*s\n", (int)self->size, (char *)self->data);
}

static void __content_tree_node_print(const content_tree_node_t *self)
{
  if (self == NULL)
  {
    return;
  }

  uint64_t i;

  content_tree_node_print(self);

  for (i = 0ul; i < self->count; i++)
  {
    if (NULL == self->children[i])
    {
      continue;
    }

    __content_tree_node_print(self->children[i]);
  }
}

static void content_tree_node_queue_setup(content_tree_node_queue_t *self, const size_t size, const size_t cap)
{
  memset(self, 0, size);
  self->cap = cap;
}

content_tree_node_queue_t *content_tree_node_queue_new(const size_t cap)
{
  const size_t size = offsetof(content_tree_node_queue_t, nodes[cap]);
  content_tree_node_queue_t *self = NULL;
  self = (content_tree_node_queue_t *)malloc(size);
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }
  content_tree_node_queue_setup(self, size, cap);
  return self;
}

void content_tree_node_queue_destroy(content_tree_node_queue_t *self)
{
  if (self == NULL)
  {
    free(self);
    self = NULL;
  }
}

bool content_tree_node_queue_enqueue(content_tree_node_queue_t *self, content_tree_node_t *node)
{
  if ((self->w - self->r) >= self->cap)
  {
    return false;
  }
  self->nodes[self->w++ % self->cap] = node;
  return true;
}

content_tree_node_t *content_tree_node_queue_dequeue(content_tree_node_queue_t *self)
{
  if (self->r == self->w)
  {
    return NULL;
  }
  return self->nodes[self->r++ % self->cap];
}

content_tree_t *content_tree_new(void)
{
  content_tree_t *self = NULL;
  self = (content_tree_t *)calloc(1ul, sizeof(*self));
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }
  return self;
}

void content_tree_destroy(content_tree_t *self)
{
  if (self != NULL)
  {
    free(self);
    self = NULL;
  }
}

void content_tree_print(const content_tree_t *self)
{
  __content_tree_node_print(self->root);
}
