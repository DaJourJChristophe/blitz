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
#include "tree.h"
#include "io.h"
#include "token.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_BUF 64

token_queue_t *text_lex(const char *data)
{
  if (data == NULL)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "null pointer exception");
    exit(EXIT_FAILURE);
  }

  token_queue_t *que = NULL;
  token_t *tok = NULL;
  char buf[MAX_WORD_BUF];
  uint32_t i;

  que = token_queue_new(TOKEN_QUEUE_CAPACITY);

  for (; *data; data++)
  {
    tok = token_queue_current(que);
    if (tok == NULL)
    {
      return que;
    }

    switch (*data)
    {
      case '"':
      case '(':
      case ')':
      case '[':
      case ']':
      case '\n':
        break;

      case ' ':
        tok->kind = KIND_SPACE;
        break;

      case '.':
        tok->kind = KIND_PERIOD;
        break;

      case '!':
        tok->kind = KIND_EXCL;
        break;

      case ',':
        tok->kind = KIND_COMMA;
        break;

      case ':':
        tok->kind = KIND_COLON;
        break;

      case ';':
        tok->kind = KIND_SEMI_COLON;
        break;

      default:
        if (*data == '-' || (!ispunct(*data) && !isspace(*data) && !iscntrl(*data)))
        {
          for (i = 0u; i < (MAX_WORD_BUF - 1) && *data && (*data == '-' || (!ispunct(*data) && !isspace(*data) && !iscntrl(*data))); data++)
          {
            buf[i++] = *data;
          }
          if (i >= (MAX_WORD_BUF - 1) && *data && (*data == '-' || (!ispunct(*data) && !isspace(*data) && !iscntrl(*data))))
          {
            fprintf(stderr, "%s(): %s\n", __func__, "could not write to word buffer, buffer full");
            exit(EXIT_FAILURE);
          }
          data--;

          tok->data = calloc(1u+i, sizeof(*buf));
          if (tok->data == NULL)
          {
            fprintf(stderr, "%s(): %s\n", __func__, "memory error");
            exit(EXIT_FAILURE);
          }
          tok->size = 1u+i;
          tok->kind = KIND_WORD;

          memcpy(tok->data, buf, i);
          break;
        }

        fprintf(stderr, "%s(): %s (%c)\n", __func__, "illegal character", *data);
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
