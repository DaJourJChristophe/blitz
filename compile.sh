#!/bin/bash

set -e

gcc -o bin/main lex.c main.c parse.c token.c
