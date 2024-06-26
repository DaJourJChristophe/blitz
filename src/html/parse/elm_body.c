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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token_queue_t *vqueue = NULL;

int __parse_tag_open(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

static int __parse_next_elm_body(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que);

static int __parse_elm_body_lim_state_2(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  token_t *curr = NULL;
  curr = token_queue_dequeue(vqueue);

  switch (curr->kind)
  {
    case KIND_DASH:
    case KIND_WORD:
    case KIND_NUMBER:
      __parse_elm_body_lim_state_2(tree, stack, attr_stack, states, que);
      break;

    case KIND_RT_CARET:
      if (false == state_queue_enqueue_back(states, &__parse_tag_open))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", curr->kind);
      exit(EXIT_FAILURE);
  }
}

static int __parse_elm_body_lim_state_1(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  token_t *curr = NULL;
  curr = token_queue_dequeue(vqueue);

  switch (curr->kind)
  {
    case KIND_LT_CARET:
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", curr->kind);
      exit(EXIT_FAILURE);
  }
}

static int __parse_elm_body_lim(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  token_t *curr = NULL;
  token_t *next = NULL;

  curr = token_queue_dequeue(que);
  if (curr == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid current token state");
    exit(EXIT_FAILURE);
  }

  next = token_queue_peek(que);

  switch (curr->kind)
  {
    case KIND_LT_CARET:
    case KIND_WORD:
    case KIND_NUMBER:
      if (false == token_queue_enqueue(vqueue, curr))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue token into vqueue");
        exit(EXIT_FAILURE);
      }
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", curr->kind);
      exit(EXIT_FAILURE);
  }

  if (next == NULL)
  {
    if (false == state_queue_enqueue_front(states, &__parse_next_elm_body_lim))
    {
      fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
      exit(EXIT_FAILURE);
    }
    return 1;
  }

  switch (next->kind)
  {
    case KIND_RT_CARET:
      __parse_elm_body_lim_state_1(tree, stack, attr_stack, states, que);
      break;

    case KIND_WORD:
    case KIND_NUMBER:
      if (false == state_queue_enqueue_back(states, &__parse_elm_body_lim))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_QMARK:
    case KIND_LT_CARET:
      if (false == state_queue_enqueue_back(states, &__parse_elm_body))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", curr->kind);
      exit(EXIT_FAILURE);
  }

  return 0;
}

static int __parse_next_elm_body_lim(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  token_t *next = NULL;

  next = token_queue_peek(que);
  if (next == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "invalid next token state");
    exit(EXIT_FAILURE);
  }

  switch (next->kind)
  {
    case KIND_LT_CARET:
    case KIND_RT_CURLY_BRACKET:
    case KIND_LT_CURLY_BRACKET:
    case KIND_CARET:
    case KIND_VBAR:
    case KIND_UNDERSCORE:
    case KIND_FWD_SLASH:
    case KIND_AMP:
    case KIND_EQUALS:
    case KIND_SNG_QUOT:
    case KIND_PLUS:
    case KIND_QMARK:
    case KIND_WORD:
    case KIND_NUMBER:
    case KIND_LT_CARET:
      if (false == state_queue_enqueue_back(states, &__parse_elm_body))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", curr->kind);
      exit(EXIT_FAILURE);
  }

  return 0;
}

int __parse_elm_body(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
{
  dom_tree_node_t *node = NULL;
  token_t *curr = NULL;
  token_t *next = NULL;

  (void)tree;
  (void)attr_stack;

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

  next = token_queue_peek(que);

  switch (curr->kind)
  {
    case KIND_WORD:
    case KIND_NUMBER:
      if (false == dom_tree_node_append_body(node, curr->data, curr->size - 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_QMARK:
      if (false == dom_tree_node_append_body(node, "?", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_PLUS:
      if (false == dom_tree_node_append_body(node, "+", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_SNG_QUOT:
      if (false == dom_tree_node_append_body(node, "'", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_EQUALS:
      if (false == dom_tree_node_append_body(node, "=", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_AMP:
      if (false == dom_tree_node_append_body(node, "&", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_FWD_SLASH:
      if (false == dom_tree_node_append_body(node, "/", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_UNDERSCORE:
      if (false == dom_tree_node_append_body(node, "_", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_VBAR:
      if (false == dom_tree_node_append_body(node, "|", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_CARET:
      if (false == dom_tree_node_append_body(node, "^", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_LT_CURLY_BRACKET:
      if (false == dom_tree_node_append_body(node, "{", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_RT_CURLY_BRACKET:
      if (false == dom_tree_node_append_body(node, "}", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_LT_CARET:
      if (false == dom_tree_node_append_body(node, "<", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_RT_CARET:
      if (false == dom_tree_node_append_body(node, ">", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_OPEN_PARENTHESIS:
      if (false == dom_tree_node_append_body(node, "(", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_CLOSE_PARENTHESIS:
      if (false == dom_tree_node_append_body(node, ")", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_OPEN_SQUARE_BRACKET:
      if (false == dom_tree_node_append_body(node, "[", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_CLOSE_SQUARE_BRACKET:
      if (false == dom_tree_node_append_body(node, "]", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_PERIOD:
      if (false == dom_tree_node_append_body(node, ".", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_SEMI_COLON:
      if (false == dom_tree_node_append_body(node, ";", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_DBL_QUOT:
      if (false == dom_tree_node_append_body(node, "\"", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_DASH:
      if (false == dom_tree_node_append_body(node, "-", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_COLON:
      if (false == dom_tree_node_append_body(node, ":", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_COMMA:
      if (false == dom_tree_node_append_body(node, ",", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_SPACE:
      if (false == dom_tree_node_append_body(node, " ", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_EXCL:
      if (false == dom_tree_node_append_body(node, "!", 1ul))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not write into node body");
        exit(EXIT_FAILURE);
      }
      break;

    default:
      fprintf(stderr, "%s(): %s (%d)\n", __func__, "invalid syntax", curr->kind);
      exit(EXIT_FAILURE);
  }

  if (next == NULL)
  {
    if (false == state_queue_enqueue_front(states, &__parse_next_elm_body))
    {
      fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
      exit(EXIT_FAILURE);
    }
    return 1;
  }

  switch (next->kind)
  {
    case KIND_PLUS:
    case KIND_FWD_SLASH:
    case KIND_CARET:
    case KIND_AMP:
    case KIND_VBAR:
    case KIND_QMARK:
    case KIND_NUMBER:
    case KIND_SNG_QUOT:
    case KIND_EQUALS:
    case KIND_UNDERSCORE:
    case KIND_LT_CURLY_BRACKET:
    case KIND_RT_CURLY_BRACKET:
    case KIND_OPEN_PARENTHESIS:
    case KIND_CLOSE_PARENTHESIS:
    case KIND_OPEN_SQUARE_BRACKET:
    case KIND_CLOSE_SQUARE_BRACKET:
    case KIND_PERIOD:
    case KIND_SEMI_COLON:
    case KIND_DBL_QUOT:
    case KIND_DASH:
    case KIND_COLON:
    case KIND_WORD:
    case KIND_COMMA:
    case KIND_SPACE:
    case KIND_EXCL:
      if (false == state_queue_enqueue_back(states, &__parse_elm_body))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_LT_CARET:
      vqueue = token_queue_new(TOKEN_QUEUE_CAPACITY);
      if (false == state_queue_enqueue_back(states, &__parse_elm_body_lim))
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

static int __parse_next_elm_body(dom_tree_t *tree, dom_tree_node_stack_t *stack, dom_tree_node_attr_stack_t *attr_stack, state_queue_t *states, token_queue_t *que)
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
    case KIND_PLUS:
    case KIND_FWD_SLASH:
    case KIND_CARET:
    case KIND_AMP:
    case KIND_VBAR:
    case KIND_QMARK:
    case KIND_NUMBER:
    case KIND_SNG_QUOT:
    case KIND_EQUALS:
    case KIND_UNDERSCORE:
    case KIND_LT_CURLY_BRACKET:
    case KIND_RT_CURLY_BRACKET:
    case KIND_OPEN_PARENTHESIS:
    case KIND_CLOSE_PARENTHESIS:
    case KIND_OPEN_SQUARE_BRACKET:
    case KIND_CLOSE_SQUARE_BRACKET:
    case KIND_PERIOD:
    case KIND_SEMI_COLON:
    case KIND_DBL_QUOT:
    case KIND_DASH:
    case KIND_COLON:
    case KIND_WORD:
    case KIND_COMMA:
    case KIND_SPACE:
    case KIND_EXCL:
      if (false == state_queue_enqueue_back(states, &__parse_elm_body))
      {
        fprintf(stderr, "%s(): %s\n", __func__, "could not enqueue into state queue");
        exit(EXIT_FAILURE);
      }
      break;

    case KIND_LT_CARET:
      if (false == state_queue_enqueue_back(states, &__parse_tag_open))
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
