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
#include "html/conv.h"
#include "html/parse.h"
#include "html/tree.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
  char data[4096];
  memset(data, 0, 4096 * sizeof(*data));
  if (readfile(data, "index.html") < 0)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not read from file");
    exit(EXIT_FAILURE);
  }
  dom_tree_t *tree = NULL;
  tree = parse(data);
  dom_tree_print(tree);
  dom_tree_destroy(tree);
  return EXIT_SUCCESS;
}
