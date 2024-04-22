#!/bin/bash

set -e

gcc -I. -std=c99 -pedantic -ggdb3 -Wall -Wextra -Werror -o bin/main \
  html/parse/attr_name.c \
  html/parse/attr_value.c \
  html/parse/doctype.c \
  html/parse/elm_body.c \
  html/parse/elm_close.c \
  html/parse/tag_close.c \
  html/parse/tag_name.c \
  html/parse/tag_open.c \
  html/attr.c \
  html/conv.c \
  html/lex.c \
  html/node.c \
  html/parse.c \
  html/query.c \
  html/state.c \
  html/tree.c \
  text/cmpl.c \
  text/lex.c \
  text/parse.c \
  text/tree.c \
  graph.c \
  io.c \
  main.c \
  token.c
