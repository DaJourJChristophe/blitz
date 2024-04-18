#ifndef STATE_H
#define STATE_H

#include "token.h"

#include <stddef.h>
#include <stdint.h>

struct state_queue;

typedef void (*parse_state_t)(struct state_queue *, token_queue_t *);

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

bool state_queue_enqueue(state_queue_t *self, parse_state_t state);

parse_state_t state_queue_dequeue(state_queue_t *self);

#endif/*STATE_H*/
