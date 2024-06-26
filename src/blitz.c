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
#include "text/cmpl.h"
#include "text/tree.h"
#include "io.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

content_tree_t *dom_tree_expand(dom_tree_t *self)
{
  content_tree_t *tree = NULL;
  content_tree_t *subtree = NULL;
  content_tree_node_queue_t *content_tree_que = NULL;
  content_tree_node_t *parent = NULL;
  dom_tree_node_queue_t *que = NULL;
  dom_tree_node_t *node = NULL;
  uint64_t i;

  tree = content_tree_new();
  content_tree_que = content_tree_node_queue_new(CONTENT_TREE_NODE_QUEUE_CAPACITY);
  que = dom_tree_node_queue_new(DOM_TREE_NODE_QUEUE_CAPACITY);

  que = dom_tree_node_queue_enqueue(que, self->root);

  tree->root = content_tree_node_new(self->root->body, strlen(self->root->body), CONTENT_TREE_NODE_CAPACITY);
  content_tree_que = content_tree_node_queue_enqueue(content_tree_que, tree->root);

  while (NULL != (node = dom_tree_node_queue_dequeue(que)))
  {
    parent = content_tree_node_queue_dequeue(content_tree_que);

    for (i = 0ul; i < node->count; i++)
    {
      if (node->children[i] == NULL)
      {
        continue;
      }

      que = dom_tree_node_queue_enqueue(que, node->children[i]);

      if (NULL == node->children[i]->body)
      {
        continue;
      }

      subtree = text_compile(node->children[i]->body);

      if (parent != NULL)
      {
        if (false == content_tree_node_append(parent, subtree->root))
        {
          fprintf(stderr, "%s(): %s\n", __func__, "could not append content tree node child to parent");
          exit(EXIT_FAILURE);
        }
      }

      content_tree_que = content_tree_node_queue_enqueue(content_tree_que, subtree->root);

      subtree->root = NULL;
      content_tree_destroy(subtree);
      subtree = NULL;
    }
  }

  dom_tree_node_queue_destroy(que);
  content_tree_node_queue_destroy(content_tree_que);
  return tree;
}

int main(int argc, const char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "not enough arguments pass a filepath");
    return EXIT_FAILURE;
  }
  // content_tree_t *content_tree = NULL;
  dom_tree_t *dom_tree = NULL;
  dom_tree = html_parse_file(argv[1]);
  // content_tree = dom_tree_expand(dom_tree);
  dom_tree_print(dom_tree);
  // content_tree_print(content_tree);
  dom_tree_destroy(dom_tree);
  // content_tree_destroy(content_tree);
  return EXIT_SUCCESS;
}
