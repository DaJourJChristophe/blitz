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
  char *keys[16];
};

typedef struct list list_t;

/**
 * @brief Append the address of a substring in the list buffer. This
 *        is a memoization technique to make for faster traversals.
 *
 */
static void __list_index(list_t *self, char *key)
{
  if (self->size >= 16ul)
  {
    return;
  }
  self->keys[self->size++] = key;
}

/**
 * @brief Append a substring to a list of substrings.
 *
 */
static void __list_append(list_t *self, char *item, size_t len)
{
  __list_index(self, item);

  // NOTE: We must add our own terminating byte as the buffered
  //       bytes terminator's are line-breaks.
  *(item + len) = '\0';
}

/**
 * @brief Search the list items buffer for a substring.
 *
 */
static char *list_get(list_t *self, const uint64_t i)
{
  return self->keys[i];
}

/**
 * @brief Parse substrings from a string using the parameterized delimiter.
 *
 */
static void parse_lines(list_t *list, char *data, const char delim)
{
  uint64_t i;

  for (i = 0ul; *data; data++)
  {
    if (delim == *data)
    {
      if (0ul == i)
      {
        continue;
      }

      __list_append(list, (data - i), i);
      i = 0ul;
      continue;
    }

    i++;
  }

  if (0ul == i)
  {
    return;
  }

  __list_append(list, (data - i), i);
}

int main(void)
{
  char data[] = "<!DOCTYPE html>\n<html>\n  <head>\n    <body></body>\n  </head>\n</html>\n";
  list_t list;
  char *line = NULL;
  uint64_t i;

  memset(&list, 0, sizeof(list));
  parse_lines(&list, data, '\n');

  for (i = 0ul; i < list.size; i++)
  {
    line = list_get(&list, i);
    printf("%s\n", line);
  }

  return EXIT_SUCCESS;
}
