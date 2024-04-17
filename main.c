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
#include <ctype.h>
#include <stdbool.h>
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
};

struct token
{
  int kind;
  void *data;
  size_t size;
};

typedef struct token token_t;

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

    default:
      fprintf(stderr, "%s(): %s\n", __func__, "unknown token.kind");
      exit(EXIT_FAILURE);
  }
}

struct token_queue
{
  size_t cap;
  uint64_t r;
  uint64_t w;
  token_t toks[];
};

typedef struct token_queue token_queue_t;

void token_queue_setup(token_queue_t *self, const size_t size, const size_t cap)
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

token_queue_t *lex(const char *line)
{
  token_queue_t *que = NULL;
  token_t tok;
  char buf[32];
  uint32_t i;

  que = token_queue_new((1ul << 5));

  for (; *line; line++)
  {
    switch (*line)
    {
      case ' ':
        tok.kind = KIND_SPACE;
        break;

      case '<':
        tok.kind = KIND_LT_CARET;
        break;

      case '>':
        tok.kind = KIND_RT_CARET;
        break;

      case '/':
        tok.kind = KIND_FWD_SLASH;
        break;

      case '=':
        tok.kind = KIND_EQUALS;
        break;

      case '"':
        tok.kind = KIND_DBL_QUOT;
        break;

      case '\'':
        tok.kind = KIND_SNG_QUOT;
        break;

      case '!':
        tok.kind = KIND_EXCL;
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

          tok.data = calloc(i, sizeof(*buf));
          tok.size = i;
          tok.kind = KIND_WORD;

          memcpy(tok.data, buf, i);
          break;
        }

        fprintf(stderr, "%s(): %s (%c)\n", __func__, "illegal character", *line);
        exit(EXIT_FAILURE);
    }

    if (false == token_queue_enqueue(que, &tok))
    {
      fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue token into token queue");
      exit(EXIT_FAILURE);
    }
  }

  return que;
}

void parse(char *data)
{
  token_queue_t *que = NULL;
  token_t *tok = NULL;
  list_t list;
  uint64_t i;

  memset(&list, 0, sizeof(list));
  parse_lines(&list, data, '\n');

  for (i = 0ul; i < list.size; i++)
  {
    que = lex(list_get(&list, i));

    while (NULL != (tok = token_queue_dequeue(que)))
    {
      token_print(tok);
    }

    token_queue_destroy(que);
  }
}

int main(void)
{
  char data[] = "<!DOCTYPE html>\n<html>\n  <head>\n    <body></body>\n  </head>\n</html>\n";
  parse(data);
  return EXIT_SUCCESS;
}
