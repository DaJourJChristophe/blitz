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
#include <sys/types.h>

#define MAX_WORD_BUF 64

token_queue_t *lex(uint8_t **line, const ssize_t size, int64_t *j)
{
  token_queue_t *que = NULL;
  token_t *tok = NULL;
  uint8_t buf[MAX_WORD_BUF];
  uint32_t i;

  que = token_queue_new(TOKEN_QUEUE_CAPACITY);

  for (; *j < size && **line; (*line)++, *j += 1)
  {
    tok = token_queue_current(que);
    if (tok == NULL)
    {
      return que;
    }

    switch (**line)
    {
      case '[':
        tok->kind = KIND_OPEN_SQUARE_BRACKET;
        break;

      case ']':
        tok->kind = KIND_CLOSE_SQUARE_BRACKET;
        break;

      case '(':
        tok->kind = KIND_OPEN_PARENTHESIS;
        break;

      case ')':
        tok->kind = KIND_CLOSE_PARENTHESIS;
        break;

      case '.':
        tok->kind = KIND_PERIOD;
        break;

      case ':':
        tok->kind = KIND_COLON;
        break;

      case ';':
        tok->kind = KIND_SEMI_COLON;
        break;

      case ',':
        tok->kind = KIND_COMMA;
        break;

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
        if (isalpha(**line))
        {
          memset(buf, 0, MAX_WORD_BUF * sizeof(*buf));
          for (i = 0u; *j < size && i < (MAX_WORD_BUF - 1) && **line && isalpha(**line); (*line)++, i++, *j += 1)
          {
            buf[i] = **line;
          }
          if (i >= (MAX_WORD_BUF - 1) && **line && isalpha(**line))
          {
            fprintf(stderr, "%s(): %s\n", __func__, "could not write to word buffer, buffer full");
            exit(EXIT_FAILURE);
          }
          (*line)--; *j -= 1;

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

        fprintf(stderr, "%s(): %s (%c / 0x%x)\n", __func__, "illegal character", **line, **line);
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
