#ifndef LEX_H
#define LEX_H

#include "token.h"

#include <stdint.h>

token_queue_t *lex(uint8_t **line, const ssize_t size, int64_t *j);

#endif/*LEX_H*/
