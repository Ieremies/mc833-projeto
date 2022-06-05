#ifndef MC833_PROJETO_UTILS_H
#define MC833_PROJETO_UTILS_H

#define MAX_STR_LEN 50

void backup(void *__restrict src_ptr);

void load_backup(void *__restrict dest_ptr);

void sigchld_handler(int socket);

#endif // MC833_PROJETO_UTILS_H
