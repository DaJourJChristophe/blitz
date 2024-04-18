#!/bin/bash

set -e

gcc -ggdb3 -o bin/main lex.c main.c parse.c token.c
