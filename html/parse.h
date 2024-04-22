#ifndef PARSE_H
#define PARSE_H

#include "tree.h"

#include <sys/types.h>

dom_tree_t *html_parse_file(const char *filepath);

dom_tree_t *html_parse(void *data, const ssize_t size);

#endif/*PARSE_H*/
