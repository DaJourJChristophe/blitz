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
#include "attr.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dom_tree_node_attr_t *dom_tree_node_attr_new(const char *name, const char *value)
{
  dom_tree_node_attr_t *self = NULL;
  self = (dom_tree_node_attr_t *)calloc(1ul, sizeof(*self));
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }

  if (name != NULL)
  {
    strcpy(self->name, name);
  }

  if (value != NULL)
  {
    strcpy(self->value, value);
  }

  return self;
}

void dom_tree_node_attr_destroy(dom_tree_node_attr_t *self)
{
  if (self != NULL)
  {
    if (self->value != NULL)
    {
      free(self->value);
      self->value = NULL;
    }

    free(self);
    self = NULL;
  }
}

bool dom_tree_node_attr_append_value(dom_tree_node_attr_t *self, const void *data, const size_t size)
{
  if (self->value == NULL)
  {
    self->value = (char *)calloc(DOM_TREE_NODE_ATTR_VALLEN_DEFAULT, sizeof(*self->value));
  }
  else
  {
    const size_t prev_threshold = self->vallen / DOM_TREE_NODE_ATTR_VALLEN_DEFAULT;
    const size_t curr_threshold = (size + self->vallen) / DOM_TREE_NODE_ATTR_VALLEN_DEFAULT;
    if (curr_threshold > prev_threshold)
    {
      void *__old = self->value;
      self->value = NULL;
      self->value = (char *)realloc(__old, curr_threshold * DOM_TREE_NODE_ATTR_VALLEN_DEFAULT * sizeof(self->value));
    }
  }
  if (self->value == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }
  memcpy((self->value + self->vallen), data, size);
  self->vallen += size;
  return true;
}

static void dom_tree_node_attr_stack_setup(dom_tree_node_attr_stack_t *self, const size_t size, const size_t cap)
{
  memset(self, 0, size);
  self->cap = cap;
}

dom_tree_node_attr_stack_t *dom_tree_node_attr_stack_new(const size_t cap)
{
  const size_t size = offsetof(dom_tree_node_attr_stack_t, attrs[cap]);
  dom_tree_node_attr_stack_t *self = NULL;
  self = (dom_tree_node_attr_stack_t *)malloc(size);
  dom_tree_node_attr_stack_setup(self, size, cap);
  return self;
}

void dom_tree_node_attr_stack_destroy(dom_tree_node_attr_stack_t *self)
{
  if (self != NULL)
  {
    free(self);
    self = NULL;
  }
}

bool dom_tree_node_attr_stack_push(dom_tree_node_attr_stack_t *self, dom_tree_node_attr_t *attr)
{
  if (self->top >= self->cap)
  {
    return false;
  }
  self->attrs[self->top++] = attr;
  return true;
}

dom_tree_node_attr_t *dom_tree_node_attr_stack_peek(dom_tree_node_attr_stack_t *self)
{
  if (0ul == self->top)
  {
    return NULL;
  }
  return self->attrs[self->top - 1ul];
}

dom_tree_node_attr_t *dom_tree_node_attr_stack_pop(dom_tree_node_attr_stack_t *self)
{
  if (0ul == self->top)
  {
    return NULL;
  }
  return self->attrs[--self->top];
}
