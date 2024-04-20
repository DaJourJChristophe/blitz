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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct queue
{
  size_t cap;
  uint64_t r;
  uint64_t w;
  int items[1u << 5];
};

typedef struct queue queue_t;

void queue_setup(queue_t *self)
{
  memset(self, 0, sizeof(*self));
  self->cap = 1ul << 5;
}

bool queue_enqueue(queue_t *self, const int item)
{
  if ((self->w - self->r) >= self->cap)
  {
    return false;
  }
  self->items[self->w++ % self->cap] = item;
  return true;
}

int *queue_dequeue(queue_t *self)
{
  if (self->r == self->w)
  {
    return NULL;
  }
  return (self->items + (self->r++ % self->cap));
}

static graph_node_t *graph_node_new(const char *name)
{
  graph_node_t *self = NULL;
  self = (graph_node_t *)calloc(1ul, sizeof(*self));
  strcpy(self->name, name);
  return self;
}

static void graph_node_destroy(graph_node_t *self)
{
  if (self != NULL)
  {
    free(self);
    self = NULL;
  }
}

static void graph_node_queue_setup(graph_node_queue_t *self, const size_t size, const size_t cap)
{
  memset(self, 0, size);
  self->cap = cap;
}

graph_node_queue_t *graph_node_queue_new(const size_t cap)
{
  const size_t size = offsetof(graph_node_queue_t, nodes[cap]);
  graph_node_queue_t *self = NULL;
  self = (graph_node_queue_t *)malloc(size);
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }
  graph_node_queue_setup(self, size, cap);
  return self;
}

void graph_node_queue_destroy(graph_node_queue_t *self)
{
  if (self == NULL)
  {
    free(self);
    self = NULL;
  }
}

bool graph_node_queue_enqueue(graph_node_queue_t *self, graph_node_t *node)
{
  if ((self->w - self->r) >= self->cap)
  {
    return false;
  }
  self->nodes[self->w++ % self->cap] = node;
  return true;
}

graph_node_t *graph_node_queue_dequeue(graph_node_queue_t *self)
{
  if (self->r == self->w)
  {
    return NULL;
  }
  return self->nodes[self->r++ % self->cap];
}

static void graph_setup(graph_t *self, const size_t size, const size_t cap)
{
  memset(self, 0, size);
  self->cap = cap;
}

graph_t *graph_new(const size_t cap)
{
  const size_t size = offsetof(graph_t, adj_list[cap]);
  graph_t *self = NULL;
  self = (graph_t *)malloc(size);
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }
  return self;
}

void graph_destroy(graph_t *self)
{
  if (self != NULL)
  {
    free(self);
    self = NULL;
  }
}

static void graph_add_node(graph_t *self, graph_node_t *node)
{
  node->vertex = self->count++;
  self->vertices[node->vertex] = node;
}

void graph_add_directed_edge(graph_t *self, graph_node_t *dst, graph_node_t *src, const int weight)
{
  // dst->next = self->adj_list[src->vertex];
  // self->adj_list[src->vertex] = dst;
  src->weight = weight;
  src->next = self->adj_list[dst->vertex];
  self->adj_list[dst->vertex] = src;
}

graph_node_t *graph_node_create(graph_t *self, const char *name)
{
  graph_node_t *node = graph_node_new(name);
  graph_add_node(self, node);
  return node;
}

void graph_BFS(graph_t *self, const int start)
{
  queue_t que;
  graph_node_t *node = NULL;
  int *item = NULL;

  queue_setup(&que);

  self->visited[start] = 1;
  if (false == queue_enqueue(&que, start))
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue item into queue");
    exit(EXIT_FAILURE);
  }

  while (NULL != (item = queue_dequeue(&que)))
  {
    printf("(%d)%s ", self->vertices[*item]->weight, self->vertices[*item]->name);

    node = self->adj_list[*item];

    while (node != NULL)
    {
      if (0 == self->visited[node->vertex])
      {
        self->visited[node->vertex] = 1;

        if (false == queue_enqueue(&que, node->vertex))
        {
          fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue item into queue");
          exit(EXIT_FAILURE);
        }
      }

      node = node->next;
    }
  }

  printf("\n");
}
