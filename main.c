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
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct list
{
  size_t size;
  size_t len;
  char *items;
};

typedef struct list list_t;

static void list_append(list_t *self, const char *item, size_t len)
{
  len += 1ul;

  if (NULL == self->items)
  {
    self->items = (char *)malloc(len * sizeof(*self->items));
  }
  else
  {
    void *__old = NULL;
    __old = self->items;
    self->items = NULL;
    self->items = (char *)realloc(__old, ((len + self->len) * sizeof(*self->items)));
  }

  if (NULL == self->items)
  {
    fprintf(stderr, "%s(): %s\n", __func__, "memory error");
    exit(EXIT_FAILURE);
  }

  // NOTE: Subtract one from the length as not to copy the
  //       terminating byte from the item.
  memcpy((self->items + self->len), item, (len - 1ul));

  // NOTE: We must add our own terminating byte as the buffered
  //       bytes terminator's are line-breaks.
  *(self->items + (len + self->len)) = '\0';

  self->len += len;
  self->size++;
}

static char *list_get(list_t *self, const uint64_t i)
{
  uint64_t j;
  uint64_t k;

  for (j = 0ul, k = 0ul; j < i; j++)
  {
    while (self->items[k])
    {
      k++;
    }
    k++;
  }

  return (self->items + k);
}

static void parse_lines(const char *data)
{
  list_t list;
  uint64_t i;

  i = 0ul;
  memset(&list, 0, sizeof(list));

  for (; *data; data++)
  {
    if ('\n' == *data)
    {
      list_append(&list, (data - i), i);
      i = 0ul;
      continue;
    }
    i++;
  }

  for (i = 0ul; i < list.size; i++)
  {
    printf("%s\n", list_get(&list, i));
  }
}

int main(void)
{
  const char data[] = "<!DOCTYPE html>\n<html>\n  <head>\n    <body></body>\n  </head>\n</html>\n";
  parse_lines(data);
  return EXIT_SUCCESS;
}
