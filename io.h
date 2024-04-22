#ifndef X_IO_H
#define X_IO_H

#include <stdint.h>
#include <sys/types.h>

typedef void (*read_callback_t)(uint8_t *, const ssize_t);

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Open a file on the disk, determine it's size, and then
 *        read the entire file from the disk into memory.
 */
int readstream(uint8_t *buffer, const char *file_path, const ssize_t buflen, read_callback_t call);

/**
 * @brief Read a file from the disk.
 */
int readfile(void *buffer, const char *file_path);

#ifdef __cplusplus
}
#endif

#endif/*X_IO_H*/
