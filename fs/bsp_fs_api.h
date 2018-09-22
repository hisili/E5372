/*
 * This file contains the procedures for the handling of select and poll
 *
 * Created for Linux based loosely upon Mathius Lattner's minix
 * patches by Peter MacDonald. Heavily edited by Linus.
 *
 *  4 February 1994
 *     COFF/ELF binary emulation. If the process has the STICKY_TIMEOUTS
 *     flag set in its personality we do *not* modify the given timeout
 *     parameter to reflect time remaining.
 *
 *  24 January 2000
 *     Changed sys_poll()/do_poll() to use PAGE_SIZE chunk-based allocation
 *     of fds to overcome nfds < 16390 descriptors limit (Tigran Aivazian).
 */
#ifndef __BSP_FS_API__
#define __BSP_FS_API__

#if 0
extern long bsp_sys_creat(const char __user *pathname, int mode);

extern long bsp_sys_open(const char __user *filename,int flags, int mode);
extern long bsp_sys_close(unsigned int fd);
extern long bsp_sys_lseek(unsigned int fd, unsigned int offset,unsigned int origin);

extern long bsp_sys_read(unsigned int fd, char __user *buf, size_t count);

extern long bsp_sys_write(unsigned int fd, const char __user *buf,size_t count);

extern long bsp_sys_mkdir(const char __user *pathname, int mode);

extern long bsp_sys_rmdir(const char __user *pathname);
extern long bsp_sys_unlink(const char  *pathname);

extern long bsp_sys_getdents(unsigned int fd,
          struct linux_dirent __user *dirent,
          unsigned int count);
#endif
extern long bsp_sys_statfs(const char __user * path,
          struct statfs __user *buf);

//extern long bsp_sys_sync(void);

#endif
