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
#include "html/node.h"
#include "html/state.h"
#include "html/tree.h"
#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int __parse_attribute_name(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

static int __parse_next_attribute_value(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

int __parse_attribute_value(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  dom_tree_node_attr_t *attr = NULL;
  token_t *curr = NULL;
  token_t *next = NULL;

  (void)tree;
  (void)stack;

  attr = dom_tree_node_attr_stack_peek(attr_stack);
  if (attr == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid syntax");
    exit(EXIT_FAILURE);
  }

  curr = token_queue_dequeue(que);
  if (curr == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid current token state");
    exit(EXIT_FAILURE);
  }

  next = token_queue_peek(que);

  switch (curr->kind)
  {
    case KIND_COLON:
      if (false == dom_tree_node_attr_append_value(attr, ":", 2ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_DASH:
      if (false == dom_tree_node_attr_append_value(attr, "-", 2ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_PERIOD:
      if (false == dom_tree_node_attr_append_value(attr, ".", 2ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_FWD_SLASH:
      if (false == dom_tree_node_attr_append_value(attr, "/", 2ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_UNDERSCORE:
      if (false == dom_tree_node_attr_append_value(attr, "/", 2ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_WORD:
      if (false == dom_tree_node_attr_append_value(attr, curr->data, curr->size))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_NUMBER:
      if (false == dom_tree_node_attr_append_value(attr, curr->data, curr->size))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_DBL_QUOT:
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax :: current token", curr->kind);
      exit(EXIT_FAILURE);
  }

  if (next == NULL)
  {
    if (false == state_queue_enqueue_front(states, &__parse_next_attribute_value))
    {
      fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
      exit(EXIT_FAILURE);
    }
    return 1;
  }

  switch (next->kind)
  {
    case KIND_UNDERSCORE:
    case KIND_PERIOD:
    case KIND_FWD_SLASH:
    case KIND_COLON:
    case KIND_WORD:
    case KIND_DASH:
    case KIND_NUMBER:
      if (false == state_queue_enqueue_back(states, &__parse_attribute_value))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_DBL_QUOT:
      dom_tree_node_attr_stack_pop(attr_stack);
      if (false == state_queue_enqueue_back(states, &__parse_attribute_name))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax :: next token", next->kind);
      exit(EXIT_FAILURE);
  }

  return 0;
}

static int __parse_next_attribute_value(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  token_t *next = NULL;

  (void)tree;
  (void)stack;
  (void)attr_stack;

  next = token_queue_peek(que);

  if (next == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid next token state");
    exit(EXIT_FAILURE);
  }

  switch (next->kind)
  {
    case KIND_UNDERSCORE:
    case KIND_PERIOD:
    case KIND_FWD_SLASH:
    case KIND_COLON:
    case KIND_WORD:
    case KIND_DASH:
    case KIND_NUMBER:
      if (false == state_queue_enqueue_back(states, &__parse_attribute_value))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_DBL_QUOT:
      dom_tree_node_attr_stack_pop(attr_stack);
      if (false == state_queue_enqueue_back(states, &__parse_attribute_name))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", next->kind);
      exit(EXIT_FAILURE);
  }

  return 0;
}
