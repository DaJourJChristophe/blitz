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
#include "token.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token_queue_t *lex(const char *line)
{
  token_queue_t *que = NULL;
  token_t *tok = NULL;
  char buf[32];
  uint32_t i;

  que = token_queue_new((1ul << 5));

  for (; *line; line++)
  {
    tok = token_queue_current(que);

    switch (*line)
    {
      case ' ':
        tok->kind = KIND_SPACE;
        break;

      case '<':
        tok->kind = KIND_LT_CARET;
        break;

      case '>':
        tok->kind = KIND_RT_CARET;
        break;

      case '/':
        tok->kind = KIND_FWD_SLASH;
        break;

      case '=':
        tok->kind = KIND_EQUALS;
        break;

      case '"':
        tok->kind = KIND_DBL_QUOT;
        break;

      case '\'':
        tok->kind = KIND_SNG_QUOT;
        break;

      case '!':
        tok->kind = KIND_EXCL;
        break;

      case '-':
        tok->kind = KIND_DASH;
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

          tok->data = calloc(i, sizeof(*buf));
          tok->size = i;
          tok->kind = KIND_WORD;

          memcpy(tok->data, buf, i);
          break;
        }

        fprintf(stderr, "%s(): %s (%c)\n", __func__, "illegal character", *line);
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
