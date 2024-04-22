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
#include "lex.h"
#include "parse.h"
#include "token.h"
#include "tree.h"

#include <stdio.h>

content_tree_t *text_compile(const char *data)
{
  content_tree_t *tree = NULL;
  token_queue_t  *que  = NULL;

  que  = text_lex(data);
  tree = text_parse(que);

  token_queue_destroy(que);
  return tree;
}
