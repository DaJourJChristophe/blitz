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
#include <stdio.h>
#include <stdlib.h>

dom_tree_t *dom_tree_new(void)
{
  dom_tree_t *self = NULL;
  self = (dom_tree_t *)calloc(1ul, sizeof(*self));
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }
  return self;
}

void dom_tree_destroy(dom_tree_t *self)
{
  if (self != NULL)
  {
    free(self);
    self = NULL;
  }
}

void dom_tree_print(const dom_tree_t *self)
{
  __dom_tree_node_print(self->root);
}
