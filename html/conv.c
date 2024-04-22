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
#include "graph.h"
#include "tree.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void dom_tree_BFS(const dom_tree_t *self)
{
  graph_t *graph = NULL;
  graph_node_queue_t *graph_que = NULL;
  graph_node_t *dst = NULL;
  graph_node_t *src = NULL;
  dom_tree_node_queue_t *que = NULL;
  dom_tree_node_t *node = NULL;
  uint64_t i;
  uint64_t depth;

  depth = 0ul;
  graph = graph_new(1ul << 5);

  graph_que = graph_node_queue_new((1ul << 5));
  que = dom_tree_node_queue_new((1ul << 5));

  if (false == dom_tree_node_queue_enqueue(que, self->root))
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue node into node queue");
    exit(EXIT_FAILURE);
  }

  src = graph_node_create(graph, self->root->name);
  if (false == graph_node_queue_enqueue(graph_que, src))
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue node into node queue");
    exit(EXIT_FAILURE);
  }

  while (NULL != (node = dom_tree_node_queue_dequeue(que)))
  {
    dst = graph_node_queue_dequeue(graph_que);

    for (i = node->count; i > 0ul; i--)
    {
      if (NULL == node->children[i - 1ul])
      {
        continue;
      }

      if (false == dom_tree_node_queue_enqueue(que, node->children[i - 1ul]))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue node into node queue");
        exit(EXIT_FAILURE);
      }

      src = graph_node_create(graph, node->children[i - 1ul]->name);

      graph_add_directed_edge(graph, dst, src, depth);

      if (false == graph_node_queue_enqueue(graph_que, src))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue node into node queue");
        exit(EXIT_FAILURE);
      }
    }
  }

  graph_BFS(graph, 0);

  dom_tree_node_queue_destroy(que);
  graph_node_queue_destroy(graph_que);
  graph_destroy(graph);
}
