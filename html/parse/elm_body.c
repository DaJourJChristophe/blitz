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

void __parse_tag_open(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

void __parse_elm_body(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
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
      strncat(node->body, curr->data, curr->size);
      break;

    case KIND_COMMA:
      strncat(node->body, ",", 2ul);
      break;

    case KIND_SPACE:
      strncat(node->body, " ", 2ul);
      break;

    case KIND_EXCL:
      strncat(node->body, "!", 2ul);
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
    case KIND_COMMA:
    case KIND_SPACE:
    case KIND_EXCL:
      if (false == state_queue_enqueue(states, &__parse_elm_body))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

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
