#ifndef X_IO_H
#define X_IO_H

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Read a file from the disk.
 */
int readfile(void *buffer, const char *file_path);

#ifdef __cplusplus
}
#endif

#endif/*X_IO_H*/
