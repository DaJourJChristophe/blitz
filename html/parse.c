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
#include "io.h"
#include "lex.h"
#include "node.h"
#include "parse.h"
#include "state.h"
#include "token.h"
#include "tree.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define STRING_LIST_CAPACITY 64ul

struct list
{
  size_t size;
  size_t len;
  uint8_t *keys[STRING_LIST_CAPACITY];
};

typedef struct list list_t;

static void __parse(dom_tree_t *tree, dom_tree_node_stack_t *stack,
  dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states,
  token_queue_t *que);

/**
 * @brief Parse substrings from a string using the parameterized delimiter.
 *
 */
static void parse_lines(list_t *list, uint8_t *data, const ssize_t size, const char delim);

/**
 * @brief Search the list items buffer for a substring.
 *
 */
static uint8_t *list_get(list_t *self, const uint64_t i);

int __parse_tag_open(dom_tree_t *tree, dom_tree_node_stack_t *stack,
  dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states,
  token_queue_t *que);

#define MAXBUF ((1u << 12) - 1u)

dom_tree_t *tree = NULL;
dom_tree_node_stack_t *stack = NULL;
dom_tree_node_attr_stack_t *attr_stack = NULL;
state_queue_t *states = NULL;
token_queue_t *que = NULL;

static void __html_parse_file(uint8_t *data, const ssize_t size)
{
  (void)size;
  const char delim = '\n';
  list_t list;
  uint8_t *line = NULL;
  uint64_t i;
  int64_t j;

  j = 0;

  memset(&list, 0, sizeof(list));

  parse_lines(&list, data, size, delim);

  for (i = 0ul; i < list.size; i++)
  {
    line = list_get(&list, i);

    while (j < size && *line)
    {
      que = lex(&line, size, &j);

      __parse(tree, stack, attr_stack, states, que);

      token_queue_destroy(que);
    }
  }
}

dom_tree_t *html_parse_file(const char *filepath)
{
  uint8_t data[MAXBUF];
  int n = 0;

  tree = dom_tree_new();
  stack = dom_tree_node_stack_new(DOM_TREE_NODE_STACK_CAPACITY);
  attr_stack = dom_tree_node_attr_stack_new(DOM_TREE_NODE_ATTR_STACK_CAPACITY);
  states = state_queue_new(STATE_QUEUE_CAPACITY);

  memset(data, 0, MAXBUF * sizeof(*data));

  if (false == state_queue_enqueue_back(states, &__parse_tag_open))
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
    exit(EXIT_FAILURE);
  }

  n = readstream((void *)data, filepath, MAXBUF, &__html_parse_file);
  if (n < 0)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not read from file");
    exit(EXIT_FAILURE);
  }

  if (1ul != stack->top)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "incomplete");
    exit(EXIT_FAILURE);
  }

  state_queue_destroy(states);
  dom_tree_node_attr_stack_destroy(attr_stack);

  tree->root = dom_tree_node_stack_pop(stack);
  dom_tree_node_stack_destroy(stack);

  return tree;
}

dom_tree_t *html_parse(void *data, const ssize_t size)
{
  const char delim = '\n';
  dom_tree_t *tree = NULL;
  dom_tree_node_stack_t *stack = NULL;
  dom_tree_node_attr_stack_t *attr_stack = NULL;
  state_queue_t *states = NULL;
  token_queue_t *que = NULL;
  list_t list;
  uint8_t *line = NULL;
  uint64_t i;
  int64_t j;

  j = 0;

  tree = dom_tree_new();
  stack = dom_tree_node_stack_new(DOM_TREE_NODE_STACK_CAPACITY);
  attr_stack = dom_tree_node_attr_stack_new(DOM_TREE_NODE_ATTR_STACK_CAPACITY);
  states = state_queue_new(STATE_QUEUE_CAPACITY);

  memset(&list, 0, sizeof(list));

  if (false == state_queue_enqueue_back(states, &__parse_tag_open))
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
    exit(EXIT_FAILURE);
  }

  parse_lines(&list, data, size, delim);

  for (i = 0ul; i < list.size; i++)
  {
    line = list_get(&list, i);

    while (*line != '\0')
    {
      que = lex(&line, size, &j);

      __parse(tree, stack, attr_stack, states, que);

      token_queue_destroy(que);
    }
  }

  state_queue_destroy(states);
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

static void __parse(dom_tree_t *tree, dom_tree_node_stack_t *stack,
  dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states,
  token_queue_t *que)
{
  parse_state_t state = NULL;

  // NOTE: This inner loop is to limit the call stack
  //       depth when scheduling numerous states at once.
  while  (NULL != token_queue_peek(que) &&
          NULL != (state = state_queue_dequeue(states)))
  {
    if (state(tree, stack, attr_stack, states, que))
    {
      break;
    }
  }
}

/**
 * @brief Append the address of a substring in the list buffer. This
 *        is a memoization technique to make for faster traversals.
 *
 */
static bool __list_index(list_t *self, uint8_t *key)
{
  if (self->size >= STRING_LIST_CAPACITY)
  {
    return false;
  }
  self->keys[self->size++] = key;
  return true;
}

/**
 * @brief Append a substring to a list of substrings.
 *
 */
static bool __list_append(list_t *self, uint8_t *item, size_t len)
{
  if (false == __list_index(self, item))
  {
    return false;
  }

  // NOTE: We must add our own terminating byte as the buffered
  //       bytes terminator's are line-breaks.
  *(item + len) = '\0';

  return true;
}

/**
 * @brief Search the list items buffer for a substring.
 *
 */
static uint8_t *list_get(list_t *self, const uint64_t i)
{
  return self->keys[i];
}

/**
 * @brief Parse substrings from a string using the parameterized delimiter.
 *
 */
static void parse_lines(list_t *list, uint8_t *data, const ssize_t size, const char delim)
{
  uint8_t *p = NULL;
  int64_t i;

  p = data;
  i = 0;

  do
  {
    while (i < size && *p && *p != delim)
    {
      p++; i++;
    }

    if (p == data)
    {
      goto next;
    }

    if (false == __list_append(list, data, (p - data)))
    {
      fprintf(stderr, "%s(): %s\n", __func__, "could not append string to list");
      exit(EXIT_FAILURE);
    }

next:
    data = ++p; i++;

  } while (i < size && *data);
}
