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
#include "io.h"
#include "token.h"

#include <stdio.h>
#include <stdlib.h>

static content_tree_node_t *parse2(token_queue_t *que)
{
  content_tree_node_t *parent = NULL;
  content_tree_node_t *node = NULL;
  token_t *tok = NULL;

  parent = content_tree_node_new(NULL, 0ul, CONTENT_TREE_NODE_CAPACITY);

  while (NULL != (tok = token_queue_dequeue(que)))
  {
    switch (tok->kind)
    {
      case KIND_SPACE:
        break;

      case KIND_WORD:
        node = content_tree_node_new(tok->data, tok->size, CONTENT_TREE_NODE_CAPACITY);
        if (false == content_tree_node_append(parent, node))
        {
          fprintf(stderr, "%s(): %s\n", __func__, "could not append child node to parent node");
          exit(EXIT_FAILURE);
        }
        break;

      case KIND_PERIOD:
      case KIND_EXCL:
      case KIND_COMMA:
      case KIND_COLON:
      case KIND_SEMI_COLON:
        fprintf(stderr, "%s(): %s(%d)\n", __func__, "illegal token.kind", tok->kind);
        exit(EXIT_FAILURE);

      default:
        fprintf(stderr, "%s(): %s(%d)\n", __func__, "unknown token.kind", tok->kind);
        exit(EXIT_FAILURE);
    }
  }

  return parent;
}

content_tree_t *text_parse(token_queue_t *que)
{
  content_tree_t *tree = NULL;
  content_tree_node_t *node = NULL;
  token_queue_t *new_que = NULL;
  token_t *tok = NULL;

  tree = content_tree_new();
  new_que = token_queue_new((1ul << 12));
  tree->root = content_tree_node_new(NULL, 0ul, CONTENT_TREE_NODE_CAPACITY);

  while (NULL != (tok = token_queue_dequeue(que)))
  {
    switch (tok->kind)
    {
      case KIND_SPACE:
      case KIND_WORD:
        if (false == token_queue_enqueue_back(new_que, tok))
        {
          fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into token queue");
          exit(EXIT_FAILURE);
        }
        break;

      case KIND_PERIOD:
      case KIND_EXCL:
      case KIND_COMMA:
      case KIND_COLON:
      case KIND_SEMI_COLON:
        node = parse2(new_que);
        token_queue_destroy(new_que);
        if (false == content_tree_node_append(tree->root, node))
        {
          fprintf(stderr, "%s(): %s\n", __func__, "could not append child node to parent node");
          exit(EXIT_FAILURE);
        }
        new_que = token_queue_new((1ul << 12));
        break;

      default:
        fprintf(stderr, "%s(): %s(%d)\n", __func__, "unknown token.kind", tok->kind);
        exit(EXIT_FAILURE);
    }
  }

  return tree;
}
