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

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void token_print(const token_t *self)
{
  switch (self->kind)
  {
    case KIND_SPACE:
      printf("%c", ' ');
      break;

    case KIND_LT_CARET:
      printf("%c", '<');
      break;

    case KIND_RT_CARET:
      printf("%c", '>');
      break;

    case KIND_FWD_SLASH:
      printf("%c", '/');
      break;

    case KIND_EQUALS:
      printf("%c", '=');
      break;

    case KIND_DBL_QUOT:
      printf("%c", '"');
      break;

    case KIND_SNG_QUOT:
      printf("%c", '\'');
      break;

    case KIND_EXCL:
      printf("%c", '!');
      break;

    case KIND_WORD:
      printf("%s", (char *)self->data);
      break;

    case KIND_DASH:
      printf("%c", '-');
      break;

    default:
      fprintf(stderr, "%s(): %s\n", __func__, "unknown token.kind");
      exit(EXIT_FAILURE);
  }
}

static void token_queue_setup(token_queue_t *self, const size_t size, const size_t cap)
{
  memset(self, 0, sizeof(*self));
  self->cap = cap;
}

token_queue_t *token_queue_new(const size_t cap)
{
  const size_t size = offsetof(token_queue_t, toks[cap]);
  token_queue_t *self = NULL;
  self = (token_queue_t *)malloc(size);
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }
  token_queue_setup(self, size, cap);
  return self;
}

void token_queue_destroy(token_queue_t *self)
{
  if (self != NULL)
  {
    free(self);
    self = NULL;
  }
}

bool token_queue_enqueue(token_queue_t *self, token_t *tok)
{
  if ((self->w - self->r) >= self->cap)
  {
    return false;
  }
  memcpy((self->toks + (self->w++ % self->cap)), tok, sizeof(*self->toks));
  return true;
}

token_t *token_queue_dequeue(token_queue_t *self)
{
  if (self->r == self->w)
  {
    return NULL;
  }
  return (self->toks + (self->r++ % self->cap));
}

token_t *token_queue_current(token_queue_t *self)
{
  return (self->toks + (self->w % self->cap));
}

bool token_queue_next(token_queue_t *self)
{
  if ((self->w - self->r) >= self->cap)
  {
    return false;
  }
  self->w++;
  return true;
}
