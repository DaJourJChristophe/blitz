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

typedef void *(*parse_state_t)(token_t *tok);

static void *__parse_tag_open(token_t *tok);

static void *__parse_tag_close(token_t *tok)
{
  switch (tok->kind)
  {
    case KIND_RT_CARET:
      token_print(tok);
      return &__parse_tag_open;

    default:
      break;
  }

  return NULL;
}

static void *__parse_attribute_close(token_t *tok)
{
  switch (tok->kind)
  {
    case KIND_EQUALS:
      token_print(tok);
      return &__parse_tag_open;

    default:
      break;
  }

  return NULL;
}

static void *__parse_attribute_name(token_t *tok);

static void *__parse_attribute_space(token_t *tok)
{
  switch (tok->kind)
  {
    case KIND_SPACE:
      token_print(tok);
      return &__parse_attribute_name;

    case KIND_WORD:
      token_print(tok);
      return __parse_attribute_name(tok);

    case KIND_RT_CARET:
      return __parse_tag_close(tok);

    default:
      break;
  }

  return NULL;
}

static void *__parse_attribute_value(token_t *tok)
{
  switch (tok->kind)
  {
    case KIND_WORD:
      token_print(tok);
      return &__parse_attribute_value;

    case KIND_DASH:
      token_print(tok);
      return &__parse_attribute_value;

    case KIND_DBL_QUOT:
      token_print(tok);
      return &__parse_attribute_space;

    default:
      break;
  }

  return NULL;
}

static void *__parse_attribute_open(token_t *tok)
{
  switch (tok->kind)
  {
    case KIND_DBL_QUOT:
      token_print(tok);
      return &__parse_attribute_value;

    default:
      break;
  }

  return NULL;
}

static void *__parse_attribute_seperator(token_t *tok)
{
  switch (tok->kind)
  {
    case KIND_EQUALS:
      token_print(tok);
      return &__parse_attribute_open;

    default:
      break;
  }

  return NULL;
}

static void *__parse_attribute_name(token_t *tok)
{
  switch (tok->kind)
  {
    case KIND_WORD:
      token_print(tok);
      return &__parse_attribute_seperator;

    default:
      break;
  }

  return NULL;
}

static void *__parse_space(token_t *tok)
{
  switch (tok->kind)
  {
    case KIND_SPACE:
      token_print(tok);
      return &__parse_attribute_name;

    case KIND_RT_CARET:
      token_print(tok);
      return &__parse_tag_open;

    default:
      break;
  }

  return &__parse_tag_close;
}

static void *__parse_doctype(token_t *tok)
{
  switch (tok->kind)
  {
    case KIND_WORD:
      token_print(tok);
      return &__parse_doctype;

    case KIND_SPACE:
      token_print(tok);
      return &__parse_doctype;

    case KIND_RT_CARET:
      token_print(tok);
      return &__parse_tag_open;

    default:
      break;
  }
}

static void *__parse_tag_name(token_t *tok)
{
  switch (tok->kind)
  {
    case KIND_WORD:
      token_print(tok);
      return &__parse_space;

    case KIND_FWD_SLASH:
      token_print(tok);
      return &__parse_tag_name;

    case KIND_EXCL:
      token_print(tok);
      return &__parse_doctype;

    default:
      break;
  }

  return NULL;
}

static void *__parse_tag_open(token_t *tok)
{
  switch (tok->kind)
  {
    case KIND_LT_CARET:
      token_print(tok);
      return &__parse_tag_name;

    case KIND_SPACE:
      token_print(tok);
      return &__parse_tag_open;

    default:
      break;
  }

  return NULL;
}

static void __parse(token_queue_t *que)
{
  parse_state_t state = NULL;
  token_t *tok = NULL;

  state = &__parse_tag_open;

  while (NULL != (tok = token_queue_dequeue(que)))
  {
    if (state == NULL)
    {
      fprintf(stderr, "%s(): %s\n", __func__, "null pointer exception");
      exit(EXIT_FAILURE);
    }

    state = state(tok);
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
