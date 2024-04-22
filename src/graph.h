#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct graph_node
{
  char name[32];
  int weight;
  int vertex;
  struct graph_node *next;
};

typedef struct graph_node graph_node_t;

struct graph_node_queue
{
  size_t cap;
  uint64_t r;
  uint64_t w;
  graph_node_t *nodes[];
};

typedef struct graph_node_queue graph_node_queue_t;

graph_node_queue_t *graph_node_queue_new(const size_t cap);

void graph_node_queue_destroy(graph_node_queue_t *self);

bool graph_node_queue_enqueue(graph_node_queue_t *self, graph_node_t *node);

graph_node_t *graph_node_queue_dequeue(graph_node_queue_t *self);

struct graph
{
  size_t cap;
  uint64_t count;
  int visited[1u << 5];
  graph_node_t *vertices[1u << 5];
  graph_node_t *adj_list[];
};

typedef struct graph graph_t;

graph_t *graph_new(const size_t cap);

void graph_destroy(graph_t *self);

void graph_add_directed_edge(graph_t *self, graph_node_t *dst, graph_node_t *src, const int weight);

graph_node_t *graph_node_create(graph_t *self, const char *name);

void graph_BFS(graph_t *self, const int start);

#endif/*GRAPH_H*/
