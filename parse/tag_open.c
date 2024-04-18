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
#include "node.h"
#include "state.h"
#include "token.h"

#include <stdio.h>
#include <stdlib.h>

void __parse_tag_name(dom_tree_node_stack_t *stack, state_queue_t *states, token_queue_t *que);
void __parse_elm_close(dom_tree_node_stack_t *stack, state_queue_t *states, token_queue_t *que);
void __parse_doctype(dom_tree_node_stack_t *stack, state_queue_t *states, token_queue_t *que);

void __parse_tag_open(dom_tree_node_stack_t *stack, state_queue_t *states, token_queue_t *que)
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
