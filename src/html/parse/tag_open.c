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

int __parse_tag_name(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);
int __parse_elm_close(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);
int __parse_doctype(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

static int __parse_next_tag_open(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

int __parse_tag_open(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  token_t *curr = NULL;
  token_t *next = NULL;

  (void)tree;
  (void)attr_stack;

  curr = token_queue_dequeue(que);
  if (curr == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid current token state");
    exit(EXIT_FAILURE);
  }

  next = token_queue_peek(que);
  if (next == NULL)
  {
    goto skip;
  }

  switch (next->kind)
  {
    case KIND_FWD_SLASH:
      if (false == state_queue_enqueue_back(states, &__parse_elm_close))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      return 0;

    case KIND_EXCL:
      if (false == state_queue_enqueue_back(states, &__parse_doctype))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      return 0;

    default:
      break;
  }

skip:
  switch (curr->kind)
  {
    case KIND_LT_CARET:
      if (false == dom_tree_node_stack_push(stack, dom_tree_node_new(NULL, NULL, DOM_TREE_NODE_DEFAULT_CAPACITY)))
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

  if (next == NULL)
  {
    if (false == state_queue_enqueue_front(states, &__parse_next_tag_open))
    {
      fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
      exit(EXIT_FAILURE);
    }
    return 1;
  }

  switch (next->kind)
  {
    case KIND_LT_CARET:
    case KIND_SPACE:
      if (false == state_queue_enqueue_back(states, &__parse_tag_open))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_WORD:
      if (false == state_queue_enqueue_back(states, &__parse_tag_name))
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

static int __parse_next_tag_open(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
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
    case KIND_FWD_SLASH:
      if (false == state_queue_enqueue_back(states, &__parse_elm_close))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_EXCL:
      if (false == state_queue_enqueue_back(states, &__parse_doctype))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_LT_CARET:
    case KIND_SPACE:
      if (false == state_queue_enqueue_back(states, &__parse_tag_open))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_WORD:
      if (false == state_queue_enqueue_back(states, &__parse_tag_name))
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
