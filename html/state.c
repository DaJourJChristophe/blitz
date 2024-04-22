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
#include "token.h"
#include "state.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void state_queue_setup(state_queue_t *self, const size_t size, const size_t cap)
{
  memset(self, 0, size);
  self->cap = cap;
}

state_queue_t *state_queue_new(const size_t cap)
{
  const size_t size = offsetof(state_queue_t, states[cap]);
  state_queue_t *self = NULL;
  self = (state_queue_t *)malloc(size);
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }
  state_queue_setup(self, size, cap);
  return self;
}

void state_queue_destroy(state_queue_t *self)
{
  if (self != NULL)
  {
    free(self);
    self = NULL;
  }
}

bool state_queue_enqueue_back(state_queue_t *self, parse_state_t state)
{
  if ((self->w - self->r) >= self->cap)
  {
    return false;
  }
  self->states[self->w++ % self->cap] = state;
  return true;
}

bool state_queue_enqueue_front(state_queue_t *self, parse_state_t state)
{
  if ((self->w - self->r) >= self->cap)
  {
    return false;
  }
  self->states[--self->r % self->cap] = state;
  return true;
}

parse_state_t state_queue_dequeue(state_queue_t *self)
{
  if (self->r == self->w)
  {
    return NULL;
  }
  return self->states[self->r++ % self->cap];
}

size_t state_queue_size(state_queue_t *self)
{
  return self->w - self->r;
}
