#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>
#include <stdint.h>

enum
{
  KIND_SPACE,
  KIND_LT_CARET,
  KIND_RT_CARET,
  KIND_FWD_SLASH,
  KIND_EQUALS,
  KIND_DBL_QUOT,
  KIND_SNG_QUOT,
  KIND_EXCL,
  KIND_WORD,
  KIND_DASH,
};

struct token
{
  int kind;
  void *data;
  size_t size;
};

typedef struct token token_t;

void token_print(const token_t *self);

struct token_queue
{
  size_t cap;
  uint64_t r;
  uint64_t w;
  token_t toks[];
};

typedef struct token_queue token_queue_t;

token_queue_t *token_queue_new(const size_t cap);

void token_queue_destroy(token_queue_t *self);

bool token_queue_enqueue(token_queue_t *self, token_t *tok);

token_t *token_queue_dequeue(token_queue_t *self);

token_t *token_queue_current(token_queue_t *self);

bool token_queue_next(token_queue_t *self);

#endif/*TOKEN_H*/