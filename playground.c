#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>
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

token_t *token_queue_peek(token_queue_t *self);

token_t *token_queue_dequeue(token_queue_t *self);

token_t *token_queue_current(token_queue_t *self);

bool token_queue_next(token_queue_t *self);

#endif/*TOKEN_H*/

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
// #include "token.h"

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

token_t *token_queue_peek(token_queue_t *self)
{
  if (self->r == self->w)
  {
    return NULL;
  }
  return (self->toks + (self->r % self->cap));
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

#ifndef LEX_H
#define LEX_H

// #include "token.h"

token_queue_t *lex(const char *line);

#endif/*LEX_H*/

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
// #include "token.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token_queue_t *lex(const char *line)
{
  token_queue_t *que = NULL;
  token_t *tok = NULL;
  char buf[32];
  uint32_t i;

  que = token_queue_new((1ul << 5));

  for (; *line; line++)
  {
    tok = token_queue_current(que);

    switch (*line)
    {
      case ' ':
        tok->kind = KIND_SPACE;
        break;

      case '<':
        tok->kind = KIND_LT_CARET;
        break;

      case '>':
        tok->kind = KIND_RT_CARET;
        break;

      case '/':
        tok->kind = KIND_FWD_SLASH;
        break;

      case '=':
        tok->kind = KIND_EQUALS;
        break;

      case '"':
        tok->kind = KIND_DBL_QUOT;
        break;

      case '\'':
        tok->kind = KIND_SNG_QUOT;
        break;

      case '!':
        tok->kind = KIND_EXCL;
        break;

      case '-':
        tok->kind = KIND_DASH;
        break;

      default:
        if (isalpha(*line))
        {
          for (i = 0u; *line && isalpha(*line); line++)
          {
            buf[i++] = *line;
          }
          buf[i++] = '\0';
          line--;

          tok->data = calloc(i, sizeof(*buf));
          tok->size = i;
          tok->kind = KIND_WORD;

          memcpy(tok->data, buf, i);
          break;
        }

        fprintf(stderr, "%s(): %s (%c)\n", __func__, "illegal character", *line);
        exit(EXIT_FAILURE);
    }

    if (false == token_queue_next(que))
    {
      fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue token into token queue");
      exit(EXIT_FAILURE);
    }
  }

  return que;
}

struct dom_tree_node_attr
{
  char name[32];
  char value[32];
};

typedef struct dom_tree_node_attr dom_tree_node_attr_t;

dom_tree_node_attr_t *dom_tree_node_attr_new(const char *name, const char *value)
{
  dom_tree_node_attr_t *self = NULL;
  self = (dom_tree_node_attr_t *)calloc(1ul, sizeof(*self));
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }

  if (name != NULL)
  {
    strcpy(self->name, name);
  }

  if (value != NULL)
  {
    strcpy(self->value, value);
  }

  return self;
}

void dom_tree_node_attr_destroy(dom_tree_node_attr_t *self)
{
  if (self != NULL)
  {
    free(self);
    self = NULL;
  }
}

struct dom_tree_node_attr_stack
{
  size_t cap;
  uint64_t top;
  dom_tree_node_attr_t *attrs[];
};

typedef struct dom_tree_node_attr_stack dom_tree_node_attr_stack_t;

static void dom_tree_node_attr_stack_setup(dom_tree_node_attr_stack_t *self, const size_t size, const size_t cap)
{
  memset(self, 0, sizeof(*self));
  self->cap = cap;
}

dom_tree_node_attr_stack_t *dom_tree_node_attr_stack_new(const size_t cap)
{
  const size_t size = offsetof(dom_tree_node_attr_stack_t, attrs[cap]);
  dom_tree_node_attr_stack_t *self = NULL;
  self = (dom_tree_node_attr_stack_t *)malloc(size);
  dom_tree_node_attr_stack_setup(self, size, cap);
  return self;
}

void dom_tree_node_attr_stack_destroy(dom_tree_node_attr_stack_t *self)
{
  if (self != NULL)
  {
    free(self);
    self = NULL;
  }
}

bool dom_tree_node_attr_stack_push(dom_tree_node_attr_stack_t *self, dom_tree_node_attr_t *attr)
{
  if (self->top >= self->cap)
  {
    return false;
  }
  self->attrs[self->top++] = attr;
  return true;
}

dom_tree_node_attr_t *dom_tree_node_attr_stack_peek(dom_tree_node_attr_stack_t *self)
{
  if (0ul == self->top)
  {
    return NULL;
  }
  return self->attrs[self->top - 1ul];
}

dom_tree_node_attr_t *dom_tree_node_attr_stack_pop(dom_tree_node_attr_stack_t *self)
{
  if (0ul == self->top)
  {
    return NULL;
  }
  return self->attrs[--self->top];
}

#ifndef NODE_H
#define NODE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct dom_tree_node
{
  char name[32];
  char body[512];
  size_t cap;
  uint64_t count;
  uint64_t attrs_count;
  dom_tree_node_attr_t **attrs;
  struct dom_tree_node *parent;
  struct dom_tree_node **children;
};

typedef struct dom_tree_node dom_tree_node_t;

dom_tree_node_t *dom_tree_node_new(const char *name, const char *body, const size_t cap);

void dom_tree_node_destroy(dom_tree_node_t *self);

bool dom_tree_node_append(dom_tree_node_t *self, dom_tree_node_t *node);

void __dom_tree_node_print(const dom_tree_node_t *self);
void dom_tree_node_print(const dom_tree_node_t *self);

struct dom_tree_node_stack
{
  size_t cap;
  uint64_t top;
  dom_tree_node_t *nodes[];
};

typedef struct dom_tree_node_stack dom_tree_node_stack_t;

dom_tree_node_stack_t *dom_tree_node_stack_new(const size_t cap);

void dom_tree_node_stack_destroy(dom_tree_node_stack_t *self);

bool dom_tree_node_stack_push(dom_tree_node_stack_t *self, dom_tree_node_t *node);

dom_tree_node_t *dom_tree_node_stack_peek(dom_tree_node_stack_t *self);

dom_tree_node_t *dom_tree_node_stack_pop(dom_tree_node_stack_t *self);

#endif/*NODE_H*/

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
// #include "node.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void dom_tree_node_setup(dom_tree_node_t *self, const size_t cap)
{
  self->cap = cap;
}

dom_tree_node_t *dom_tree_node_new(const char *name, const char *body, const size_t cap)
{
  dom_tree_node_t *self = NULL;
  self = (dom_tree_node_t *)calloc(1ul, sizeof(*self));
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }

  dom_tree_node_setup(self, cap);

  if (name != NULL)
  {
    strcpy(self->name, name);
  }

  if (body != NULL)
  {
    strcpy(self->body, body);
  }

  return self;
}

void dom_tree_node_destroy(dom_tree_node_t *self)
{
  if (self != NULL)
  {
    if (self->children != NULL)
    {
      free(self->children);
      self->children = NULL;
    }

    free(self);
    self = NULL;
  }
}

bool dom_tree_node_append(dom_tree_node_t *self, dom_tree_node_t *node)
{
  if (self->count >= self->cap)
  {
    return false;
  }

  if (self->children == NULL)
  {
    self->children = (dom_tree_node_t **)calloc(1ul, sizeof(*self->children));
  }
  else
  {
    void *__old = self->children;
    self->children = NULL;
    self->children = (dom_tree_node_t **)realloc(__old, (1ul + self->count) * sizeof(*self->children));
  }

  if (self->children == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }

  self->children[self->count++] = node;
  return true;
}

bool dom_tree_node_append_attribute(dom_tree_node_t *self, dom_tree_node_attr_t *attr)
{
  if (self->attrs_count >= 16ul)
  {
    return false;
  }

  if (self->attrs == NULL)
  {
    self->attrs = (dom_tree_node_attr_t **)calloc(1ul, sizeof(*self->attrs));
  }
  else
  {
    void *__old = self->attrs;
    self->attrs = NULL;
    self->attrs = (dom_tree_node_attr_t **)realloc(__old, (1ul + self->attrs_count) * sizeof(*self->attrs));
  }

  if (self->attrs == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }

  self->attrs[self->attrs_count++] = attr;
  return true;
}

void dom_tree_node_print_open(const dom_tree_node_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "null pointer exception");
    exit(EXIT_FAILURE);
  }

  uint64_t i;

  printf("<%s", self->name);

  if (0ul < self->attrs_count)
  {
    printf("%c", ' ');
  }

  for (i = 0ul; i < self->attrs_count; i++)
  {
    printf("%s=\"%s\"", self->attrs[i]->name, self->attrs[i]->value);

    if ((1ul + i) < self->attrs_count)
    {
      printf("%c", ' ');
    }
  }

  printf("%c", '>');
}

void dom_tree_node_print_close(const dom_tree_node_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "null pointer exception");
    exit(EXIT_FAILURE);
  }
  printf("</%s>", self->name);
}

void dom_tree_node_print(const dom_tree_node_t *self)
{
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "null pointer exception");
    exit(EXIT_FAILURE);
  }
  printf("<%s>%s</%s>", self->name, self->body, self->name);
}

void __dom_tree_node_print(const dom_tree_node_t *self)
{
  if (self == NULL)
  {
    return;
  }

  uint64_t i;

  dom_tree_node_print_open(self);

  for (i = 0ul; i < self->count; i++)
  {
    if (self->children[i] == NULL)
    {
      continue;
    }

    __dom_tree_node_print(self->children[i]);
  }

  dom_tree_node_print_close(self);
}

static void dom_tree_node_stack_setup(dom_tree_node_stack_t *self, const size_t size, const size_t cap)
{
  memset(self, 0, size);
  self->cap = cap;
}

dom_tree_node_stack_t *dom_tree_node_stack_new(const size_t cap)
{
  const size_t size = offsetof(dom_tree_node_stack_t, nodes[cap]);
  dom_tree_node_stack_t *self = NULL;
  self = (dom_tree_node_stack_t *)malloc(size);
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }
  dom_tree_node_stack_setup(self, size, cap);
  return self;
}

void dom_tree_node_stack_destroy(dom_tree_node_stack_t *self)
{
  if (self == NULL)
  {
    free(self);
    self = NULL;
  }
}

bool dom_tree_node_stack_push(dom_tree_node_stack_t *self, dom_tree_node_t *node)
{
  if (self->top >= self->cap)
  {
    return false;
  }
  self->nodes[self->top++] = node;
  return true;
}

dom_tree_node_t *dom_tree_node_stack_peek(dom_tree_node_stack_t *self)
{
  if (0ul == self->top)
  {
    return NULL;
  }
  return self->nodes[self->top - 1ul];
}

dom_tree_node_t *dom_tree_node_stack_pop(dom_tree_node_stack_t *self)
{
  if (0ul == self->top)
  {
    return NULL;
  }
  return self->nodes[--self->top];
}

struct dom_tree_node_queue
{
  size_t cap;
  uint64_t r;
  uint64_t w;
  dom_tree_node_t *nodes[];
};

typedef struct dom_tree_node_queue dom_tree_node_queue_t;

static void dom_tree_node_queue_setup(dom_tree_node_queue_t *self, const size_t size, const size_t cap)
{
  memset(self, 0, size);
  self->cap = cap;
}

dom_tree_node_queue_t *dom_tree_node_queue_new(const size_t cap)
{
  const size_t size = offsetof(dom_tree_node_queue_t, nodes[cap]);
  dom_tree_node_queue_t *self = NULL;
  self = (dom_tree_node_queue_t *)malloc(size);
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }
  dom_tree_node_queue_setup(self, size, cap);
  return self;
}

void dom_tree_node_queue_destroy(dom_tree_node_queue_t *self)
{
  if (self == NULL)
  {
    free(self);
    self = NULL;
  }
}

bool dom_tree_node_queue_enqueue(dom_tree_node_queue_t *self, dom_tree_node_t *node)
{
  if ((self->w - self->r) >= self->cap)
  {
    return false;
  }
  self->nodes[self->w++ % self->cap] = node;
  return true;
}

dom_tree_node_t *dom_tree_node_queue_dequeue(dom_tree_node_queue_t *self)
{
  if (self->r == self->w)
  {
    return NULL;
  }
  return self->nodes[self->r++ % self->cap];
}

#ifndef STATE_H
#define STATE_H

// #include "node.h"
// #include "token.h"

#include <stddef.h>
#include <stdint.h>

struct state_queue;
struct dom_tree;
struct dom_tree_node_attr_stack;

typedef void (*parse_state_t)(struct dom_tree *, dom_tree_node_stack_t *, struct dom_tree_node_attr_stack *, struct state_queue *, token_queue_t *);

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
// #include "token.h"
// #include "state.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void state_queue_setup(state_queue_t *self, const size_t size, const size_t cap)
{
  memset(self, 0, sizeof(*self));
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

bool state_queue_enqueue(state_queue_t *self, parse_state_t state)
{
  if ((self->w - self->r) >= self->cap)
  {
    return false;
  }
  self->states[self->w++ % self->cap] = state;
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

#ifndef TREE_H
#define TREE_H

// #include "node.h"

struct dom_tree
{
  char doctype[256];
  dom_tree_node_t *root;
};

typedef struct dom_tree dom_tree_t;

dom_tree_t *dom_tree_new(void);

void dom_tree_destroy(dom_tree_t *self);

void dom_tree_print(const dom_tree_t *self);

#endif/*TREE_H*/

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
// #include "tree.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

dom_tree_t *dom_tree_new(void)
{
  dom_tree_t *self = NULL;
  self = (dom_tree_t *)calloc(1ul, sizeof(*self));
  if (self == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }
  return self;
}

void dom_tree_destroy(dom_tree_t *self)
{
  if (self != NULL)
  {
    free(self);
    self = NULL;
  }
}

void dom_tree_print(const dom_tree_t *self)
{
  printf("<!%s>", self->doctype);
  __dom_tree_node_print(self->root);
}

#ifndef PARSE_H
#define PARSE_H

// #include "tree.h"

dom_tree_t *parse(char *data);

#endif/*PARSE_H*/

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
// #include "lex.h"
// #include "node.h"
// #include "parse.h"
// #include "state.h"
// #include "token.h"
// #include "tree.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct list
{
  size_t size;
  size_t len;
  char *keys[16];
};

typedef struct list list_t;

static void __parse(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, token_queue_t *que);

/**
 * @brief Parse substrings from a string using the parameterized delimiter.
 *
 */
static void parse_lines(list_t *list, char *data, const char delim);

/**
 * @brief Search the list items buffer for a substring.
 *
 */
static char *list_get(list_t *self, const uint64_t i);

dom_tree_t *parse(char *data)
{
  dom_tree_t *tree = NULL;
  dom_tree_node_stack_t *stack = NULL;
  dom_tree_node_attr_stack_t *attr_stack = NULL;
  token_queue_t *que = NULL;
  list_t list;
  uint64_t i;

  tree = dom_tree_new();
  stack = dom_tree_node_stack_new((1ul << 5));
  attr_stack = dom_tree_node_attr_stack_new((1ul << 5));

  memset(&list, 0, sizeof(list));
  parse_lines(&list, data, '\n');

  for (i = 0ul; i < list.size; i++)
  {
    que = lex(list_get(&list, i));

    __parse(tree, stack, attr_stack, que);

    token_queue_destroy(que);
  }

  dom_tree_node_attr_stack_destroy(attr_stack);

  if (1ul != stack->top)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "incomplete");
    exit(EXIT_FAILURE);
  }

  tree->root = dom_tree_node_stack_pop(stack);
  dom_tree_node_stack_destroy(stack);
  return tree;
}

void __parse_elm_close(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);
void __parse_attribute_value(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);
void __parse_attribute_name(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);
void __parse_doctype(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);
void __parse_tag_close(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);
void __parse_tag_name(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);
void __parse_tag_open(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

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
// #include "node.h"
// #include "state.h"
// #include "token.h"

#include <stdio.h>
#include <stdlib.h>

void __parse_attribute_value(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);
void __parse_attribute_name(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);
void __parse_tag_close(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

void __parse_attribute_name(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  dom_tree_node_attr_t *attr = NULL;
  dom_tree_node_t *node = NULL;
  token_t *curr = NULL;
  token_t *next = NULL;

  node = dom_tree_node_stack_peek(stack);

  curr = token_queue_dequeue(que);
  if (curr == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid current token state");
    exit(EXIT_FAILURE);
  }

  switch (curr->kind)
  {
    case KIND_WORD:
      attr = dom_tree_node_attr_new(curr->data, NULL);
      if (false == dom_tree_node_append_attribute(node, attr))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not append attribute to element");
        exit(EXIT_FAILURE);
      }
      if (false == dom_tree_node_attr_stack_push(attr_stack, attr))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not push attribute into attribute stack");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_SPACE:
    case KIND_EQUALS:
    case KIND_DBL_QUOT:
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax :: invalid current token", curr->kind);
      exit(EXIT_FAILURE);
  }

  next = token_queue_peek(que);
  if (next == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid next token state");
    exit(EXIT_FAILURE);
  }

  switch (next->kind)
  {
    case KIND_WORD:
      if (false == state_queue_enqueue(states, &__parse_attribute_name))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_EQUALS:
      if (false == state_queue_enqueue(states, &__parse_attribute_name))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_DBL_QUOT:
      if (false == state_queue_enqueue(states, &__parse_attribute_value))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_RT_CARET:
      if (false == state_queue_enqueue(states, &__parse_tag_close))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_SPACE:
      if (false == state_queue_enqueue(states, &__parse_attribute_name))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax :: invalid next token", next->kind);
      exit(EXIT_FAILURE);
  }
}

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
// #include "node.h"
// #include "state.h"
// #include "token.h"

#include <stdio.h>
#include <stdlib.h>

void __parse_attribute_name(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

void __parse_attribute_value(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  dom_tree_node_attr_t *attr = NULL;
  dom_tree_node_t *node = NULL;
  token_t *curr = NULL;
  token_t *next = NULL;

  node = dom_tree_node_stack_peek(stack);

  curr = token_queue_dequeue(que);
  if (curr == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid current token state");
    exit(EXIT_FAILURE);
  }

  switch (curr->kind)
  {
    case KIND_WORD:
    case KIND_DASH:
      attr = dom_tree_node_attr_stack_peek(attr_stack);
      if (attr == NULL)
      {
        fprintf(stderr, "%s(): %s\n", __func__, "invalid syntax");
        exit(EXIT_FAILURE);
      }
      memcpy(attr->value, curr->data, curr->size);
      break;

    case KIND_DBL_QUOT:
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", curr->kind);
      exit(EXIT_FAILURE);
  }

  next = token_queue_peek(que);
  if (next == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid next token state");
    exit(EXIT_FAILURE);
  }

  switch (next->kind)
  {
    case KIND_WORD:
    case KIND_DASH:
      if (false == state_queue_enqueue(states, &__parse_attribute_value))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_DBL_QUOT:
      dom_tree_node_attr_stack_pop(attr_stack);
      if (false == state_queue_enqueue(states, &__parse_attribute_name))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", next->kind);
      exit(EXIT_FAILURE);
  }
}

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
// #include "node.h"
// #include "state.h"
// #include "token.h"

#include <stdio.h>
#include <stdlib.h>

void __parse_tag_close(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

void __parse_doctype(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  token_t *curr = NULL;
  token_t *next = NULL;

  curr = token_queue_dequeue(que);
  if (curr == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid current token state");
    exit(EXIT_FAILURE);
  }

  switch (curr->kind)
  {
    case KIND_WORD:
      strncat(tree->doctype, (char *)curr->data, curr->size);
      break;

    case KIND_SPACE:
      strncat(tree->doctype, " ", 2ul);
      break;

    case KIND_EXCL:
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", curr->kind);
      exit(EXIT_FAILURE);
  }

  next = token_queue_peek(que);
  if (next == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid next token state");
    exit(EXIT_FAILURE);
  }

  switch (next->kind)
  {
    case KIND_WORD:
    case KIND_SPACE:
      if (false == state_queue_enqueue(states, &__parse_doctype))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_RT_CARET:
      if (false == state_queue_enqueue(states, &__parse_tag_close))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", next->kind);
      exit(EXIT_FAILURE);
  }
}

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
// #include "node.h"
// #include "state.h"
// #include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void __parse_tag_close(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

void __parse_elm_close(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  dom_tree_node_t *parent = NULL;
  dom_tree_node_t *node = NULL;
  token_t *curr = NULL;
  token_t *next = NULL;

  curr = token_queue_dequeue(que);
  if (curr == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid current token state");
    exit(EXIT_FAILURE);
  }

  switch (curr->kind)
  {
    case KIND_WORD:
      if (1ul < stack->top)
      {
        node = dom_tree_node_stack_pop(stack);
        if (0 != memcmp(node->name, curr->data, curr->size))
        {
          fprintf(stderr, "%s(): %s\n", __func__, "closing tag name does not match open tag name");
          exit(EXIT_FAILURE);
        }
        parent = dom_tree_node_stack_peek(stack);
        if (parent != NULL)
        {
          if (false == dom_tree_node_append(parent, node))
          {
            fprintf(stderr, "%s(): %s\n", __func__, "could not append child node to parent node");
            exit(EXIT_FAILURE);
          }
        }
      }
      break;

    case KIND_FWD_SLASH:
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", curr->kind);
      exit(EXIT_FAILURE);
  }

  next = token_queue_peek(que);
  if (next == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid next token state");
    exit(EXIT_FAILURE);
  }

  switch (next->kind)
  {
    case KIND_WORD:
      if (false == state_queue_enqueue(states, &__parse_elm_close))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_RT_CARET:
      if (false == state_queue_enqueue(states, &__parse_tag_close))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", next->kind);
      exit(EXIT_FAILURE);
  }
}

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
// #include "node.h"
// #include "state.h"
// #include "token.h"

#include <stdio.h>
#include <stdlib.h>

void __parse_tag_open(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

void __parse_tag_close(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  dom_tree_node_t *node = NULL;
  token_t *curr = NULL;
  token_t *next = NULL;

  curr = token_queue_dequeue(que);
  if (curr == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid current token state");
    exit(EXIT_FAILURE);
  }

  switch (curr->kind)
  {
    case KIND_RT_CARET:
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", curr->kind);
      exit(EXIT_FAILURE);
  }

  next = token_queue_peek(que);
  if (next == NULL)
  {
    return;
  }

  switch (next->kind)
  {
    case KIND_LT_CARET:
      if (false == state_queue_enqueue(states, &__parse_tag_open))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", next->kind);
      exit(EXIT_FAILURE);
  }
}

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
// #include "node.h"
// #include "state.h"
// #include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void __parse_attribute_name(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);
void __parse_tag_close(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);
void __parse_doctype(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

void __parse_tag_name(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  dom_tree_node_t *node = NULL;
  token_t *curr = NULL;
  token_t *next = NULL;

  node = dom_tree_node_stack_peek(stack);
  if (node == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "null pointer exception");
    exit(EXIT_FAILURE);
  }

  curr = token_queue_dequeue(que);
  if (curr == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid current token state");
    exit(EXIT_FAILURE);
  }

  switch (curr->kind)
  {
    case KIND_WORD:
      memcpy(node->name, curr->data, curr->size);
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax :: current token", curr->kind);
      exit(EXIT_FAILURE);
  }

  next = token_queue_peek(que);
  if (next == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid next token state");
    exit(EXIT_FAILURE);
  }

  switch (next->kind)
  {
    case KIND_SPACE:
      if (false == state_queue_enqueue(states, &__parse_attribute_name))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_RT_CARET:
      if (false == state_queue_enqueue(states, &__parse_tag_close))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_EXCL:
      if (false == state_queue_enqueue(states, &__parse_doctype))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax :: next token", next->kind);
      exit(EXIT_FAILURE);
  }
}

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
// #include "node.h"
// #include "state.h"
// #include "token.h"

#include <stdio.h>
#include <stdlib.h>

void __parse_tag_name(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);
void __parse_elm_close(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);
void __parse_doctype(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

void __parse_tag_open(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  dom_tree_node_t *node = NULL;
  token_t *curr = NULL;
  token_t *next = NULL;

  node = dom_tree_node_stack_peek(stack);

  curr = token_queue_dequeue(que);
  if (curr == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid current token state");
    exit(EXIT_FAILURE);
  }

  next = token_queue_peek(que);
  if (next == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid next token state");
    exit(EXIT_FAILURE);
  }

  switch (next->kind)
  {
    case KIND_FWD_SLASH:
      if (false == state_queue_enqueue(states, &__parse_elm_close))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      return;

    case KIND_EXCL:
      if (false == state_queue_enqueue(states, &__parse_doctype))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      return;

    default:
      break;
  }

  switch (curr->kind)
  {
    case KIND_LT_CARET:
      node = dom_tree_node_new(NULL, NULL, (1ul << 5));
      if (false == dom_tree_node_stack_push(stack, node))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not push node onto node stack");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_SPACE:
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax :: current token", curr->kind);
      exit(EXIT_FAILURE);
  }

  switch (next->kind)
  {
    case KIND_LT_CARET:
    case KIND_SPACE:
      if (false == state_queue_enqueue(states, &__parse_tag_open))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_WORD:
      if (false == state_queue_enqueue(states, &__parse_tag_name))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax :: next token", next->kind);
      exit(EXIT_FAILURE);
  }
}

static void __parse(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, token_queue_t *que)
{
  state_queue_t *states = NULL;
  parse_state_t state = NULL;
  token_t *tok = NULL;

  states = state_queue_new((1ul << 5));

  if (false == state_queue_enqueue(states, &__parse_tag_open))
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
    exit(EXIT_FAILURE);
  }

  // NOTE: This inner loop is to limit the call stack
  //       depth when scheduling numerous states at once.
  while  (NULL != token_queue_peek(que) &&
          NULL != (state = state_queue_dequeue(states)))
  {
    state(tree, stack, attr_stack, states, que);
  }

  state_queue_destroy(states);

  if (NULL != token_queue_peek(que))
  {
    fprintf(stderr, "%s(): %s\n", __func__, "incomplete");
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief Append the address of a substring in the list buffer. This
 *        is a memoization technique to make for faster traversals.
 *
 */
static void __list_index(list_t *self, char *key)
{
  if (self->size >= 16ul)
  {
    return;
  }
  self->keys[self->size++] = key;
}

/**
 * @brief Append a substring to a list of substrings.
 *
 */
static void __list_append(list_t *self, char *item, size_t len)
{
  __list_index(self, item);

  // NOTE: We must add our own terminating byte as the buffered
  //       bytes terminator's are line-breaks.
  *(item + len) = '\0';
}

/**
 * @brief Search the list items buffer for a substring.
 *
 */
static char *list_get(list_t *self, const uint64_t i)
{
  return self->keys[i];
}

/**
 * @brief Parse substrings from a string using the parameterized delimiter.
 *
 */
static void parse_lines(list_t *list, char *data, const char delim)
{
  char *p = NULL;
  p = data;

  do
  {
    while (*p && *p != '\n')
    {
      p++;
    }

    if (p == data)
    {
      goto next;
    }

    __list_append(list, data, (p - data));

next:
    data = ++p;

  } while (*data);
}

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
static dom_tree_node_t *__dom_tree_get_element_by_name(const dom_tree_node_t *self, const char *name)
{
  if (self == NULL)
  {
    return NULL;
  }

  dom_tree_node_t *node = NULL;
  uint64_t i;

  if (0 == strcmp(self->name, name))
  {
    return (dom_tree_node_t *)self;
  }

  for (i = 0ul; i < self->count; i++)
  {
    if (self->children[i] == NULL)
    {
      continue;
    }

    node = __dom_tree_get_element_by_name(self->children[i], name);
    if (node != NULL)
    {
      break;
    }
  }

  return node;
}

dom_tree_node_t *dom_tree_get_element_by_name(const dom_tree_t *self, const char *name)
{
  return __dom_tree_get_element_by_name(self->root, name);
}

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

struct graph_node
{
  char name[32];
  int weight;
  int vertex;
  struct graph_node *next;
};

typedef struct graph_node graph_node_t;

static graph_node_t *graph_node_new(const char *name, const int vertex, const int weight)
{
  graph_node_t *self = NULL;
  self = (graph_node_t *)calloc(1ul, sizeof(*self));
  strcpy(self->name, name);
  self->weight = weight;
  self->vertex = vertex;
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

struct graph
{
  size_t cap;
  uint64_t count;
  int visited[1u << 5];
  graph_node_t *adj_list[];
};

typedef struct graph graph_t;

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

void graph_add_edge(graph_t *self, graph_node_t *dst, graph_node_t *src)
{
  dst->next = self->adj_list[src->vertex];
  self->adj_list[src->vertex] = dst;

  src->next = self->adj_list[dst->vertex];
  self->adj_list[dst->vertex] = src;
}

graph_node_t *graph_node_create(graph_t *self, const int weight, const char *name)
{
  return graph_node_new(name, self->count++, weight);
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
    node = self->adj_list[*item];

    printf("(%d)%s ", *item, node->name);

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

void dom_tree_BFS(const dom_tree_t *self)
{
  graph_t *graph = NULL;
  graph_node_t *dst = NULL;
  graph_node_t *src = NULL;
  dom_tree_node_queue_t *que = NULL;
  dom_tree_node_t *node = NULL;
  uint64_t i;
  uint64_t depth;
  uint64_t count;

  count = self->root->count;
  depth = 0ul;

  graph = graph_new(1ul << 5);
  que = dom_tree_node_queue_new((1ul << 5));

  if (false == dom_tree_node_queue_enqueue(que, self->root))
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue node into node queue");
    exit(EXIT_FAILURE);
  }

  while (NULL != (node = dom_tree_node_queue_dequeue(que)))
  {
    src = graph_node_create(graph, depth, node->name);

    if (dst == NULL)
    {
      dst = src;
    }
    else
    {
      graph_add_edge(graph, dst, src);
    }

    if (0ul == count)
    {
      count = node->count;
      dst = src;
      depth++;
    }
    count--;

    for (i = 0ul; i < node->count; i++)
    {
      if (NULL == node->children[i])
      {
        continue;
      }

      if (false == dom_tree_node_queue_enqueue(que, node->children[i]))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue node into node queue");
        exit(EXIT_FAILURE);
      }
    }
  }

  graph_BFS(graph, 0);

  dom_tree_node_queue_destroy(que);
  graph_destroy(graph);
}

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
// #include "parse.h"
// #include "tree.h"

#include <stdlib.h>

int main(void)
{
  char data[] = "<!DOCTYPE html>\n<html dir=\"ltr\" lang=\"en-US\">\n  <head></head>\n  <body></body>\n</html>\n";
  dom_tree_t *tree = NULL;
  tree = parse(data);
  dom_tree_BFS(tree);
  dom_tree_destroy(tree);
  return EXIT_SUCCESS;
}
