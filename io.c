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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

/**
 * @brief Open a file on the disk, determine it's size, and then
 *        read the entire file from the disk into memory.
 */
int readstream(uint8_t *buffer, const char *file_path, const ssize_t buflen, read_callback_t call)
{
  char const mode[] = "rb";
  FILE *fd = NULL;
  long fsize;
  int bytes = 0;

  fd = fopen(file_path, mode);
  if (fd == NULL)
  {
    fprintf(stderr, "fopen() failed to open a file on the disk\n");
    return (-1);
  }

  fseek(fd, 0, SEEK_END);
  if (ferror(fd))
  {
    fprintf(stderr, "fseek() failed\n");

    int r = fclose(fd);
    if (r == EOF)
    {
      fprintf(stderr, "cannot close file handler\n");
    }

    fd = NULL;
    return (-1);
  }

  fsize = ftell(fd);
  if (fsize == (-1))
  {
    fprintf(stderr, "fseek() failed\n");

    int r = fclose(fd);
    if (r == EOF)
    {
      fprintf(stderr, "cannot close file handler\n");
    }

    fd = NULL;
    return (-1);
  }

  rewind(fd);

  long size = 0;

  for (; fsize > (-1); fsize -= buflen)
  {
    memset(buffer, 0, buflen * sizeof(*buffer));
    size = (fsize > buflen) ? buflen : fsize;
    bytes = fread(buffer, 1, size, fd);
    if (bytes != size)
    {
      fprintf(stderr, "fread() failed to read all bytes from file\n");

      int r = fclose(fd);
      if (r == EOF)
      {
        fprintf(stderr, "cannot close file handler\n");
      }

      fd = NULL;
      return -1;
    }

    if (ferror(fd))
    {
      fprintf(stderr, "fread() failed\n");

      int r = fclose(fd);
      if (r == EOF)
      {
        fprintf(stderr, "cannot close file handler\n");
      }

      fd = NULL;
      return (-1);
    }

    call(buffer, size);
  }

  int r = fclose(fd);
  if (r == EOF)
  {
    fprintf(stderr, "cannot close file handler\n");
  }

  fd = NULL;
  return 0;
}

/**
 * @brief Open a file on the disk, determine it's size, and then
 *        read the entire file from the disk into memory.
 */
int readfile(void *buffer, const char *file_path)
{
  char const mode[] = "rb";
  FILE *fd = NULL;
  long fsize;
  int bytes = 0;

  fd = fopen(file_path, mode);
  if (fd == NULL)
  {
    fprintf(stderr, "fopen() failed to open a file on the disk\n");
    return (-1);
  }

  fseek(fd, 0, SEEK_END);
  if (ferror(fd))
  {
    fprintf(stderr, "fseek() failed\n");

    int r = fclose(fd);
    if (r == EOF)
    {
      fprintf(stderr, "cannot close file handler\n");
    }

    fd = NULL;
    return (-1);
  }

  fsize = ftell(fd);
  if (fsize == (-1))
  {
    fprintf(stderr, "fseek() failed\n");

    int r = fclose(fd);
    if (r == EOF)
    {
      fprintf(stderr, "cannot close file handler\n");
    }

    fd = NULL;
    return (-1);
  }

  rewind(fd);

  bytes = fread(buffer, 1, fsize, fd);
  if (bytes != fsize)
  {
    fprintf(stderr, "fread() failed to read all bytes from file\n");

    int r = fclose(fd);
    if (r == EOF)
    {
      fprintf(stderr, "cannot close file handler\n");
    }

    fd = NULL;
    return -1;
  }

  if (ferror(fd))
  {
    fprintf(stderr, "fread() failed\n");

    int r = fclose(fd);
    if (r == EOF)
    {
      fprintf(stderr, "cannot close file handler\n");
    }

    fd = NULL;
    return (-1);
  }

  int r = fclose(fd);
  if (r == EOF)
  {
    fprintf(stderr, "cannot close file handler\n");
  }

  fd = NULL;
  return bytes;
}

