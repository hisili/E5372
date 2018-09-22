#if (FEATURE_UPDATEONLINE == FEATURE_ON)

#ifndef  _OS_PLATFORM_H_
#define  _OS_PLATFORM_H_

#define OSAL_OK            (0)
#define OSAL_ERROR         (-1)

#ifdef WIN32

#include <stdio.h>
#include <fcntl.h>

#ifndef __user
#define __user
#endif

typedef unsigned int UINT32;

FILE *  os_file_open(const char __user *filename);
int  os_file_close(FILE * fd);

#else

//typedef long unsigned int size_t;

int  os_file_open(const char *filename);
int os_file_lseek(unsigned int fd,long offset);
long os_file_read(unsigned int fd, char *buf, int count);
long os_file_write(unsigned int fd, char *buf, int count);
unsigned int os_file_size(unsigned int fd);
int  os_file_close(unsigned int fd);

struct file * os_filp_open(const char *filename);
long os_vfs_read(struct file *file, char *buf, int count);
void os_filp_close(struct file *filp);

#endif//WIN32

#endif//_OS_PLATFORM_H_

#endif//FEATURE_UPDATEONLINE

