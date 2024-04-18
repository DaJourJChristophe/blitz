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
#include "lex.h"
#include "parse.h"
#include "state.h"
#include "token.h"

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

static void __parse(token_queue_t *que);

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

void parse(char *data)
{
  token_queue_t *que = NULL;
  list_t list;
  uint64_t i;

  memset(&list, 0, sizeof(list));
  parse_lines(&list, data, '\n');

  for (i = 0ul; i < list.size; i++)
  {
    que = lex(list_get(&list, i));

    __parse(que);

    token_queue_destroy(que);
  }
}

void __parse_elm_close(state_queue_t *states, token_queue_t *que);
void __parse_attribute_value(state_queue_t *states, token_queue_t *que);
void __parse_attribute_name(state_queue_t *states, token_queue_t *que);
void __parse_doctype(state_queue_t *states, token_queue_t *que);
void __parse_tag_close(state_queue_t *states, token_queue_t *que);
void __parse_tag_name(state_queue_t *states, token_queue_t *que);
void __parse_tag_open(state_queue_t *states, token_queue_t *que);

static void __parse(token_queue_t *que)
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
      state(states, que);
  }

  if (NULL != token_queue_peek(que))
  {
    fprintf(stderr, "%s(): %s\n", __func__, "incomplete");
    exit(EXIT_FAILURE);
  }

  state_queue_destroy(states);
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
