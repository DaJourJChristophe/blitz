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
#include "parse.h"
#include "tree.h"

#include <stdlib.h>

int main(void)
{
  char data[] = "<!DOCTYPE html>\n<html dir=\"ltr\" lang=\"en-US\">\n  <head></head>\n  <body></body>\n</html>\n";
  dom_tree_t *tree = NULL;
  tree = parse(data);
  dom_tree_print(tree);
  dom_tree_destroy(tree);
  return EXIT_SUCCESS;
}
