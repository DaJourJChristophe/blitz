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
#include "token.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token_queue_t *lex(const char *data)
{
  token_queue_t *que = NULL;
  token_t *tok = NULL;
  char buf[32];
  uint32_t i;

  que = token_queue_new((1ul << 12));

  for (; *data; data++)
  {
    tok = token_queue_current(que);

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
          for (i = 0u; *data && (*data == '-' || (!ispunct(*data) && !isspace(*data) && !iscntrl(*data))); data++)
          {
            buf[i++] = *data;
          }
          buf[i++] = '\0';
          data--;

          tok->data = calloc(i, sizeof(*buf));
          tok->size = i;
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

void parse2(token_queue_t *que)
{
  token_t *tok = NULL;

  while (NULL != (tok = token_queue_dequeue(que)))
  {
    switch (tok->kind)
    {
      case KIND_SPACE:
        break;

      case KIND_WORD:
        token_print(tok); printf("\n");
        break;

      case KIND_PERIOD:
      case KIND_EXCL:
      case KIND_COMMA:
      case KIND_COLON:
      case KIND_SEMI_COLON:
        fprintf(stderr, "%s(): %s(%d)\n", __func__, "illegal token.kind", tok->kind);
        exit(EXIT_FAILURE);

      default:
        fprintf(stderr, "%s(): %s(%d)\n", __func__, "unknown token.kind", tok->kind);
        exit(EXIT_FAILURE);
    }
  }

  printf("\n\n");
}

void parse(token_queue_t *que)
{
  token_queue_t *new_que = NULL;
  token_t *tok = NULL;

  new_que = token_queue_new((1ul << 12));

  while (NULL != (tok = token_queue_dequeue(que)))
  {
    switch (tok->kind)
    {
      case KIND_SPACE:
      case KIND_WORD:
        token_queue_enqueue(new_que, tok);
        break;

      case KIND_PERIOD:
      case KIND_EXCL:
      case KIND_COMMA:
      case KIND_COLON:
      case KIND_SEMI_COLON:
        parse2(new_que);
        new_que = token_queue_new((1ul << 12));
        break;

      default:
        fprintf(stderr, "%s(): %s(%d)\n", __func__, "unknown token.kind", tok->kind);
        exit(EXIT_FAILURE);
    }
  }

  token_queue_destroy(que);
}

int main(void)
{
  char data[(1u << 16)];
  token_queue_t *que = NULL;
  memset(data, 0, (1u << 16) * sizeof(*data));
  if (readfile(data, "data.txt") < 0)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "could not read from file");
    exit(EXIT_FAILURE);
  }
  que = lex(data);
  parse(que);
  return EXIT_SUCCESS;
}
