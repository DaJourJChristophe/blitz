/**
 * @file io.c
 * @author Da'Jour J. Christophe (dajour.christophe@gmail.com)
 * @brief Define useful into output operations.
 * @version 0.1
 * @date 2023-04-08
 *
 * @copyright Copyright (c) 2023 Da'Jour J. Christophe. All rights reserved.
 */
#include <stdio.h>

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

