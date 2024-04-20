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
#include "node.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void dom_tree_node_setup(dom_tree_node_t *self, const size_t cap)
{
  self->cap = cap;
}

dom_tree_node_t *dom_tree_node_new(const char *name, const char *body, const size_t cap)
{
  dom_tree_node_t *self = NULL;
  self = (dom_tree_node_t *)calloc(1ul, sizeof(*self));
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }

  dom_tree_node_setup(self, cap);

  if (name != NULL)
  {
    strcpy(self->name, name);
  }

  if (body != NULL)
  {
    strcpy(self->body, body);
  }

  return self;
}

void dom_tree_node_destroy(dom_tree_node_t *self)
{
  if (self != NULL)
  {
    if (self->children != NULL)
    {
      free(self->children);
      self->children = NULL;
    }

    free(self);
    self = NULL;
  }
}

bool dom_tree_node_append(dom_tree_node_t *self, dom_tree_node_t *node)
{
  if (self->count >= self->cap)
  {
    return false;
  }

  node->parent = self;

  if (self->children == NULL)
  {
    self->children = (dom_tree_node_t **)calloc(1ul, sizeof(*self->children));
  }
  else
  {
    void *__old = self->children;
    self->children = NULL;
    self->children = (dom_tree_node_t **)realloc(__old, (1ul + self->count) * sizeof(*self->children));
  }

  if (self->children == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }

  self->children[self->count++] = node;
  return true;
}

bool dom_tree_node_append_attribute(dom_tree_node_t *self, dom_tree_node_attr_t *attr)
{
  if (self->attrs_count >= 16ul)
  {
    return false;
  }

  if (self->attrs == NULL)
  {
    self->attrs = (dom_tree_node_attr_t **)calloc(1ul, sizeof(*self->attrs));
  }
  else
  {
    void *__old = self->attrs;
    self->attrs = NULL;
    self->attrs = (dom_tree_node_attr_t **)realloc(__old, (1ul + self->attrs_count) * sizeof(*self->attrs));
  }

  if (self->attrs == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }

  self->attrs[self->attrs_count++] = attr;
  return true;
}

void dom_tree_node_print_open(const dom_tree_node_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "null pointer exception");
    exit(EXIT_FAILURE);
  }

  uint64_t i;

  printf("<%s", self->name);

  if (0ul < self->attrs_count)
  {
    printf("%c", ' ');
  }

  for (i = 0ul; i < self->attrs_count; i++)
  {
    printf("%s=\"%s\"", self->attrs[i]->name, self->attrs[i]->value);

    if ((1ul + i) < self->attrs_count)
    {
      printf("%c", ' ');
    }
  }

  printf("%c", '>');
}

void dom_tree_node_print_close(const dom_tree_node_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "null pointer exception");
    exit(EXIT_FAILURE);
  }
  printf("</%s>", self->name);
}

void dom_tree_node_print(const dom_tree_node_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "null pointer exception");
    exit(EXIT_FAILURE);
  }
  printf("<%s>%s</%s>", self->name, self->body, self->name);
}

void __dom_tree_node_print(const dom_tree_node_t *self)
{
  if (self == NULL)
  {
    return;
  }

  uint64_t i;

  dom_tree_node_print_open(self);

  for (i = 0ul; i < self->count; i++)
  {
    if (self->children[i] == NULL)
    {
      continue;
    }

    __dom_tree_node_print(self->children[i]);
  }

  dom_tree_node_print_close(self);
}

static void dom_tree_node_stack_setup(dom_tree_node_stack_t *self, const size_t size, const size_t cap)
{
  memset(self, 0, size);
  self->cap = cap;
}

dom_tree_node_stack_t *dom_tree_node_stack_new(const size_t cap)
{
  const size_t size = offsetof(dom_tree_node_stack_t, nodes[cap]);
  dom_tree_node_stack_t *self = NULL;
  self = (dom_tree_node_stack_t *)malloc(size);
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }
  dom_tree_node_stack_setup(self, size, cap);
  return self;
}

void dom_tree_node_stack_destroy(dom_tree_node_stack_t *self)
{
  if (self == NULL)
  {
    free(self);
    self = NULL;
  }
}

bool dom_tree_node_stack_push(dom_tree_node_stack_t *self, dom_tree_node_t *node)
{
  if (self->top >= self->cap)
  {
    return false;
  }
  self->nodes[self->top++] = node;
  return true;
}

dom_tree_node_t *dom_tree_node_stack_peek(dom_tree_node_stack_t *self)
{
  if (0ul == self->top)
  {
    return NULL;
  }
  return self->nodes[self->top - 1ul];
}

dom_tree_node_t *dom_tree_node_stack_pop(dom_tree_node_stack_t *self)
{
  if (0ul == self->top)
  {
    return NULL;
  }
  return self->nodes[--self->top];
}

static void dom_tree_node_queue_setup(dom_tree_node_queue_t *self, const size_t size, const size_t cap)
{
  memset(self, 0, size);
  self->cap = cap;
}

dom_tree_node_queue_t *dom_tree_node_queue_new(const size_t cap)
{
  const size_t size = offsetof(dom_tree_node_queue_t, nodes[cap]);
  dom_tree_node_queue_t *self = NULL;
  self = (dom_tree_node_queue_t *)malloc(size);
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }
  dom_tree_node_queue_setup(self, size, cap);
  return self;
}

void dom_tree_node_queue_destroy(dom_tree_node_queue_t *self)
{
  if (self == NULL)
  {
    free(self);
    self = NULL;
  }
}

bool dom_tree_node_queue_enqueue(dom_tree_node_queue_t *self, dom_tree_node_t *node)
{
  if ((self->w - self->r) >= self->cap)
  {
    return false;
  }
  self->nodes[self->w++ % self->cap] = node;
  return true;
}

dom_tree_node_t *dom_tree_node_queue_dequeue(dom_tree_node_queue_t *self)
{
  if (self->r == self->w)
  {
    return NULL;
  }
  return self->nodes[self->r++ % self->cap];
}
