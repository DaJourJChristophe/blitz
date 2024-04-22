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

#include <stddef.h>
#include <string.h>

static dom_tree_node_t *__dom_tree_get_element_by_name(const dom_tree_node_t *self, const char *name)
{
  if (self == NULL)
  {
    return NULL;
  }

  dom_tree_node_t *node = NULL;
  uint64_t i;

  if (0 == strcmp(self->name, name))
  {
    return (dom_tree_node_t *)self;
  }

  for (i = 0ul; i < self->count; i++)
  {
    if (self->children[i] == NULL)
    {
      continue;
    }

    node = __dom_tree_get_element_by_name(self->children[i], name);
    if (node != NULL)
    {
      break;
    }
  }

  return node;
}

dom_tree_node_t *dom_tree_get_element_by_name(const dom_tree_t *self, const char *name)
{
  return __dom_tree_get_element_by_name(self->root, name);
}
