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
#include "parse.h"

#include <stdlib.h>

int main(void)
{
  char data[] = "<!DOCTYPE html>\n<html dir=\"ltr\" lang=\"en-US\">\n  <head>\n    <body></body>\n  </head>\n</html>\n";
  parse(data);
  return EXIT_SUCCESS;
}
