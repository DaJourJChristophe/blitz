#ifndef STATE_H
#define STATE_H

#include "node.h"
#include "token.h"

#include <stddef.h>
#include <stdint.h>

#define STATE_QUEUE_CAPACITY 5ul

struct state_queue;
struct dom_tree;
struct dom_tree_node_attr_stack;

typedef int (*parse_state_t)(struct dom_tree *, dom_tree_node_stack_t *, struct dom_tree_node_attr_stack *, struct state_queue *, token_queue_t *);

struct state_queue
{
  size_t cap;
  uint64_t r;
  uint64_t w;
  parse_state_t states[];
};

typedef struct state_queue state_queue_t;

state_queue_t *state_queue_new(const size_t cap);

void state_queue_destroy(state_queue_t *self);

bool state_queue_enqueue_back(state_queue_t *self, parse_state_t state);

bool state_queue_enqueue_front(state_queue_t *self, parse_state_t state);

parse_state_t state_queue_dequeue(state_queue_t *self);

size_t state_queue_size(state_queue_t *self);

#endif/*STATE_H*/
