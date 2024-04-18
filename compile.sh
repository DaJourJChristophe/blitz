#!/bin/bash

set -e

gcc -I. -ggdb3 -o bin/main \
  parse/attr_name.c \
  parse/attr_value.c \
  parse/doctype.c \
  parse/elm_close.c \
  parse/tag_close.c \
  parse/tag_name.c \
  parse/tag_open.c \
  lex.c \
  main.c \
  node.c \
  parse.c \
  state.c \
  token.c \
  tree.c
