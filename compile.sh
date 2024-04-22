#!/bin/bash

set -e

gcc -Isrc -std=c99 -pedantic -ggdb3 -Wall -Wextra -Werror -o bin/main \
  src/html/parse/attr_name.c \
  src/html/parse/attr_value.c \
  src/html/parse/doctype.c \
  src/html/parse/elm_body.c \
  src/html/parse/elm_close.c \
  src/html/parse/tag_close.c \
  src/html/parse/tag_name.c \
  src/html/parse/tag_open.c \
  src/html/attr.c \
  src/html/conv.c \
  src/html/lex.c \
  src/html/node.c \
  src/html/parse.c \
  src/html/query.c \
  src/html/state.c \
  src/html/tree.c \
  src/text/cmpl.c \
  src/text/lex.c \
  src/text/parse.c \
  src/text/tree.c \
  src/blitz.c \
  src/graph.c \
  src/io.c \
  src/token.c
